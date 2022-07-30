#include "SpriteGui.hpp"

// 参考 : 2Dのポリゴン描画の仕組みと方法
// http://marupeke296.com/NGDV_No3_Sprite.html

SpriteGui::SpriteGui
(
    LPCSTR      srcFile,
    D3DXVECTOR2 position,
    LONG        texWidth,
    LONG        texHeight,
    RECT        clippingRect,
    D3DXVECTOR2 pivotOffset,
    bool        adjustUV
) : Gui(position, texWidth, texHeight, pivotOffset), vertex_buffer(nullptr)
{
    // シェーダーを設定
    this->SetHLSL(HLSL::GetHLSLByTag("GUI-DEFAULT"));

    // ClippingRectが初期値の場合はテクスチャのサイズをそのまま
    if (clippingRect.left   == 0 &&
        clippingRect.top    == 0 &&
        clippingRect.right  == 0 &&
        clippingRect.bottom == 0)
    {   
        clippingRect = { 0, 0, texWidth, texHeight };
    }

    // サイズが2のべき乗かどうか
    bool isN2 =
        ((texWidth  & (texWidth  - 1)) == 0) &&
        ((texHeight & (texHeight - 1)) == 0);

    // 0指定の場合は作成時に2のべき乗扱いになるのでtrue
    isN2 = (texWidth == 0) && (texHeight == 0);

    // UV調節（テクスチャのサイズが2のべき乗でない場合UVを少しずらす）
    float uvAdjustValue = (isN2 && adjustUV) ? 0.0f : 0.0001f;

#pragma region 頂点バッファ作成

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
    const VertexPositionTexture VERTICES_BLOCK_SPRITE[4] =
    {
        VertexPositionTexture(-(clippingRect.right - clippingRect.left) * 0.5f, -(clippingRect.bottom - clippingRect.top) * 0.5f,  0.0f,      (float)clippingRect.left  / texWidth + uvAdjustValue, (float)clippingRect.top    / texHeight + uvAdjustValue), // 2
        VertexPositionTexture( (clippingRect.right - clippingRect.left) * 0.5f, -(clippingRect.bottom - clippingRect.top) * 0.5f,  0.0f,      (float)clippingRect.right / texWidth,                 (float)clippingRect.top    / texHeight + uvAdjustValue), // 3
        VertexPositionTexture(-(clippingRect.right - clippingRect.left) * 0.5f,  (clippingRect.bottom - clippingRect.top) * 0.5f,  0.0f,      (float)clippingRect.left  / texWidth + uvAdjustValue, (float)clippingRect.bottom / texHeight                ), // 0
        VertexPositionTexture( (clippingRect.right - clippingRect.left) * 0.5f,  (clippingRect.bottom - clippingRect.top) * 0.5f,  0.0f,      (float)clippingRect.right / texWidth,                 (float)clippingRect.bottom / texHeight                ), // 1
    };

    // ########## 頂点バッファ作成 ##########
    const UINT verts_length     = ARRAYSIZE(VERTICES_BLOCK_SPRITE);
    const UINT verts_array_size = sizeof(VertexPositionTexture) * verts_length;

    // 作成
    if (FAILED(DX9::instance->device3d->CreateVertexBuffer
    (
        verts_array_size,
        0,
        VertexPositionTexture::FVF,
        D3DPOOL_MANAGED,
        &this->vertex_buffer,
        nullptr
    ))) RELEASE_RETURN(this);

    void* pVertices = nullptr;
    this->vertex_buffer->Lock(0, verts_array_size, (void**)&pVertices, D3DLOCK_NOSYSLOCK);
    memcpy(pVertices, VERTICES_BLOCK_SPRITE, verts_array_size);
    this->vertex_buffer->Unlock();

#pragma endregion

#pragma region テクスチャ作成

    // テクスチャ作成
    if (!this->hlslParam.CreateTexture_Diffuse(srcFile, texWidth, texHeight))
    {
        OutputDebugFormat("[ SpriteGui ] ({}): テクスチャの作成に失敗しました", srcFile);
    }

#pragma endregion
}

SpriteGui::~SpriteGui()
{
    this->Release();
}

void SpriteGui::SetParam(ID3DXEffect* effect)
{
    // ワールド行列を設定
    this->GetWorldMatrix(&this->hlslParam.mat_World);
    effect->SetMatrix("mat_World", &this->hlslParam.mat_World);

    // 乗算色を設定
    this->hlslParam.mulColor = this->color;
    auto mulColor = this->hlslParam.GetMulColor();
    effect->SetVector("mulColor", &mulColor);

    // テクスチャを設定
    effect->SetTexture("tex_Diffuse", this->hlslParam.tex_Diffuse);
}

bool SpriteGui::Render(ID3DXEffect* effect)
{
    // Vertexを設定
    DX9::instance->device3d->SetStreamSource(0, this->vertex_buffer, 0, sizeof(VertexPositionTexture));

    // FVFを設定
    DX9::instance->device3d->SetFVF(VertexPositionTexture::FVF);

    // ポリゴン描画
    DX9::instance->device3d->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    return true;
}

void SpriteGui::Release()
{
    Gui::Release();

    SAFE_RELEASE(this->vertex_buffer);
}
