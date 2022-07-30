#include "Gui.hpp"
#include "HLSL.hpp"
#pragma once

/*==================================
           スプライトGUI
==================================*/
class SpriteGui : public Gui
{
public:
    // スプライトGUI
    // @param srcFile        : 画像ファイルパス
    // @param position       : 位置(%)
    // @param texWidth       : テクスチャ幅
    // @param texHeight      : テクスチャ高さ
    // @param widthInScreen  : スクリーン上での幅
    // @param heightInScreen : スクリーン上での高さ
    // @param (clippingRect  : 切り抜きRECT)
    // @param (pivotOffset)  : 位置ピボット
    // @param (adjustUV)     : UVを調節するかどうか
    SpriteGui
    (
        LPCSTR      srcFile,
        D3DXVECTOR2 position,
        LONG        texWidth,
        LONG        texHeight,
        RECT        clippingRect = {},
        D3DXVECTOR2 pivotOffset  = { 0.0f, 0.0f },
        bool        adjustUV     = true
    );

    SpriteGui(const SpriteGui&) = delete;
    ~SpriteGui();

public:
    // HLSL Virtual Functions

    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

public:
    // メモリを解放する
    virtual void Release() override;

private:
    // 頂点バッファ
    IDirect3DVertexBuffer9* vertex_buffer;
};
