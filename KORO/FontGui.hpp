#include "Gui.hpp"
#include <tuple>
#pragma once

/*==================================
             Text-Align
==================================*/
enum class TextAlign
{
    RIGHT,  // 右
    CENTER, // 中心
    LEFT    // 左
};

/*==================================
           テキストボタンの色
==================================*/
enum TextButtonColor : DWORD
{
    DEFAULT = 0xFFB2B2B2, // デフォルト
    HOVERED = 0xFFFFFFFF, // ホバー時
    INVALID = 0xFF333333  // 無効時
};

/*==================================
         フォント描画のアイテム
    ・頂点バッファ
    ・テクスチャ
    ・横幅
==================================*/
#define FontRenderItem std::tuple<IDirect3DVertexBuffer9*, IDirect3DTexture9*, LONG>

#define IS_NEWLINE_TEX(...) (__VA_ARGS__ == (IDirect3DTexture9*)'\n')

/*==================================
          フォントテキストGUI
==================================*/
class FontGui : public Gui
{
public:
    // フォントテキストGUI
    // @param text        : テキスト
    // @param position    : 位置(%)
    // @param font_size   : フォントサイズpx
    // @param color       : 文字色
    // @param text_align  : TextAlign
    // @param font_family : フォント
    FontGui
    (
        LPCSTR      text,
        D3DXVECTOR2 position,
        LONG        font_size,
        D3DXCOLOR   color,
        TextAlign   text_align  = TextAlign::LEFT,
        bool        monospace   = false,
        LPCSTR      font_family = "源真ゴシック Heavy"
    );
    FontGui(const FontGui&) = delete;
    ~FontGui();

public:
    // HLSL Virtual Functions

    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

public:
    // テキストを設定（バッファやテクスチャを持ってくる）
    void SetText(string text);

public:
    string GetText();
    LONG   GetFontSize();

public:
    virtual void OnMouseUp() override;

public:
    // テキスト
    string text;

    // font-size
    LONG font_size;

    // text-align
    TextAlign text_align;

    // monospace
    bool monospace;

    // フォント
    string font_family;

    // 複数行
    bool multiLine;

public:
    // テキストボタンかどうか
    bool isTextButton;

    // 有効なテキストボタンかどうか
    bool isValidTextButton;

    // テキストボタンの強調矢印を有効にするかどうか
    bool enableTextButtonArrow;

public:
    // 描画用のアイテムリスト: (頂点バッファ, テクスチャ, 横幅)
    std::vector<FontRenderItem> renderItems;
};
