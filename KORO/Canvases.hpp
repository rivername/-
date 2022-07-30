#include "Canvas.hpp"
#include "GUIs.hpp"
#include "Game.hpp"
#include "MathLib.hpp"
#include "Queue.hpp"
#include "Timer.hpp"
#include "Delay.hpp"
#include "Stage.hpp"
#include "Camera.hpp"
#pragma once

/*==================================
            タイトル画面
==================================*/
class TitleScreen : public Canvas
{
public:
    TitleScreen()
    {
        this->EnableInput();

        this->AddGui(new TitleSideArea(), "title-side-area");

        this->AddGui(new Text_NewGame(),          "new-game");
        this->AddGui(new Text_Continue(),         "continue");
        this->AddGui(new Text_StageSelect(),      "select");
        this->AddGui(new Text_Settings_InTitle(), "settings");
        this->AddGui(new Text_Quit_01(),          "quit");

        this->AddGui(new VersionText(),      "version-text");

        this->AddGui(new SpriteGui("resources/sprites/logo1.png", { 19 + 1, 30 + 1 }, 1080, 1080), "koro-logo-shadow");
        this->AddGui(new SpriteGui("resources/sprites/logo1.png", { 19, 30 }, 1080, 1080),         "koro-logo");

        this->GetGuiChildByName<Gui>("koro-logo-shadow")->scale *= 0.4f;
        this->GetGuiChildByName<Gui>("koro-logo-shadow")->color  = { 0.0f, 0.0f, 0.0f, 0.2f };
        this->GetGuiChildByName<Gui>("koro-logo")->scale        *= 0.4f;
    }

public:
    void StartHideAnimation()
    {
        this->hide = true;
    }

    void StartShowAnimation()
    {
        this->hide = false;
        this->Show();
    }

public:
    virtual void Update() override
    {
        float amo = 60.0f;

        for (auto& pair : this->allChildGuiWithName)
        {
            // バージョン表示テキストは移動しない
            if (pair.second == "version-text") continue;

            auto gui = pair.first;

            if (hide)
            {
                if (offset >= -18)
                {
                    gui->position.x -= amo;
                    
                    if (this->allChildGuiWithName.back() == pair)
                    {
                        offset--;
                    }
                }
            }
            else
            {
                if (offset < 0)
                {
                    gui->position.x  += amo;

                    if (this->allChildGuiWithName.back() == pair)
                    {
                        offset++;
                    }
                }
            }
        }

        // 現在のセーブデータのステージが0の場合は続けるボタンは無効
        this->GetGuiChildByName<FontGui>("continue")->isValidTextButton = (Game::instance->saveData.level != 0);
    }

private:
    bool hide   = false;
    int  offset = 0;
};

/*==================================
           ステージ選択画面
==================================*/
class StageSelectScreen : public Canvas
{
public:
    StageSelectScreen()
    {
        this->EnableInput();

        this->AddGui(new TitleSideArea(), "title-side-area");

        this->AddGui(new FontGui("ステージを選択してください", { 19, 20 }, 52, PanelColor::WHITE, TextAlign::CENTER), "caption");

        this->AddGui(new Text_StageSelectBack(), "back");
        this->AddGui(new Text_StageButton_0(),   "1");
        this->AddGui(new Text_StageButton_1(),   "2");
        this->AddGui(new Text_StageButton_2(),   "3");
        this->AddGui(new Text_StageButton_3(),   "4");
        this->AddGui(new Text_StageButton_4(),   "5");
        this->AddGui(new Text_StageButton_5(),   "6");
        this->AddGui(new Text_StageButton_6(),   "7");
        this->AddGui(new Text_StageButton_7(),   "8");
        this->AddGui(new Text_StageButton_8(),   "9");
        this->AddGui(new Text_StageButton_9(),   "10");
    }

public:
    void StartHideAnimation()
    {
        this->hide = true;
    }

    void StartShowAnimation()
    {
        this->hide = false;
        this->Show();
    }

public:
    virtual void Update() override
    {
        float amo = 60.0f;

        if (hide)
        {
            if (offset >= -18)
            {
                this->AddOffset(-amo, 0.0f);
                offset--;
            }
        }
        else
        {
            if (offset < 0)
            {
                this->AddOffset(amo, 0.0f);
                offset++;
            }
        }
    }

private:
    bool hide   = true;
    int  offset = 0;
};

class Canvas_Star : public Canvas
{
public:
    Canvas_Star()
    {
        this->AddGui(new SpriteGui("resources/sprites/star.png", { 0, 0 }, 60, 60), "star-frame");
        this->AddGui(new SpriteGui("resources/sprites/star.png", { 0, 0 }, 60, 60), "star");

        this->GetGuiChildByName<Gui>("star-frame")->color  = D3DCOLOR_XRGB(255, 200, 0);
        this->GetGuiChildByName<Gui>("star")->color        = D3DCOLOR_XRGB(255, 240, 0);
        this->GetGuiChildByName<Gui>("star")->scale *= 0.75f;
    }

public:
    virtual void Update() override
    {
        D3DXVECTOR2 out     = V2_DEFAULT;
        D3DXVECTOR2 current = this->GetGuiChildByName<Gui>("star")->GetPercentPos();
        D3DXVECTOR2 to      = { 80.0f, 8.0f };

        D3DXVec2Lerp(&out, &current, &to, this->alpha);

        this->GetGuiChildByName<Gui>("star"      )->SetPositionByPercent(out.x, out.y);
        this->GetGuiChildByName<Gui>("star-frame")->SetPositionByPercent(out.x, out.y);

        this->alpha += 9.807f * this->alpha * 0.01f;

        this->alpha = std::clamp(this->alpha, 0.0f, 1.0f);

        if (this->alpha < 0.5f)
        {
            this->AddOffsetPercent(1, 0);
        }
    }

private:
    float alpha = 0.01f;
};

/*==================================
              タイマー
==================================*/
class Canvas_Timer : public Canvas
{
public:
    Canvas_Timer()
    {
        // スプライト
        this->AddGui(new PanelGui({ 50, 8 }, 300, 128, PanelColor::ALPHA50, BorderType::ELLIPSE), "background");
        this->AddGui(new SpriteGui("resources/sprites/timer.png", { 46, 8 }, 128, 128, { 0,  0, 80, 80 }), "frame");
        this->AddGui(new SpriteGui("resources/sprites/timer.png", { 46, 8 }, 128, 128, { 0, 80,  6, 80 + (34 * 2 - 6) }), "arrow");
        
        // 秒テキスト
        this->AddGui(new FontGui("60", { 51, 8 }, 84, { 1.0f, 1.0f, 1.0f, 1.0f }, TextAlign::CENTER), "text");
    }

public:
    virtual void Update() override
    {
        if (Timer::instance)
        {
            auto secondF = Timer::instance->GetCurrentSecondFloat();
            auto second  = Timer::instance->GetCurrentSecond();

            // タイマー秒から針の回転を設定
            this->GetGuiChildByName<Gui>("arrow")->rotationDeg = secondF * 6.0f;

            // テキストを秒に変更
            this->GetGuiChildByName<FontGui>("text")->SetText(std::format("{}", second));

            // 残り5秒で赤
            if (second <= 5)
            {
                this->GetGuiChildByName<Gui>("text")->color = { 1.0f, 0.0f, 0.0f, 1.0f };
            } else

            // 残り10秒でオレンジ
            if (second <= 10)
            {
                this->GetGuiChildByName<Gui>("text")->color = { 1.0f, 0.5f, 0.0f, 1.0f };
            } else
            
            {
                this->GetGuiChildByName<Gui>("text")->color = { 1.0f, 1.0f, 1.0f, 1.0f };
            }
        }
    }
};

/*==================================
            画面上の情報版
==================================*/
class Canvas_InfoBoard : public Canvas
{
public:
    Canvas_InfoBoard()
    {
        this->AddCanvas(new Canvas_Timer(), "timer");

        //this->AddGUI(new SpriteGui("resources/sprites/timer.png", { 83, 8 }, 380, 128, { 0, 0, 300, 128 }), "background-coin");
        this->AddGui(new FontGui("5", { 83 + 1, 8 }, 64, { 1.0f, 1.0f, 1.0f, 1.0f }, TextAlign::CENTER), "coins");
        this->AddGui(new GUI_Coin(GUI_Coin::Mode::SOLID_SPRITE), "coin");

        this->GetGuiChildByName<Gui>("coin")->visible = false;
        this->GetGuiChildByName<Gui>("coins")->visible = false;

        //this->GetGuiChildByName<Gui>("background-coin")->scale *= 0.75f;
        this->GetGuiChildByName<Gui>("coin")->z_index = 3U;

        // 画面外に隠す
        this->AddOffsetPercent(0, -8 * 2);
    }

public:
    void CoinPopAnimation()
    {
        auto coin = this->GetGuiChildByName<Gui>("coin");
        auto amo  = cosf(this->popAlpha * D3DX_PI / 2.0f); // cos(PI / 2) = 0

        // スケールアニメーション
        coin->AddScaleValueFromInitialScale(amo * 0.5f);

        // 位置アニメーション
        coin->AddOffsetPercentFromInitialPosition(0, amo * -2.0f);
        
        this->popAlpha += 0.1f;
        this->popAlpha = std::clamp(this->popAlpha, 0.0f, 1.0f);
    }

    // コインポップアニメーション
    void PopCoin()
    {
        this->popAlpha = 0.0f;
    }

public:
    virtual void Update() override
    {
        // 出現アニメーション
        if (this->GetCanvasChildByName("timer")->GetGuiChildByName<Gui>("text")->GetPercentPos().y < 8)
        {
            this->AddOffsetPercent(0, 1);
        }
        else
        {
            this->CoinPopAnimation();
        }
    }

private:
    float popAlpha = 1.0f;
};

/*==================================
    ステージを傾けてボールをゴールに導け
==================================*/
class GuideText : public Canvas
{
public:
    GuideText()
    {
        this->AddGui(new GuideText01(), "ボールをゴールに導け");
    }

public:
    virtual void Update() override
    {
        auto child01 = this->GetGuiChildByName<Gui>("ボールをゴールに導け");

        if (this->elapsedTick <= 30)
        {
            this->AddOffsetPercent(0, 1);
        }

        if (this->elapsedTick > 30 + 60 * 2)
        {
            this->AddOffsetPercent(0, -1);
        }

        elapsedTick++;
    }

private:
    ULONGLONG elapsedTick = 0ULL;
};

/*==================================
            レベル表示ボード
==================================*/
class LevelBoard : public Canvas
{
public:
    LevelBoard()
    {
        this->AddGui(new SpriteGui("resources/sprites/level-board.png", { 50, 50 }, 420, 500), "sprite");
        this->AddGui(new FontGui("1", { 48.5f, 52.0f }, 120, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), TextAlign::CENTER), "text");

        this->sprite_scale = this->GetGuiChildByName<Gui>("sprite")->scale;

        this->GetGuiChildByName<Gui>("sprite")->scale = 0.0f;
    }

public:
    virtual void Update() override
    {
        float scaleAmo   = 0.1f;
        bool textVisible = false;

        if (mode == 0)
        {
            scaleAmo    = 0.1f;
            textVisible = true;
        }

        if (mode == 1)
        {
            scaleAmo    = -0.1f;
            textVisible = false;
        }

        float scale = this->GetGuiChildByName<Gui>("sprite")->scale + scaleAmo;

        scale = std::clamp(scale, 0.0f, this->sprite_scale);

        this->GetGuiChildByName<Gui>("sprite")->scale = scale;
        this->GetGuiChildByName<Gui>("text")->visible = textVisible;
    }

public:
    WORD mode = 0;

private:
    float sprite_scale = 0.0f;
};

/*==================================
              操作ガイド
==================================*/
class Canvas_ControlGuid : public Canvas
{
public:
    Canvas_ControlGuid()
    {
        {   // マウス左クリックアイコン
            float x = 30;

            this->AddGui(
                new SpriteGui("resources/sprites/input_icon.png", { x, 91 + 5 }, 900, 300, { 0, 0, 300, 300 }, V2_DEFAULT, false),
                "mouse-left"
            );

            this->AddGui(new FontGui("ボードを傾ける", { x + 3.0f, 91 + 5 }, 32, { 1.0f, 1.0f, 1.0f, 1.0f }), "main-control");
        }

        {   // マウスホイールアイコン
            float x = 45.0f;

            this->AddGui(
                new SpriteGui("resources/sprites/input_icon.png", { x, 91 + 5 }, 900, 300, { 300, 0, 600, 300 }, V2_DEFAULT, false),
                "mouse-middle"
            );

            this->AddGui(new FontGui("ズーム In / Out", { x + 3.0f, 91 + 5 }, 32, { 1.0f, 1.0f, 1.0f, 1.0f }), "zoom-in-out");
        }

        {   // Tキーアイコン
            float x = 62.0f;

            this->AddGui(
                new SpriteGui("resources/sprites/input_icon.png", { x, 91 + 5 }, 900, 300, { 600, 0, 900, 300 }, V2_DEFAULT, true),
                "t-key"
            );

            this->AddGui(new FontGui("カメラ切り替え", { x + 3.5f, 91 + 5 }, 32, { 1.0f, 1.0f, 1.0f, 1.0f }), "zoom-in-out");
        }

        this->GetGuiChildByName<Gui>("mouse-left")->SetScaleValueFromInitialScale(0.2f);
        this->GetGuiChildByName<Gui>("mouse-middle")->SetScaleValueFromInitialScale(0.2f);
        this->GetGuiChildByName<Gui>("t-key")->SetScaleValueFromInitialScale(0.2f);
    }
};

/*==================================
             ポーズメニュー
==================================*/
class PauseMenu : public Canvas
{
public:
    PauseMenu()
    {
        this->AddGui(new PanelGui({ 50, 50 }, 1920, 1080, PanelColor::ALPHA50),   "back");
        this->AddGui(new Text_Resume(),                                           "resume");
        this->AddGui(new Text_Settings_InPause(),                                         "settings");
        this->AddGui(new Text_Quit_02(),                                          "quit");
    }
};

/*==================================
               設定画面
==================================*/
class Canvas_Settings : public Canvas
{
public:
    Canvas_Settings()
    {
        const float ASSIST_MENU_POS_Y = 50.0f - 10.0f - 10.0f;
        const float VOLUME_MENU_POS_Y = 50.0f + 10.0f - 5.0f;
        const float BACK_BUTTON_POS_X = 48.5f;

        // back
        this->AddGui(new PanelGui({ 50, 50 }, 1920, 1080, PanelColor::ALPHA50), "back");

        // アシストモードテキスト
        this->AddGui(new FontGui("アシストモード :", { 36, ASSIST_MENU_POS_Y }, 64, PanelColor::WHITE, TextAlign::RIGHT), "assist-text");

        // モード0
        this->AddGui(new Gui_RadioButton(40, ASSIST_MENU_POS_Y, 64), "assist-level-btn-0");
        this->AddGui(new FontGui("なし", { 46, ASSIST_MENU_POS_Y }, 64, PanelColor::WHITE, TextAlign::LEFT), "assist-text-none");

        // モード1
        this->AddGui(new Gui_RadioButton(60, ASSIST_MENU_POS_Y, 64), "assist-level-btn-1");
        this->AddGui(new FontGui("レベル1", { 66, ASSIST_MENU_POS_Y }, 64, PanelColor::WHITE, TextAlign::LEFT), "assist-text-level-1");

        // モード2
        this->AddGui(new Gui_RadioButton(80, ASSIST_MENU_POS_Y, 64), "assist-level-btn-2");
        this->AddGui(new FontGui("レベル2", { 86, ASSIST_MENU_POS_Y }, 64, PanelColor::WHITE, TextAlign::LEFT), "assist-text-level-2");

        // 音量テキスト
        this->AddGui(new FontGui("音量 :", { 36, VOLUME_MENU_POS_Y }, 64, PanelColor::WHITE, TextAlign::RIGHT), "assist-text");

        class VolumeDownButton : public FontGui
        {
        public:
            VolumeDownButton(float posY) : FontGui("〈", { 38, posY }, 96, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER, true)
            {
                this->EnableInput();
                this->isTextButton = true;
            }

        public:
            virtual void OnMouseUp() override
            {
                Gui::OnMouseUp();

                // 音量変数適用
                Game::instance->saveData.masterVolume = std::clamp(Game::instance->saveData.masterVolume - 10, 0, 100);

                // 音量の適用
                Game::instance->soundDevice->SetVolumeBuffer(Game::instance->saveData.masterVolume * 0.01f);
            }
        };

        class VolumeUpButton : public FontGui
        {
        public:
            VolumeUpButton(float posY) : FontGui("〉", { 85, posY }, 96, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER, true)
            {
                this->EnableInput();
                this->isTextButton = true;
            }

        public:
            virtual void OnMouseUp() override
            {
                Gui::OnMouseUp();

                // 音量変数適用
                Game::instance->saveData.masterVolume = std::clamp(Game::instance->saveData.masterVolume + 10, 0, 100);

                // 音量の適用
                Game::instance->soundDevice->SetVolumeBuffer(Game::instance->saveData.masterVolume * 0.01f);
            }
        };

        // 音量ボタン
        this->AddGui(new VolumeDownButton(VOLUME_MENU_POS_Y), "volume-down-btn");
        this->AddGui(new VolumeUpButton(VOLUME_MENU_POS_Y), "volume-up-btn");

        // 音量ゲージ
        for (auto i = 0; i < 10; i++)
        {
            this->AddGui(new PanelGui({ 44 + i * 4.0f, VOLUME_MENU_POS_Y }, 48, 24, PanelColor::WHITE, BorderType::DEFAULT, 8), std::format("volume-{}", i));
        }

        // 現在の音量テキスト
        this->AddGui(new FontGui(std::format("{}", Game::instance->saveData.masterVolume).c_str(), { 61, VOLUME_MENU_POS_Y - 5.0f }, 82, PanelColor::WHITE, TextAlign::CENTER), "current-volume");

        // 矢印
        this->AddGui(new PanelGui({ BACK_BUTTON_POS_X - 1,          81        }, 32, 6, PanelColor::WHITE, BorderType::ELLIPSE), "arrow-base");
        this->AddGui(new PanelGui({ BACK_BUTTON_POS_X - 1 - 0.5f,   81 - 0.7f }, 24, 6, PanelColor::WHITE, BorderType::ELLIPSE), "arrow-up-0");
        this->AddGui(new PanelGui({ BACK_BUTTON_POS_X - 1 - 0.5f,   81 + 0.7f }, 24, 6, PanelColor::WHITE, BorderType::ELLIPSE), "arrow-up-1");

        // 戻るボタン
        this->AddGui(new Text_BackToPauseMenu(BACK_BUTTON_POS_X), "text-Back");
        this->GetGuiChildByName<FontGui>("text-Back")->isTextButton = true;
        this->GetGuiChildByName<FontGui>("text-Back")->EnableInput();

        // 矢印回転
        this->GetGuiChildByName<Gui>("arrow-up-0")->rotationDeg = -45.0f;
        this->GetGuiChildByName<Gui>("arrow-up-1")->rotationDeg =  45.0f;

        // ラジオボタンリスト初期化
        this->radioButtons =
        {
            this->GetGuiChildByName<Gui_RadioButton>("assist-level-btn-0"),
            this->GetGuiChildByName<Gui_RadioButton>("assist-level-btn-1"),
            this->GetGuiChildByName<Gui_RadioButton>("assist-level-btn-2")
        };

        // セーブデータを読み込み
        this->radioButtons[Game::instance->saveData.assistModeLevel]->SetSelectState(true, true);

        // ラジオボタンのラジオを追加
        for (byte i = 0; i < this->radioButtons.size(); i++)
        {
            this->AddGui(this->radioButtons[i]->radio, std::format("assist-level-btn-radio-{}", i));
        }
    }

public:
    virtual void EventTick(float deltaTime) override
    {
        Canvas::EventTick(deltaTime);

        // 選択状態をセーブデータに反映

        for (byte i = 0; i < this->radioButtons.size(); i++)
        {
            if (this->radioButtons[i]->GetSelectState())
            {
                Game::instance->saveData.assistModeLevel = i;
            }
        }

        // 矢印の色を同期

        auto back_text_color = this->GetGuiChildByName<Gui>("text-Back")->color;
        this->GetGuiChildByName<Gui>("arrow-base")->color = back_text_color;
        this->GetGuiChildByName<Gui>("arrow-up-0")->color = back_text_color;
        this->GetGuiChildByName<Gui>("arrow-up-1")->color = back_text_color;

        // 現在の音量テキスト更新
        this->GetGuiChildByName<FontGui>("current-volume")->SetText(std::format("{}", Game::instance->saveData.masterVolume));

        // 音量ゲージの色設定
        for (auto i = 0; i < 10; i++)
        {
            this->GetGuiChildByName<Gui>(std::format("volume-{}", i))->color =
                ((i + 1) * 10 <= Game::instance->saveData.masterVolume) ?
                PanelColor::WHITE :
                0x80FFFFFF;
        }

        // 現在の音量により音量ボタンの無効化
        this->GetGuiChildByName<FontGui>("volume-down-btn")->isValidTextButton = (Game::instance->saveData.masterVolume > 0);
        this->GetGuiChildByName<FontGui>("volume-up-btn")->isValidTextButton   = (Game::instance->saveData.masterVolume < 100);
    }

private:
    // ラジオボタンGuiリスト
    std::vector<Gui_RadioButton*> radioButtons;
};

/*==================================
            ステージクリア
==================================*/
class StageClearedCanvas : public Canvas
{
public:
    StageClearedCanvas()
    {
        this->AddGui(new SpriteGui("resources/sprites/clear.png", { 50, 50 }, 824, 568), "sprite");
        this->AddGui(new Text_BackToTitleAtClear(),                                      "title");
        this->AddGui(new Text_NextGame(),                                                "next");
        this->AddGui(new Text_Retry_InClear(),                                           "retry");

        this->AddGui(new FontGui("クリアタイム:", { 50, 51 }, 52, PanelColor::WHITE, TextAlign::CENTER), "text-clear-time");
    }

public:
    void SetClearTimeText()
    {
        this->GetGuiChildByName<FontGui>("text-clear-time")->SetText(std::format("クリアタイム:{}秒", 60 - Timer::instance->GetCurrentSecond()));
    }
};

/*==================================
             タイムアップ
==================================*/
class TimesUpCanvas : public Canvas
{
public:
    TimesUpCanvas()
    {
        this->AddGui(new SpriteGui("resources/sprites/times-up.png", { 50, 50 }, 1920, 1080), "sprite");

        this->AddGui(new Text_Retry_InTimesUp(), "retry");
        this->AddGui(new Text_BackToTitle(),     "back-to-title");
    }
};

/*==================================
         カットインアニメーション
==================================*/
class CutInAnimationCanvas : public Canvas
{
public:
    CutInAnimationCanvas()
    {
        // 負担軽減しながらインスタンス化

        // カットインバー画像5枚
        QUEUE << [this]()
        {
            this->AddGui(new SpriteGui("resources/sprites/cut-in-bar.png", { 50 + 130, 90 }, 2622, 216), "sprite0"); // 一番下
            this->AddGui(new SpriteGui("resources/sprites/cut-in-bar.png", { 50 + 130, 70 }, 2622, 216), "sprite1");
            this->AddGui(new SpriteGui("resources/sprites/cut-in-bar.png", { 50 + 130, 50 }, 2622, 216), "sprite2");
            this->AddGui(new SpriteGui("resources/sprites/cut-in-bar.png", { 50 + 130, 30 }, 2622, 216), "sprite3");
            this->AddGui(new SpriteGui("resources/sprites/cut-in-bar.png", { 50 + 130, 10 }, 2622, 216), "sprite4"); // 一番上
        };

        // カットイン丸画像5枚
        QUEUE << [this]()
        {
            this->AddGui(new PanelGui({ 50 + 130, 90 }, 216, 216, PanelColor::WHITE, BorderType::ELLIPSE, {}, { -2622 * 0.5f, 0 }), "circle0"); // 一番下
            this->AddGui(new PanelGui({ 50 + 130, 70 }, 216, 216, PanelColor::WHITE, BorderType::ELLIPSE, {}, { -2622 * 0.5f, 0 }), "circle1");
            this->AddGui(new PanelGui({ 50 + 130, 50 }, 216, 216, PanelColor::WHITE, BorderType::ELLIPSE, {}, { -2622 * 0.5f, 0 }), "circle2");
            this->AddGui(new PanelGui({ 50 + 130, 30 }, 216, 216, PanelColor::WHITE, BorderType::ELLIPSE, {}, { -2622 * 0.5f, 0 }), "circle3");
            this->AddGui(new PanelGui({ 50 + 130, 10 }, 216, 216, PanelColor::WHITE, BorderType::ELLIPSE, {}, { -2622 * 0.5f, 0 }), "circle4"); // 一番上
        };

        // 3分割KOROロゴ
        QUEUE << [this]()
        {
            this->AddGui(new SpriteGui("resources/sprites/logo2.png", { 50 + 130, 30 }, 572, 648, { 0,   0,     572, 216     }), "logo3");
            this->AddGui(new SpriteGui("resources/sprites/logo2.png", { 50 + 130, 50 }, 572, 648, { 0, 216,     572, 216 * 2 }), "logo2");
            this->AddGui(new SpriteGui("resources/sprites/logo2.png", { 50 + 130, 70 }, 572, 648, { 0, 216 * 2, 572, 216 * 3 }), "logo1");
        };

        // KOROロゴ
        QUEUE << [this]()
        {
            this->AddGui(new SpriteGui("resources/sprites/logo1.png", { 50, 50 }, 1080, 1080), "koro-logo");

            // Opacityを0に
            this->GetGuiChildByName<Gui>("koro-logo")->color.a = 0.0f;
        };

        // パラメーター設定
        QUEUE << [this]()
        {
            // レインボーに色設定
            // 色: https://gyazo.com/f2186ab9f38efb507ba06a655b8e0e8f
            this->GetGuiChildByName<Gui>("sprite0")->color = 0xFF5260A1;
            this->GetGuiChildByName<Gui>("sprite1")->color = 0xFF00A0A4;
            this->GetGuiChildByName<Gui>("sprite2")->color = 0xFFFFCB00;
            this->GetGuiChildByName<Gui>("sprite3")->color = 0xFFF99720;
            this->GetGuiChildByName<Gui>("sprite4")->color = 0xFFEF3219;

            // 3分割KOROロゴの不透明度の設定
            this->GetGuiChildByName<Gui>("logo3")->color.a = 0.8f;
            this->GetGuiChildByName<Gui>("logo2")->color.a = 0.8f;
            this->GetGuiChildByName<Gui>("logo1")->color.a = 0.8f;

            // KOROロゴのスケールの計算 -> 設定
            this->GetGuiChildByName<Gui>("koro-logo")->scale *= (572.0f / 1080.0f);

            for (auto& pair : this->allChildGuiWithName)
            {
                pair.first->z_index = 1U;
            }

            // インスタンス化完了
            this->instantiated = true;
        };

        // イベント取得元栓オフ
        Game::LISTEN_EVENT_ROOT = false;

        // カーソールモードをデフォルトに
        Game::instance->cursorMode = CursorMode::MOUSE_ARROW;
    }

public:
    virtual void Update() override
    {
        if (!this->instantiated) return;

        bool  translated = true;
        float xAmo       = -4.0f;

#pragma region フェードイン

        for (int i = 0; i < 5; i++)
        {
            Gui* guis[] =
            {
                this->GetGuiChildByName<Gui>(std::format("sprite{}", i)), // カットインバー画像5枚
                this->GetGuiChildByName<Gui>(std::format("circle{}", i)), // カットイン丸画像5枚
                this->GetGuiChildByName<Gui>(std::format("logo{}", i))    // 3分割KOROロゴ
            };

            // 順番に動き出す
            if (elapsedTick0 >= i * (10ULL - i))
            {
                for (auto gui : guis)
                {
                    if (gui != nullptr)
                    if (gui->position.x > gui->GetPosXFromPercent(50))
                    {
                        // 現在の位置パーセント
                        auto currentPercent = gui->GetPercentPos();

                        // 動き出しの遅いスプライトは高加速
                        float newPercentX = currentPercent.x + (xAmo - i);

                        // 50%~ でクランプ適用
                        gui->position.x = gui->GetPosXFromPercent(std::clamp(newPercentX, 50.0f, 200.0f));
                        translated = false;

                        // KOROロゴのスケールを保存
                        this->logo_scale = this->GetGuiChildByName<Gui>("koro-logo")->scale;
                    }
                }
            }
        }

#pragma endregion

        // フェードイン完了
        if (translated)
        {
            // KOROロゴのOpacityを0.8に
            this->GetGuiChildByName<Gui>("koro-logo")->color.a = 0.8f;

            // 3分割KOROロゴのOpacityを0に
            this->GetGuiChildByName<Gui>("logo3")->color.a = 0.0f;
            this->GetGuiChildByName<Gui>("logo2")->color.a = 0.0f;
            this->GetGuiChildByName<Gui>("logo1")->color.a = 0.0f;

#pragma region ぷるぷる

            if (this->gate)
            if (this->elapsedTick1 <= 30)
            {
                this->Shake();
            } else

            if (this->elapsedTick1 <= 90)
            {
                this->UnShake();
            } else
            
            if (this->elapsedTick1 <= 120)
            {
                this->Shake();
            } else
            
            if (this->elapsedTick1 <= 180)
            {
                this->UnShake();
            }

#pragma endregion
            
            if (this->elapsedTick1 > 130)
            {
                this->gate = false;

                // フェードアウト
                for (auto& pair : this->allChildGuiWithName)
                {
                    pair.first->AddOffsetPercent(xAmo, 0.0f);
                }
            }

            if (this->elapsedTick1 > 200)
            {
                // イベント取得元栓オン
                Game::LISTEN_EVENT_ROOT = true;
            }

            this->elapsedTick1++;
        }

        this->elapsedTick0++;
    }

private:
    // ぷるぷるさせる・ちょっと大きくする
    void Shake()
    {
        // ぷるぷるさせる
        this->GetGuiChildByName<Gui>("koro-logo")->rotationDeg = cosf(this->elapsedTick1 * 0.5f) * 2.0f;

        // ちょっと大きくする
        auto logo = this->GetGuiChildByName<Gui>("koro-logo");
        InterpTo(&logo->scale, logo->scale, this->logo_scale + 0.1f, 0.1f);
    }

    // 元に戻す
    void UnShake()
    {
        this->GetGuiChildByName<Gui>("koro-logo")->rotationDeg = 0.0f;

        // 元に戻す
        auto logo = this->GetGuiChildByName<Gui>("koro-logo");
        InterpTo(&logo->scale, logo->scale, this->logo_scale, 0.1f);
    }

private:
    ULONGLONG elapsedTick0 = 0ULL;
    ULONGLONG elapsedTick1 = 0ULL;
    bool instantiated      = false;
    bool gate              = true;
    float logo_scale       = 0.0f;
};

/*==================================
   DayNightCycleSkyBoxコントローラー
==================================*/
class DayNightCycleSkyBoxController : public Canvas
{
public:
    DayNightCycleSkyBoxController()
    {
        this->AddGui(new DayNightCycleBar(), "back-sprite");
        this->AddGui
        (
            new FontGui("|", { DAY_NIGHT_BAR_POS_X + 6.9f, DAY_NIGHT_BAR_POS_Y }, 24, { 1.0f, 1.0f, 1.0f, 0.5f }, TextAlign::CENTER),
            "split"
        );

        this->AddGui(new DayNightCycleBar_SliderSprite(), "slider-sprite");
        this->AddGui(new DayNightCycleBar_SliderButton(), "slider-button");

        this->AddGui(new DayNightCycleBar_SyncLocalDateTimeButtonSprite(), "sync-button-sprite");
        this->AddGui(new DayNightCycleBar_SyncLocalDateTimeButtonDummy(),  "sync-button-dummy");
    }

public:
    // コントロールモードを設定
    void SetControlMode(SkyBoxCycleMode cycleMode)
    {
        const float disableColor = 0.5f;

        // 色を設定
        this->GetGuiChildByName<Gui>("slider-sprite")->color = cycleMode == SkyBoxCycleMode::USER_PREFERENCE ? D3DXCOLOR(RGB3(1.0f), 1.0f) : D3DXCOLOR(RGB3(disableColor), 1.0f);
        this->GetGuiChildByName<Gui>("back-sprite")->color   = cycleMode == SkyBoxCycleMode::USER_PREFERENCE ? D3DXCOLOR(RGB3(1.0f), 1.0f) : D3DXCOLOR(RGB3(disableColor), 1.0f);

        // 色を設定
        this->GetGuiChildByName<Gui>("sync-button-sprite")->color = cycleMode == SkyBoxCycleMode::SYNC_LOCAL_DATETIME ? D3DXCOLOR(RGB3(1.0f), 1.0f) : D3DXCOLOR(RGB3(disableColor), 1.0f);
        
        // Syncボタンは入力有効切り替え
        this->GetGuiChildByName<Gui>("sync-button-dummy")->isEnableInput = cycleMode == SkyBoxCycleMode::SYNC_LOCAL_DATETIME ? false : true;
    }

    void SetControlSlide(int slideValue, bool teleport = false)
    {
        auto slide = static_cast<DayNightCycleBar_SliderButton*>(this->GetGuiChildByName<Gui>("slider-button"));
        int slideAmo = slideValue - slide->slideValue;

        if (slideAmo > 0)
        {
            for (int i = 0; i < slideAmo; i++) slide->SlideRight(teleport);
        }
        else
        {
            for (int i = 0; i < abs(slideAmo); i++) slide->SlideLeft(teleport);
        }
    }
};

/*==================================
          フェードアニメーション
==================================*/
class Canvas_FadeAnimation : public Canvas
{
    inline static const float MAX_BORDER_RADIUS = (1920 * 1.2f);

public:
    // フェードモード
    enum class Mode : char
    {
        FADE_IN_WITH_GAME_LAUNCH = -1, // フェードイン（ゲーム起動時）
        FADE_IN,                       // フェードイン
        FADE_OUT,                      // フェードアウト
        FADE_OUT_WITH_QUIT_GAME        // フェードアウト（フェード終了後にゲーム終了）
    };

    // フェードスピード
    enum class Speed : char
    {
        LAUNCH_AND_QUIT = 1, // 起動時と終了時
        BACK_TO_TITLE   = 3  // タイトルに戻るとき
    };

public:
    Canvas_FadeAnimation()
    {
        this->AddGui(new PanelGui({ 50, 50 }, 1920, 1080, PanelColor::BLACK, BorderType::INNER_MASK), "panel");

        // 最前面に設定
        this->SetZIndex(0U);
    }

public:
    void SetFadeMode(Mode mode)
    {
        this->mode = mode;
    }
    void ToggleFadeMode()
    {
        this->mode = (Mode)!(bool)this->mode;
    }
    void SetFadeSpeed(Speed speedEnum)
    {
        this->speed = speedEnum;
    }

public:
    virtual void Update() override
    {
        // ちょっと待つ
        if (this->elapsedTick < 10U)
        {
            this->elapsedTick++;
            return;
        }

        // Gui
        auto panel = this->GetGuiChildByName<PanelGui>("panel");

        // ボーダー増加量
        float amo = 10.0f + panel->innerBorderRadius * 0.03f * static_cast<float>(this->speed);

        // 値を変えながらClamp
        panel->innerBorderRadius = std::clamp
        (
            panel->innerBorderRadius + ((mode == Mode::FADE_IN_WITH_GAME_LAUNCH || mode == Mode::FADE_IN) ? amo : -amo),
            0.0f,
            MAX_BORDER_RADIUS
        );

        // ゲーム起動モード
        if (this->mode == Mode::FADE_IN_WITH_GAME_LAUNCH)
        {
            if (panel->innerBorderRadius >= MAX_BORDER_RADIUS)
            {
                // ゲームが起動した
                Game::instance->CustomEvent__LaunchedGame();

                // 普通のモードに切り替えてToggle出来るように
                this->mode = Mode::FADE_IN;
            }
        }

        // ゲーム終了モード
        if (this->mode == Mode::FADE_OUT_WITH_QUIT_GAME)
        if (panel->innerBorderRadius <= 0.0f)
        {
            // 終了
            DELAY([]()
            {
                PostQuitMessage(0);
            }, 300);

            // フラグだけ切り替え
            this->released = true;
        }

        // タイトルに戻る
        if (this->mode  == Mode::FADE_OUT)
        if (this->speed == Speed::BACK_TO_TITLE)
        {
            // 0になったらイベント呼び出し
            if (panel->innerBorderRadius <= 0.0f)
            {
                Game::instance->CustomEvent__BackToTitleScreen();
            }
        }
    }

private:
    Mode  mode        = Mode::FADE_IN_WITH_GAME_LAUNCH;
    Speed speed       = Speed::LAUNCH_AND_QUIT;
    byte  elapsedTick = 0U;
};

/*==================================
              水平メーター
==================================*/
class HorizonMeter : public Canvas
{
public:
    HorizonMeter()
    {
        this->AddGui(new PanelGui({ 10, 50 }, 320, 320, 0x20FFFFFF, BorderType::DEFAULT), "back");

        const LONG borderSize = 256L;
        const LONG dotSize    = borderSize / 16;

        this->AddGui(new PanelGui({ 10, 50 }, dotSize / 2, dotSize / 2, 0x80FFFFFF, BorderType::ELLIPSE), "e0");
        this->AddGui(new PanelGui({ 10, 50 }, dotSize, dotSize, 0x80FFFFFF, BorderType::ELLIPSE), "e1");
        this->AddGui(new PanelGui({ 10, 50 }, borderSize, borderSize, 0x80FFFFFF, BorderType::INNER_MASK, borderSize * 0.5f - dotSize * 0.5f), "e2");

        this->GetGuiChildByName<PanelGui>("e2")->outerBorderRadius = borderSize * 0.5f;

        for (auto& pair : this->allChildGuiWithName)
        {
            pair.first->position     = Gui::GetPosFromPercent(10, 0);
            pair.first->initialPos   = Gui::GetPosFromPercent(10, 0);
            pair.first->position.y   = Gui::GetPosYFromPercent(100) - Gui::GetPosXFromPercent(12);
            pair.first->initialPos.y = Gui::GetPosYFromPercent(100) - Gui::GetPosXFromPercent(12);
        }

        this->AddGui(new PanelGui({ 10, 88 }, 320, 40, 0x80FFFFFF), "assist-mode-text-back");
        this->AddGui(new FontGui("アシストモード: {}", { 4, 96 }, 28, PanelColor::WHITE, TextAlign::CENTER), "assist-mode-text");

        this->GetGuiChildByName<Gui>("assist-mode-text-back")->pivotOffset = { 0.0f, 160.0f + 40 * 0.5f };
        this->GetGuiChildByName<Gui>("assist-mode-text")->pivotOffset      = { 0.0f, 160.0f + 40 * 0.5f };

        float scaling = Gui::GetScalingValue();
        this->GetGuiChildByName<Gui>("assist-mode-text-back")->pivotOffset *= scaling;
        this->GetGuiChildByName<Gui>("assist-mode-text")->pivotOffset *= scaling;

        this->Hide();
    }

public:
    virtual void Update() override
    {
        this->SetAxisPosition();
        this->UpdatePosition();
    }

public:
    virtual void EventTick(float deltaTime) override
    {
        Canvas::EventTick(deltaTime);

        // アシストモード：レベル1未満は無効化
        if (Game::instance->saveData.assistModeLevel < 1)
        {
            this->Hide();
            return;
        }

        // Stage
        if (Game::instance->gameStatus    == GameStatus::IN_MOVIE ||
            Camera::instance->camera_mode == CameraMode::Tracking_Stage ||
            Camera::instance->camera_mode == CameraMode::Tracking_Koro)
        {
            this->Show();
            this->GetGuiChildByName<Gui>("back")->visible                  = true;
            this->GetGuiChildByName<Gui>("assist-mode-text-back")->visible = true;
        } else
        
        // それ以外
        {
            this->Hide();
        }

        if (Camera::instance->camera_mode == CameraMode::Tracking_Koro)
        {
            this->GetGuiChildByName<Gui>("back")->visible                  = false;
            this->GetGuiChildByName<Gui>("assist-mode-text-back")->visible = false;
        }

        // アシストモードテキスト更新
        this->GetGuiChildByName<FontGui>("assist-mode-text")->SetText(std::format("アシストモード: {}", Game::instance->saveData.assistModeLevel));
    }
    
private:
    // 水平メーターの軸を動かす
    void SetAxisPosition()
    {
        auto gui = this->GetGuiChildByName<Gui>("e1");

        float scaling = Gui::GetScalingValue();

        gui->position.x = gui->initialPos.x + D3DXToDegree(Stage::instance->l_x_axis) * 1.5f * scaling;
        gui->position.y = gui->initialPos.y - D3DXToDegree(Stage::instance->l_y_axis) * 1.5f * scaling;
    }

    // 位置更新
    void UpdatePosition()
    {
        if (this->cameraMode == Camera::instance->camera_mode &&
            Game::instance->gameStatus == GameStatus::PLAYING) return;

        this->cameraMode = Camera::instance->camera_mode;

        if (Game::instance->gameStatus    == GameStatus::IN_MOVIE ||
            Camera::instance->camera_mode == CameraMode::Tracking_Stage)
        {
            for (auto& pair : this->allChildGuiWithName)
            {
                // Xを設定
                pair.first->position   = Gui::GetPosFromPercent(10, 0);
                pair.first->initialPos = Gui::GetPosFromPercent(10, 0);

                // Yを設定
                pair.first->position.y   = Gui::GetPosYFromPercent(100) - Gui::GetPosXFromPercent(12);
                pair.first->initialPos.y = Gui::GetPosYFromPercent(100) - Gui::GetPosXFromPercent(12);
            }
        } else

        if (Camera::instance->camera_mode == CameraMode::Tracking_Koro)
        {
            for (auto& pair : this->allChildGuiWithName)
            {
                // アシストテキストはちょっと移動（Xだけ移動）
                if (dynamic_cast<FontGui*>(pair.first))
                {
                    pair.first->position   = Gui::GetPosFromPercent(6.0f, 80.0f);
                    pair.first->initialPos = Gui::GetPosFromPercent(6.0f, 80.0f);
                    continue;
                }

                // それ以外

                // 画面中央に移動
                pair.first->position   = Gui::GetPosFromPercent(50, 50);
                pair.first->initialPos = Gui::GetPosFromPercent(50, 50);
            }
        }
    }

private:
    CameraMode cameraMode = CameraMode::Title_camera;
};

/*==================================
            全クリおめでとう
==================================*/
class Congratulation : public Canvas
{
public:
    Congratulation()
    {
        this->AddGui(new SpriteGui("resources/sprites/congratulation.png", { 50, 50 }, 942, 825), "sprite");
        this->AddGui(new Text_Congratulation(), "text");
    }
};
