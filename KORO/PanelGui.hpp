#include "Gui.hpp"

/*==================================
            ボーダータイプ
==================================*/
enum class BorderType : byte
{
    DEFAULT,   // デフォルト
    ELLIPSE,   // 楕円
    INNER_MASK // 内側をマスクする
};

/*==================================
             パネルカラー
==================================*/
enum PanelColor : DWORD
{
    WHITE   = 0xFFFFFFFF, // 白
    BLACK   = 0xFF000000, // 黒
    ALPHA50 = 0x80000000  // 不透明度50%の黒
};

/*==================================
             パネルGUI
==================================*/
class PanelGui : public Gui
{
public:
    // パネルGUI
    // @param position     : 位置(%)
    // @param width        : サイズ: 幅(FHD基準)
    // @param height       : サイズ: 高さ(FHD基準)
    // @param color        : 色
    // @param borderType   : ボーダータイプ
    // @param borderRadius : ボーダー半径
    PanelGui
    (
        D3DXVECTOR2 position,
        LONG        width,
        LONG        height,
        D3DXCOLOR   color,
        BorderType  borderType   = BorderType::DEFAULT,
        float       borderRadius = 0.0f,
        D3DXVECTOR2 pivotOffset  = { 0.0f, 0.0f }
    );

    PanelGui(const PanelGui&) = delete;
    ~PanelGui();

public:
    // HLSL Virtual Functions

    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

public:
    // メモリを解放する
    virtual void Release() override;

public:
    // ボーダータイプ
    BorderType borderType;

    // ボーダー半径 : 外側
    float outerBorderRadius;

    // ボーダー半径 : 内側
    float innerBorderRadius;

private:
    // 頂点バッファ
    IDirect3DVertexBuffer9* vertex_buffer;
};
