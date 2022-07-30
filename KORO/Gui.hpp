#include "HLSL.hpp"
#pragma once

/*==================================
 GUIのサイズを調節する基準となる画面サイズ
==================================*/
inline const D3DXVECTOR2 SCREEN_SIZE_TO_FIT = { 1920.0f, 1080.0f };

/*==================================
        Z-Indexの最大インデックス
==================================*/
inline const UINT MAX_Z_INDEX_i = 5U;

// Release()呼び出ししてreturn
#define RELEASE_RETURN(...) { __VA_ARGS__->Release(); return; }

// Release()呼び出ししてbreak
#define RELEASE_BREAK(...) { __VA_ARGS__->Release(); break; }

// D3DXVECTOR2のデフォルト値
#define V2_DEFAULT { 0.0f, 0.0f }

/*==================================
            頂点（位置、UV）
==================================*/
struct VertexPositionTexture
{
    float x, y, z; // Position
    float u, v;    // Texture Coords

    enum
    {
        FVF = D3DFVF_XYZ | D3DFVF_TEX1
    };
};

struct GuiInputState
{
    bool mouseOver;     // マウスホバー状態
    bool mouseOverPrev; // マウスホバー状態（前回の値）

    bool dragging;      // ドラッグ状態
};

/*==================================
              GUIクラス
==================================*/
class Gui : public HLSLObject, public GameObject
{
public:
    // Z-IndexでソートされたGUIリストを取得（降順）
    static std::vector<std::vector<HLSLObject*>> GetSortedByZIndexList();

    // ウィンドウサイズによるスケーリング値を取得
    const static float GetScalingValue();

public:
    // @param position      : 位置(%)
    // @param texWidth      : サイズ: 幅
    // @param texHeight     : サイズ: 高さ
    // @param (pivotOffset) : 位置ピボット
    Gui
    (
        D3DXVECTOR2 position,
        ULONG       width       = 0UL,
        ULONG       height      = 0UL,
        D3DXVECTOR2 pivotOffset = { 0.0f, 0.0f }
    );
    Gui(const Gui&) = delete;

public:
    // HLSL Virtual Functions

    virtual void SetParamOnce(ID3DXEffect* effect) override;

public:
    // メモリを解放する
    virtual void Release();

public:
    D3DXVECTOR2 position;    // 位置
    float       rotationDeg; // 回転
    float       scale;       // スケール
    UINT        z_index;     // Z-Index: 0~4 (0=手前, 4=奥)
    
    D3DXCOLOR   color;       // 色

    bool        visible;     // 可視状態

    // 初期位置
    D3DXVECTOR2 initialPos;

    // 初期スケール
    float       initialScale;

    // 位置ピボット
    D3DXVECTOR2 pivotOffset;

public:
    float width;  // サイズ: 幅
    float height; // サイズ: 高さ

    GuiInputState inputState;

    bool released;

public:
    // ワールド行列を取得
    // @param pOut : 出力先
    void GetWorldMatrix(D3DXMATRIX* pOut);

    // 射影行列を取得
    // @param pOut : 出力先
    void GetProjMatrix(D3DXMATRIX* pOut);

public:
    // パーセントXから位置Xを取得
    static float GetPosXFromPercent(float percent);

    // パーセントYから位置Yを取得
    static float GetPosYFromPercent(float percent);

    // パーセントX,Yから位置を取得
    static D3DXVECTOR2 GetPosFromPercent(float parcentX, float parcentY);

    // 位置X,Yからパーセント位置を取得
    static D3DXVECTOR2 GetPosPercentFromPosition(float posX, float posY);

    // 現在の位置からパーセント位置を取得
    D3DXVECTOR2 GetPercentPos();

    // 位置をパーセントで設定
    void SetPositionByPercent(float percentX, float percentY);

    // オフセットをパーセントで適用
    void AddOffsetPercent(float percentX, float percentY);

    // 初期位置を基準にオフセットをパーセントで設定
    void AddOffsetPercentFromInitialPosition(float percentX, float percentY);

    // 初期スケールを基準にスケールを加算
    void AddScaleValueFromInitialScale(float value);

    // 初期スケールを基準にスケールを設定
    void SetScaleValueFromInitialScale(float value);

public:
    // イベント取得
    void EventListener();

public:
    // On Mouse Cursor Hover
    virtual void OnMouseOver();

    // On Mouse Cursor Out
    virtual void OnMouseOut();

    // On Mouse Down
    virtual void OnMouseDown();

    // On Mouse Up
    virtual void OnMouseUp();

public:
    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;
};
