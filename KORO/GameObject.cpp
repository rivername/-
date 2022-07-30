#include "GameObject.hpp"
#include "Window.hpp"
#include "Gui.hpp"
#include "Debug.hpp"
#include "Game.hpp"
#include "LevelEditor.hpp"

#include <algorithm>
#include <map>

GameObject::GameObject(ObjectType objectType) : objectType(objectType), isEnableInput(false), beganPlay(false)
{
    GameObject::all.push_back(SmartPointer(GameObject)(this));
}

void GameObject::BaseUpdate()
{
    // 最初に描画
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (!GameObject::all[i]->IsValid()) continue;

        auto gameObject = GameObject::all[i].get();

        if (gameObject->beganPlay) gameObject->EventRender();
    }

    // 最初に描画

    // ウィンドウフォーカス時のみ入力を取得
    if (Window::instance->focused)
    {
        GameObject::GetInput();
    }

    // DeltaTime計算
    // 参考 : http://www.mcihanozer.com/tips/c/calculating-delta-time/

    static std::chrono::steady_clock::time_point prev;
    static std::chrono::steady_clock::time_point current;

    prev    = current;
    current = std::chrono::steady_clock::now();

    float deltaTime = ((current - prev) * 0.000000001f).count();

    // EventTick
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (!GameObject::all[i]->IsValid()) continue;

        auto gameObject = GameObject::all[i].get();

        if (!gameObject->beganPlay)
        {
            gameObject->EventBeginPlay();
            gameObject->beganPlay = true;
        }
        else
        {
            gameObject->EventTick(Window::instance->deltaTime = deltaTime);
        }
    }
}

void GameObject::GetInput()
{
    // 元栓確認
    if (!Game::LISTEN_EVENT_ROOT && LevelEditor::instance == nullptr) return;

    GameObject::GetInput_RAW();
    GameObject::GetInput_GAMEPAD();
}

void GameObject::GetInput_RAW()
{
    // -----------------------------------------------------------------------
    // Get Keyboard Down & Up

    // 同時押しチェック
    bool withShift = GetKeyState(VK_SHIFT) < 0;
    bool withCtrl  = GetKeyState(VK_CONTROL) < 0;

    // キーの状態保存 // 一般的に使われるキー 0xE2(226)個保存
    static bool KEY_PRESSED[0xE2] = {};

    // 全キーループ // 5 = マウス入力の次のキー
    for (int key = 5; key <= ARRAYSIZE(KEY_PRESSED); key++)
    {
        // キーが押された時
        if (GetKeyState(key) < 0)
        {
            // イベント発生
            if (!KEY_PRESSED[key])
            for (int i = 0; i < GameObject::all.size(); i++)
            {
                // 無効なオブジェクトはスキップ
                if (!GameObject::all[i]->IsValid()) continue;

                // GUIの場合、visibleかどうか
                if (GameObject::all[i]->objectType == ObjectType::GUI)
                if (!(static_cast<Gui*>(GameObject::all[i].get())->visible))
                {
                    continue;
                }

                if (GameObject::all[i]->IsValid())
                if (GameObject::all[i]->isEnableInput)
                GameObject::all[i]->EventKeyDown(KeyEvent
                {
                    .keyCode   = key,
                    .withShift = withShift,
                    .withCtrl  = withCtrl
                });
            }

            KEY_PRESSED[key] = true;
        }
        // キーが離された時
        else if (KEY_PRESSED[key])
        {
            for (size_t i = 0; i < GameObject::all.size(); i++)
            {
                if (!GameObject::all[i]->IsValid()) continue;

                auto gameObject = GameObject::all[i].get();

                // GUIの場合、visibleかどうか
                if (gameObject->objectType == ObjectType::GUI)
                if (!(static_cast<Gui*>(gameObject)->visible))
                {
                    continue;
                }

                if (gameObject->IsValid())
                if (gameObject->isEnableInput)
                gameObject->EventKeyUp(KeyEvent
                {
                    .keyCode   = key,
                    .withShift = withShift,
                    .withCtrl  = withCtrl
                });
            }

            KEY_PRESSED[key] = false;
        }
    }
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Mouse Down & Up

    static int MOUSE_BUTTONS[] =
    {
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON
    };

    static bool MOUSE_PRESSED[ARRAYSIZE(MOUSE_BUTTONS)] = {};

    // カーソールの位置
    const POINT cursor = Window::instance->GetMouseCursorPos();

    // ボタン数分ループ
    for (int i = 0; i < ARRAYSIZE(MOUSE_BUTTONS); i++)
    {
        // クリックされた時
        if (GetKeyState(MOUSE_BUTTONS[i]) < 0)
        {
            // イベント発生
            if (!MOUSE_PRESSED[i])
            for (size_t j = 0; j < GameObject::all.size(); j++)
            {
                if (!GameObject::all[j]->IsValid()) continue;

                auto gameObject = GameObject::all[j].get();

                // GUIの場合、visibleかどうか
                if (gameObject->objectType == ObjectType::GUI)
                if (!(static_cast<Gui*>(gameObject)->visible))
                {
                    continue;
                }

                if (gameObject->isEnableInput)
                gameObject->EventMouseDown(MouseEvent
                {
                    .button    = (MouseEvent::Button)i,
                    .position  = cursor,
                    .withShift = withShift,
                    .withCtrl  = withCtrl
                });
            }

            MOUSE_PRESSED[i] = true;
        }
        // クリックが離された時
        else
        {
            if (MOUSE_PRESSED[i])
            for (size_t j = 0; j < GameObject::all.size(); j++)
            {
                if (!GameObject::all[j]->IsValid()) continue;

                auto gameObject = GameObject::all[j].get();

                // GUIの場合、visibleかどうか
                if (gameObject->objectType == ObjectType::GUI)
                if (!(static_cast<Gui*>(gameObject)->visible))
                {
                    continue;
                }

                if (gameObject->isEnableInput)
                gameObject->EventMouseUp(MouseEvent
                {
                    .button    = (MouseEvent::Button)i,
                    .position  = cursor,
                    .withShift = withShift,
                    .withCtrl  = withCtrl
                });
            }

            MOUSE_PRESSED[i] = false;
        }
    }
    // -----------------------------------------------------------------------
}

void GameObject::GetInput_GAMEPAD()
{
    // XINPUT_STATE取得
    XINPUT_STATE state;

    // ゲームパッドが接続されてない場合は取得しない
    if (XInputGetState(0, &state) == ERROR_DEVICE_NOT_CONNECTED)
    {
        GameObject::IsGamepadConnected() = false;
        return;
    }

    GameObject::IsGamepadConnected() = true;

    // デッドゾーン判定
    if (state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  && state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)  state.Gamepad.sThumbLX = 0;
    if (state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  && state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)  state.Gamepad.sThumbLY = 0;
    if (state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) state.Gamepad.sThumbRX = 0;
    if (state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) state.Gamepad.sThumbRY = 0;

    // 値を正規化
    float l_X_Axis = state.Gamepad.sThumbLX / 32767.0f; // ノーマライズ
    l_X_Axis       = std::clamp(l_X_Axis, -1.0f, 1.0f); // -1.0f ~ 1.0fに制限

    float l_Y_Axis = state.Gamepad.sThumbLY / 32767.0f;
    l_Y_Axis       = std::clamp(l_Y_Axis, -1.0f, 1.0f);

    float r_X_Axis = state.Gamepad.sThumbRX / 32767.0f;
    r_X_Axis       = std::clamp(r_X_Axis, -1.0f, 1.0f);

    float r_Y_Axis = state.Gamepad.sThumbRY / 32767.0f;
    r_Y_Axis       = std::clamp(r_Y_Axis, -1.0f, 1.0f);

    float l_Trigger_Axis = state.Gamepad.bLeftTrigger  / 255.0f;
    float r_Trigger_Axis = state.Gamepad.bRightTrigger / 255.0f;

    // イベント発生
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (!GameObject::all[i]->IsValid()) continue;

        auto gameObject = GameObject::all[i].get();

        // GUIの場合、visibleかどうか
        if (gameObject->objectType == ObjectType::GUI)
        if (!(static_cast<Gui*>(gameObject)->visible))
        {
            continue;
        }

        if (gameObject->isEnableInput)
        gameObject->EventGamepadAxisInput(GamepadAxisEvent
        {
            .L       = { l_X_Axis,       l_Y_Axis },
            .R       = { r_X_Axis,       r_Y_Axis },
            .Trigger = { l_Trigger_Axis, r_Trigger_Axis }
        });
    }

    // ボタン押出状態マップ
    static std::map<GamepadButtonEvent::Button, bool> BUTTON_PRESSED =
    {
        { GamepadButtonEvent::Button::A, false },
        { GamepadButtonEvent::Button::B, false },
        { GamepadButtonEvent::Button::X, false },
        { GamepadButtonEvent::Button::Y, false },
        { GamepadButtonEvent::Button::START, false },
        { GamepadButtonEvent::Button::BACK, false },
        { GamepadButtonEvent::Button::LB, false },
        { GamepadButtonEvent::Button::RB, false },
        { GamepadButtonEvent::Button::LS, false },
        { GamepadButtonEvent::Button::RS, false },
    };

    int GAMEPAD_KEYS[14] =
    {
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_BACK,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_LEFT_THUMB,
        XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_DPAD_LEFT,
    };

    for (int i = 0; i < 14; i++)
    {
        // 押下状態
        auto button_state = state.Gamepad.wButtons & GAMEPAD_KEYS[i];

        // 保存された状態
        bool& pressed = BUTTON_PRESSED[static_cast<GamepadButtonEvent::Button>(i)];

        // 0以外で押している状態
        if (button_state != 0)
        {
            if (!pressed)
            {
                for (size_t j = 0; j < GameObject::all.size(); j++)
                {
                    if (!GameObject::all[j]->IsValid()) continue;

                    auto gameObject = GameObject::all[j].get();

                    // GUIの場合、visibleかどうか
                    if (gameObject->objectType == ObjectType::GUI)
                    if (!(static_cast<Gui*>(gameObject)->visible))
                    {
                        continue;
                    }

                    if (gameObject->isEnableInput)
                    gameObject->EventGamepadButtonPressed(GamepadButtonEvent
                    {
                        .button = static_cast<GamepadButtonEvent::Button>(GAMEPAD_KEYS[i])
                    });
                }

                // 参照を変更
                pressed = true;
            }
        }
        else
        {
            if (pressed)
            {
                for (size_t j = 0; j < GameObject::all.size(); j++)
                {
                    if (!GameObject::all[j]->IsValid()) continue;

                    auto gameObject = GameObject::all[j].get();

                    // GUIの場合、visibleかどうか
                    if (gameObject->objectType == ObjectType::GUI)
                    if (!(static_cast<Gui*>(gameObject)->visible))
                    {
                        continue;
                    }

                    if (gameObject->isEnableInput)
                    gameObject->EventGamepadButtonReleased(GamepadButtonEvent
                    {
                        .button = static_cast<GamepadButtonEvent::Button>(GAMEPAD_KEYS[i])
                    });
                }

                // 参照を変更
                pressed = false;
            }
        }
    }
}

void GameObject::EnableInput()
{
    this->isEnableInput = true;
}

void GameObject::DisableInput()
{
    this->isEnableInput = false;
}

bool GameObject::IsValid()
{
    // 解放されたメモリ
    if (this == (void*)0xdddddddddddddddd) return false;

    // 初期化されてないメモリ
    if (this == (void*)0xcdcdcdcdcdcdcdcd) return false;
    if (this == (void*)0xcccccccccccccccc) return false;

    if (this == (void*)0xFFFFFFFFFFFFFFFF) return false;

    // 破棄されたメモリ
    if (this == nullptr) return false;

    if (!this)           return false;

    if (this == (void*)0x0000000000000020) return false;

    return true;
}

void GameObject::Release()
{
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (!GameObject::all[i]->IsValid()) continue;

        auto& gameObject = GameObject::all[i];

        if (this == gameObject.get())
        {
            gameObject.release();

            delete this;

            GameObject::all.erase(std::remove(GameObject::all.begin(), GameObject::all.end(), gameObject), GameObject::all.end());
            GameObject::all.shrink_to_fit();

            break;
        }
    }
}

bool& GameObject::IsGamepadConnected()
{
    static bool connected = false;
    return connected;
}
