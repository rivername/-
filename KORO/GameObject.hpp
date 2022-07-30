#include "const.hpp"
#include "utils.hpp"
#include "Transform.hpp"
#include "lib/Neargye/nameof.hpp"
#include <XInput.h>
#pragma once

/*==================================
             キーイベント
==================================*/
struct KeyEvent
{
    const int  keyCode;   // キー・キーコード
    const bool withShift; // Shift同時押し
    const bool withCtrl;  // Ctrl同時押し
};

/*==================================
             マウスイベント
==================================*/
struct MouseEvent
{
    // マウスボタン
    enum class Button
    {
        LEFT,
        RIGHT,
        MIDDLE
    };

    const Button button;         // マウスボタン
    const POINT  position;       // マウス座標
    const bool   withShift;      // Shift同時押し
    const bool   withCtrl;       // Ctrl同時押し
};

/*==================================
           オブジェクトタイプ
==================================*/
enum class ObjectType
{
    CAMERA,
    CUBE,
    STAGE,
    KORO,
    GUI,
    CANVAS,
    GRAVITY,
    GAME,
    NONE
};

/*==================================
          ゲームパッド軸イベント
==================================*/
struct GamepadAxisEvent
{
    struct L          // Lスティック
    {
        float x_Axis; // X軸 -1.0 ~ 1.0
        float y_Axis; // Y軸 -1.0 ~ 1.0
    } L;
    struct R          // Rスティック
    {
        float x_Axis; // X軸 -1.0 ~ 1.0
        float y_Axis; // Y軸 -1.0 ~ 1.0
    } R;

    struct Trigger    // トリガー
    {
        float l_axis; // 左軸 0.0 ~ 1.0
        float r_axis; // 右軸 0.0 ~ 1.0
    } Trigger;
};

/*==================================
        ゲームパッドボタンイベント
==================================*/
struct GamepadButtonEvent
{
    // ボタン
    enum class Button
    {
        A     = XINPUT_GAMEPAD_A,              // A
        B     = XINPUT_GAMEPAD_B,              // B
        X     = XINPUT_GAMEPAD_X,              // X
        Y     = XINPUT_GAMEPAD_Y,              // Y
        START = XINPUT_GAMEPAD_START,          // START
        BACK  = XINPUT_GAMEPAD_BACK,           // BACK
        LB    = XINPUT_GAMEPAD_LEFT_SHOULDER,  // Lボタン
        RB    = XINPUT_GAMEPAD_RIGHT_SHOULDER, // Rボタン
        LS    = XINPUT_GAMEPAD_LEFT_THUMB,     // Lスティック
        RS    = XINPUT_GAMEPAD_RIGHT_THUMB,    // Rスティック
        UP    = XINPUT_GAMEPAD_DPAD_UP,        // 上ボタン
        DOWN  = XINPUT_GAMEPAD_DPAD_DOWN,      // 下ボタン
        LEFT  = XINPUT_GAMEPAD_DPAD_LEFT,      // 左ボタン
        RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT      // 右ボタン
    };

    // ボタン
    Button button;
};

/*==================================
           ゲームオブジェクト
==================================*/
class GameObject
{
public:
    // 全ゲームオブジェクト
    inline static std::vector<SmartPointer(GameObject)> all;

public:
    GameObject(ObjectType objectType = ObjectType::NONE);
    virtual ~GameObject() = default;

public:
    static void BaseUpdate();

private:
    // 入力取得
    static void GetInput();

    // キーボード・マウス入力
    static void GetInput_RAW();

    // ゲームパッド入力
    static void GetInput_GAMEPAD();

public:
    // ゲームパッドが接続されているかどうか
    static bool& IsGamepadConnected();

public:
    // Event Begin Play
    virtual void EventBeginPlay() = 0;

    // Event Render
    virtual void EventRender() {}

    // Event Tick
    virtual void EventTick(float deltaTime) = 0;

    // Event Key Pressed
    virtual void EventKeyDown(KeyEvent e) = 0;

    // Event Key Released
    virtual void EventKeyUp(KeyEvent e) = 0;

    // Event Mouse Clicked
    virtual void EventMouseDown(MouseEvent e) = 0;

    // Event Mouse Click Released
    virtual void EventMouseUp(MouseEvent e) = 0;

    // Event Mouse Wheel Move
    // @param delta : -1 or 1
    virtual void EventMouseWheelMove(int delta) {}

    // Event Gamepad Axis
    virtual void EventGamepadAxisInput(GamepadAxisEvent e) {}

    // Event Gamepad Button Pressed
    virtual void EventGamepadButtonPressed(GamepadButtonEvent e) {}

    // Event Gamepad Button Released
    virtual void EventGamepadButtonReleased(GamepadButtonEvent e) {}

public:
    // 入力を有効にする
    void EnableInput();

    // 入力を無効にする
    void DisableInput();

    // 有効なオブジェクトかどうか
    bool IsValid();

public:
    // 解放する
    void Release();

public:
    ObjectType objectType;

    bool isEnableInput;

    bool beganPlay;
};
