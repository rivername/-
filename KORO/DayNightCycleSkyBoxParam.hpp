#include "SkyBox.hpp"
#include <map>
#pragma once

#undef min

// 誤差を考慮したfloat同士の==演算（甘め判定）
#define FLOAT_EQ(x, v) (((v - 0.01f) < x) && (x < ( v + 0.01f)))

/*==================================
           SkyBoxパラメーター
==================================*/
namespace SkyBoxParam
{

/*==================================
          SkyBoxサイクルモード
==================================*/
enum class SkyBoxCycleMode : byte
{
    SYNC_LOCAL_DATETIME, // 現在時刻と同期
    USER_PREFERENCE      // ユーザーが自由に設定
};

/*==================================
          SkyBoxカラーシーン
==================================*/
enum class SkyBoxColorScene
{
    EARLY_MORNING, // 早朝
    SUNRISE,       // 夜明け
    MORNING,       // 朝
    DAY,           // 昼
    SUNSET,        // 夕方
    NIGHT,         // 夜
    MIDNIGHT,      // 深夜

    __num
};

/*==================================
        色マップ : skybox_color
==================================*/
inline std::map<SkyBoxColorScene, D3DXCOLOR> SKYBOX_COLOR_COLOR =
{
    { SkyBoxColorScene::EARLY_MORNING, D3DCOLOR_XRGB(  0,  47, 105) }, // 早朝
    { SkyBoxColorScene::SUNRISE,       D3DCOLOR_XRGB( 82, 173, 247) }, // 夜明け
    { SkyBoxColorScene::MORNING,       D3DCOLOR_XRGB( 76, 173, 252) }, // 朝
    { SkyBoxColorScene::DAY,           D3DCOLOR_XRGB( 75, 133, 255) }, // 昼
    { SkyBoxColorScene::SUNSET,        D3DCOLOR_XRGB( 75, 128, 201) }, // 夕方
    { SkyBoxColorScene::NIGHT,         D3DCOLOR_XRGB( 14,  15,  46) }, // 夜
    { SkyBoxColorScene::MIDNIGHT,      D3DCOLOR_XRGB(  1,   1,  20) }  // 深夜
};

/*==================================
       色マップ : skybox_filter
==================================*/
inline std::map<SkyBoxColorScene, D3DXCOLOR> SKYBOX_FILTER_COLOR =
{
    { SkyBoxColorScene::EARLY_MORNING, D3DCOLOR_XRGB(116, 175, 174) }, // 早朝
    { SkyBoxColorScene::SUNRISE,       D3DCOLOR_XRGB(255, 142,  43) }, // 夜明け
    { SkyBoxColorScene::MORNING,       D3DCOLOR_XRGB(252, 238,  33) }, // 朝
    { SkyBoxColorScene::DAY,           D3DCOLOR_XRGB(140, 255, 115) }, // 昼
    { SkyBoxColorScene::SUNSET,        D3DCOLOR_XRGB(255,  85,   0) }, // 夕方
    { SkyBoxColorScene::NIGHT,         D3DCOLOR_XRGB(106,  77, 255) }, // 夜
    { SkyBoxColorScene::MIDNIGHT,      D3DCOLOR_XRGB(  0,  70, 135) }  // 深夜
};

/*==================================
      色マップ : skybox_sun_color
==================================*/
inline std::map<SkyBoxColorScene, D3DXCOLOR> SKYBOX_SUN_COLOR =
{
    { SkyBoxColorScene::EARLY_MORNING, D3DCOLOR_ARGB(  0,   0,   0,   0) }, // 早朝
    { SkyBoxColorScene::SUNRISE,       D3DCOLOR_XRGB(     252,  86,   9) }, // 夜明け
    { SkyBoxColorScene::MORNING,       D3DCOLOR_XRGB(     255, 148,  54) }, // 朝
    { SkyBoxColorScene::DAY,           D3DCOLOR_XRGB(     252, 192,   9) }, // 昼
    { SkyBoxColorScene::SUNSET,        D3DCOLOR_XRGB(     252,  49,   9) }, // 夕方
    { SkyBoxColorScene::NIGHT,         D3DCOLOR_ARGB(  0,   0,   0,   0) }, // 夜
    { SkyBoxColorScene::MIDNIGHT,      D3DCOLOR_ARGB(  0,   0,   0,   0) }  // 深夜
};

/*==================================
        SkyBoxオフセットモード
==================================*/
enum class SkyBoxOffsetMode
{
    IN_TITLE, // タイトル画面
    CLEARED,  // クリアしたとき
    __default // デフォルト
};

/*==================================
             SkyBoxデータ
==================================*/
struct SkyBoxData
{
    SkyBoxCycleMode cycleMode;  // サイクルモード
    byte            slideValue; // スカイボックスコントローラーのスライド位置
};

/*==================================
            DateTime構造体
==================================*/
struct DateTime
{
    int month;
    int day;

    int hour;
    int min;
    int sec;

    DateTime() {}

    DateTime(const tm dt)
    {
        this->month = dt.tm_mon + 1;
        this->day   = dt.tm_mday;
        this->hour  = dt.tm_hour;
        this->min   = dt.tm_min;
        this->sec   = dt.tm_sec;
    }
    DateTime(const int month, const int day) : hour(0), min(0), sec(0)
    {
        this->month = month;
        this->day   = day;
    }
    DateTime(const int hour, const int min, const int sec) : month(0), day(0)
    {
        this->hour = hour;
        this->min  = min;
        this->sec  = sec;
    }
};

/*==================================
    DateTimeマップ : ユーザー設定用
==================================*/
inline std::map<SkyBoxColorScene, DateTime> USER_PREFERENCE_DATETIME_BY_COLOR_SCENE =
{
    { SkyBoxColorScene::EARLY_MORNING, DateTime(04, 00, 00) }, // 早朝
    { SkyBoxColorScene::SUNRISE,       DateTime(06, 00, 00) }, // 夜明け
    { SkyBoxColorScene::MORNING,       DateTime(10, 30, 00) }, // 朝
    { SkyBoxColorScene::DAY,           DateTime(13, 00, 00) }, // 昼
    { SkyBoxColorScene::SUNSET,        DateTime(18, 00, 00) }, // 夕方
    { SkyBoxColorScene::NIGHT,         DateTime(19, 00, 00) }, // 夜
    { SkyBoxColorScene::MIDNIGHT,      DateTime(23, 00, 00) }  // 深夜
};

// 1シーンごとの回転量
inline const float rotationAmo = 360.0f / (float)SkyBoxColorScene::__num;

// 一時間で回転する量
inline const float rotationAmoInAHour = 360.0f / 24;

// 回転量方向（360度以上の回転に対応するための変数）
inline int rotatedCountDir = 0;

// デフォルトInterpスピード
inline const float defaultInterpSpeed = 0.1f;

// 回転用Interpスピード
inline float rotateInterpSpeed = 2.0f;

// 現在の日付時刻
inline DateTime currentDateTime;

// 環境光色
inline D3DXCOLOR ambientLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

} // namespace SkyBoxParam

using SkyBoxParam::SkyBoxCycleMode;
using SkyBoxParam::SkyBoxColorScene;
using SkyBoxParam::SKYBOX_COLOR_COLOR;
using SkyBoxParam::SKYBOX_FILTER_COLOR;
using SkyBoxParam::SKYBOX_SUN_COLOR;
using SkyBoxParam::USER_PREFERENCE_DATETIME_BY_COLOR_SCENE;
using SkyBoxParam::SkyBoxOffsetMode;
using SkyBoxParam::SkyBoxData;
using SkyBoxParam::DateTime;
