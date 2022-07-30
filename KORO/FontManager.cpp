#include "FontManager.hpp"

// 参考 : フォントをリソースファイルから読み込む方法
// https://ohwhsmm7.blog.fc2.com/blog-entry-322.html

// 参考 : フォントをテクスチャとして描画する
// http://marupeke296.com/DXGSmp_No5_Font.html

// 参考 : TEXTMETRICの詳細
// http://phys.cool.coocan.jp/physjpn/htextmetric.htm

void FontManager::ResourceManager::LoadFontFromResource(int resourceID)
{
    HRSRC  hResFont = FindResourceA(nullptr, MAKEINTRESOURCEA(resourceID), RT_FONT);
    HANDLE hRes     = LoadResource (nullptr, hResFont);
    LPVOID pFont    = LockResource(hRes);
    DWORD  fontSize = SizeofResource(nullptr, hResFont);

    DWORD num = 0;

    HANDLE font = AddFontMemResourceEx(pFont, fontSize, nullptr, &num);

    if (font) FontManager::ResourceManager::fonts.push_back(font);
}

void FontManager::ResourceManager::ReleaseAllFontResource()
{
    for (auto& font : FontManager::ResourceManager::fonts)
    {
        RemoveFontMemResourceEx(font);
    }

    FontManager::ResourceManager::fonts.clear();
}

void FontManager::RenderManager::OutputRenderItemsOfString(LPCSTR text, FontGui* pOut)
{
    if (strlen(text) <= 0) return;

    pOut->renderItems.clear();

#pragma region フォント作成

    // フォント作成
    LOGFONTA lf =
    {
        pOut->font_size, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
        CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN,
        
        // フォント名は後からコピーするのでNULL
        (char)nullptr
    };

    // フォント名はこれでコピーするしかない
    strcpy_s(lf.lfFaceName, LF_FACESIZE, pOut->font_family.c_str());

    HFONT hFont;

    if (!(hFont = CreateFontIndirectA(&lf)))
    {
        OutputDebugFormat("[FontGui] ({}): フォントの作成に失敗しました\n", text);
        RELEASE_RETURN(pOut);
    }

#pragma endregion

#pragma region フォント画像作成

    // デバイスコンテキスト取得
    // デバイスにフォントを持たせる
    HDC   hdc     = GetDC(nullptr);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    // 文字コード
    UINT code = 0U;

    for (size_t i = 0; i < strlen(text); i++)
    {
        // マルチバイト文字の場合
        if (IsDBCSLeadByte(text[i]))
        {
            // 2文字分取り出し
            code = ((byte)text[i] << 8 | (byte)text[i + 1]);

            // もう一文字分進める
            i++;
        }
        // それ以外はそのまま
        else
        {
            code = text[i];

            if (code == '\n')
            {
                // テクスチャとして改行コードを追加
                pOut->renderItems.push_back({ nullptr, (IDirect3DTexture9*)'\n', 0});
                continue;
            }
        }

        FontRenderItem item;

        // 既にロード済の場合はロード済を追加
        if (FontManager::RenderManager::IsTheCodeAlreadyLoaded(code, pOut->font_size, &item))
        {
            pOut->renderItems.push_back(item);
            continue;
        }

        // フォントビットマップ取得
        TEXTMETRICA tm;
        GetTextMetricsA(hdc, &tm);

        GLYPHMETRICS gm;

        const MAT2 mat =
        {
            { 0, 1 },
            { 0, 0 },
            { 0, 0 },
            { 0, 1 }
        };

        DWORD size =
            GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, nullptr, &mat);
        byte* ptr  = new byte[size];
            GetGlyphOutlineA(hdc, code, GGO_GRAY4_BITMAP, &gm, size, ptr, &mat);

        const UINT texWidth  = gm.gmCellIncX;
        const UINT texHeight = tm.tmHeight;

        // 頂点
        // 2D射影行列の性質により上下が逆さになるので
        // 0   1
        // 
        // 2   3 を
        // -----
        // 2   3
        // 
        // 0   1 に変更
        // UVはそのまま
        VertexPositionTexture VERTICES_BLOCK_FONT[4] =
        {
            VertexPositionTexture(-(texWidth * 0.5f), -(texHeight * 0.5f), 0.0f,     0.0f, 0.0f),
            VertexPositionTexture( (texWidth * 0.5f), -(texHeight * 0.5f), 0.0f,     1.0f, 0.0f),
            VertexPositionTexture(-(texWidth * 0.5f),  (texHeight * 0.5f), 0.0f,     0.0f, 1.0f),
            VertexPositionTexture( (texWidth * 0.5f),  (texHeight * 0.5f), 0.0f,     1.0f, 1.0f)
        };

        // ########## 頂点バッファ作成 ##########
        const UINT verts_length     = ARRAYSIZE(VERTICES_BLOCK_FONT);
        const UINT verts_array_size = sizeof(VertexPositionTexture) * verts_length;

        IDirect3DVertexBuffer9* vertex_buffer = nullptr;

        // 作成
        if (FAILED(DX9::instance->device3d->CreateVertexBuffer
        (
            verts_array_size,
            0,
            VertexPositionTexture::FVF,
            D3DPOOL_MANAGED,
            &vertex_buffer,
            nullptr
        ))) RELEASE_BREAK(pOut);

        void* pVertices = nullptr;
        vertex_buffer->Lock(0, verts_array_size, (void**)&pVertices, D3DLOCK_NOSYSLOCK);
        memcpy(pVertices, VERTICES_BLOCK_FONT, verts_array_size);
        vertex_buffer->Unlock();

        // ########## テクスチャ作成 ##########
        IDirect3DTexture9* texture = nullptr;

        // 空のテクスチャを動的モードで作成
        if (FAILED(D3DXCreateTexture
        (
            DX9::instance->device3d,
            texWidth, texHeight,
            1, D3DUSAGE_DYNAMIC /* 動的モード（後からテクスチャのサイズを変更可能） */,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
            &texture
        ))) RELEASE_BREAK(pOut);

        // テクスチャにフォントビットマップを書き込む
        D3DLOCKED_RECT lockedRect = {};

        if (FAILED(texture->LockRect
        (
            0, &lockedRect,
            nullptr, D3DLOCK_DISCARD
        ))) RELEASE_BREAK(pOut);

        int iOfs_x = gm.gmptGlyphOrigin.x;
        int iOfs_y = tm.tmAscent - gm.gmptGlyphOrigin.y;
        int iBmp_w = gm.gmBlackBoxX + (4 - (gm.gmBlackBoxX % 4)) % 4;
        int iBmp_h = gm.gmBlackBoxY;
        int level  = 17;

        DWORD alpha, color;

        FillMemory(lockedRect.pBits, lockedRect.Pitch * tm.tmHeight, 0);

        // 1ピクセルずつ書き込み
        for (int y = iOfs_y; y < iOfs_y + iBmp_h; y++)
        for (int x = iOfs_x; x < iOfs_x + (int)gm.gmBlackBoxX; x++)
        {
            // 空白文字はきれいな透明にする
            if (code == 0x20)
            {
                color = 0x00;
            }
            // それ以外はフォント通り
            else
            {
                alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (level - 1);
                color = 0x00FFFFFF | (alpha << 24);
            }

            memcpy((byte*)lockedRect.pBits + lockedRect.Pitch * y + 4 * x, &color, sizeof(DWORD));
        }

        texture->UnlockRect(0);
        delete[] ptr;

        // 文字幅取得
        LONG charWidth = static_cast<LONG>(texWidth >= pOut->font_size * 0.5f ? texWidth : pOut->font_size * 0.5f);

        // 描画アイテムtuple
        item = { vertex_buffer, texture, !pOut->monospace ? charWidth : static_cast<LONG>(pOut->font_size * 0.75f) };

        // アイテム追加
        pOut->renderItems.push_back(item);

        // 読み込み済みに追加
        FontManager::RenderManager::loadedRenderItemsEx.push_back({ code, pOut->font_size, item });

        // スケーリングされたheight設定
        pOut->height = max(texHeight, pOut->height);
    }

    pOut->renderItems.shrink_to_fit();

#pragma endregion

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    ReleaseDC(nullptr, hdc);
}

bool FontManager::RenderManager::IsTheCodeAlreadyLoaded(UINT code, LONG font_size, FontRenderItem* itemIfCodeLoadedIn)
{
    for (auto& itemEx : FontManager::RenderManager::loadedRenderItemsEx)
    {
        auto& [CODE, FONT_SIZE, item] = itemEx;

        if (code      == CODE)
        if (font_size == FONT_SIZE)
        {
            *itemIfCodeLoadedIn = item;
            return true;
        }
    }

    return false;
}

void FontManager::RenderManager::ReleaseAllRenderItems()
{
    for (auto& itemEx : FontManager::RenderManager::loadedRenderItemsEx)
    {
        auto& [ex1, ex2, item] = itemEx;
        auto& [vertex_buffer, texture, size] = item;

        SAFE_RELEASE(vertex_buffer);
        SAFE_RELEASE(texture);
    }
}
