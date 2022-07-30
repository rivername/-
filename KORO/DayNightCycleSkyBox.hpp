#include "DayNightCycleSkyBoxParam.hpp"
#pragma once

/*==================================
          昼夜サイクルSkyBox
        ・6層（5層）になっている
   - skybox_color
   - skybox_filter
   - skybox_star
   - skybox_moon, skybox_sun_color
   - skybox_sun_flare
==================================*/
class DayNightCycleSkyBox : public GameObject
{
public:
    inline static DayNightCycleSkyBox* instance;

public:
    DayNightCycleSkyBox();
    ~DayNightCycleSkyBox();

public:
    SkyBox* skybox_color;     // 空の色用SkyBox
    SkyBox* skybox_filter;    // フィルター用SkyBox
    SkyBox* skybox_star;      // 星用SkyBox
    SkyBox* skybox_moon;      // 月用SkyBox
    SkyBox* skybox_sun_color; // 太陽の色用SkyBox
    SkyBox* skybox_sun_flare; // 太陽のフレア用SkyBox

private:
    // 時間によるSkyBoxの色の変化の実装
    void SetSkyBoxColorByCurrentDateTimeVar();

    // SkyBoxのmulColor専用InterpTo
    // @param target      : 適用先SkyBox
    // @param current     : 現在の色
    // @param to          : 目標の色
    // @param interpSpeed : スピード
    // @return { bool }   : Interp完了
    bool ColorInterpTo(SkyBox* target, D3DXCOLOR current, D3DXCOLOR to, float interpSpeed);

    // D3DXCOLOR専用InterpTo
    // @param out         : 出力先
    // @param current     : 現在の色
    // @param to          : 目標の色
    // @param interpSpeed : スピード
    // @return { bool }   : Interp完了
    bool ColorInterpTo(D3DXCOLOR* out, D3DXCOLOR current, D3DXCOLOR to, float interpSpeed);

    // D3DXCOLOR専用Lerp
    // @return { D3DXCOLOR } : 補間された色
    D3DXCOLOR LerpColor(D3DXCOLOR from, D3DXCOLOR to, float alpha);

public:
    // SkyBox位置・回転を設定
    void SetTransform();

    // SkyBoxサイクルモードを設定
    void SetCycleMode(SkyBoxCycleMode cycleMode);

    // ユーザー設定のSkyBoxコントローラースライド値を設定
    void AddSkyBoxControllerSlide(UINT slideValue);

    // オフセットモードを設定
    void SetOffsetMode(SkyBoxOffsetMode offsetMode);

private:
    // SkyBoxデータ
    SkyBoxData skyboxData;

    // 色補間速度
    float interpSpeed;

    // SkyBoxオフセットモード
    SkyBoxOffsetMode offsetMode;

public:
    virtual void EventBeginPlay()             override;
    virtual void EventTick(float deltaTime)   override;
    virtual void EventKeyDown(KeyEvent e)     override {}
    virtual void EventKeyUp(KeyEvent e)       override {}
    virtual void EventMouseDown(MouseEvent e) override {}
    virtual void EventMouseUp(MouseEvent e)   override {}
};
