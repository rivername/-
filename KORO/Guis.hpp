#include "SpriteGui.hpp"
#include "FontGui.hpp"
#include "PanelGui.hpp"
#include "Window.hpp"
#include "Debug.hpp"
#include "Game.hpp"
#include "MathLib.hpp"
#include "EventHandler.hpp"
#pragma once

/*==================================
     ボタンテキストのデフォルトカラー
==================================*/
inline const D3DXCOLOR TEXT_BUTTON_DEFAULT_COLOR = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

/*==================================
     ステージ選択ボタンクラス生成マクロ
==================================*/
#define MAKE_STAGE_SELECT_BUTTON_TEXT(num) \
class Text_StageButton_ ## num : public FontGui \
{ \
public: \
    Text_StageButton_ ## num() : FontGui(std::format("ステージ{}", num + 1).c_str(), { ((int)(num / 5) == 0 ? 19 - 7 : 19 + 7), 35 + 6 * ((int)(num / 5) == 0 ? num : num - 5) }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER), __num(num) \
    { \
        this->EnableInput(); \
        this->isTextButton = true; \
        this->enableTextButtonArrow = true; \
    } \
public: \
    virtual void EventTick(float deltaTime) \
    { \
        Gui::EventTick(deltaTime); \
    \
        if (!Game::instance->saveData.allCleared) \
        { \
            this->isValidTextButton = (this->__num <= Game::instance->saveData.level); \
        } \
        else \
        { \
            this->isValidTextButton = (this->__num <= MAX_STAGE_NUM); \
        } \
    } \
public: \
    virtual void OnMouseOver() override \
    { \
        Gui::OnMouseOver(); \
        Game::instance->CustomEvent__Hovered_StageSelect(num); \
    } \
    virtual void OnMouseUp() override \
    { \
        Gui::OnMouseUp(); \
        Game::instance->CustomEvent__ButtonEnter_StageSelect(num); \
    } \
private: \
    byte __num; \
};

/*==================================
         タイトル画面の左側のやつ
==================================*/
class TitleSideArea : public SpriteGui
{
public:
    TitleSideArea() : SpriteGui("resources/sprites/title-side-area.png", { 50, 50 }, 1920, 1080) {}
};

/*==================================
         バージョン表示テキスト
==================================*/
class VersionText : public FontGui
{
public:
    VersionText() : FontGui("Ver. 1.0.0α", { 99, 97 }, 38, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), TextAlign::RIGHT) {}
};

/*==================================
          テキストボタンの矢印
==================================*/
class TextButtonArrow : public SpriteGui
{
public:
    inline static TextButtonArrow* instance;

public:
    TextButtonArrow() : SpriteGui("resources/sprites/text-button-arrow.png", { 19, 66 + 2 }, 16, 16)
    {
        TextButtonArrow::instance = this;
        this->fontSizeWithScale = { 42 * Window::instance->width / 1920 , this->scale };
        this->z_index = 0U;
    }

public:
    // テキストボタンのフォントサイズからスプライトのサイズを設定
    // @param font_size : フォントサイズ
    void SetScaleByFontSize(UINT font_size)
    {
        // スケール = 初期スケール * 倍率
        this->scale = this->fontSizeWithScale.second * (static_cast<float>(font_size) / this->fontSizeWithScale.first);
    }

private:
    // ペア   フォントサイズ/初期スケール
    std::pair<UINT, float> fontSizeWithScale;
};

/*==================================
          テキスト：新しいゲーム
==================================*/
class Text_NewGame : public FontGui
{
public:
    Text_NewGame() : FontGui("新しいゲーム", { 19, 64 }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
        this->enableTextButtonArrow = true;
    }

public:
    virtual void OnMouseOver() override
    {
        Gui::OnMouseOver();
        Game::instance->CustomEvent__Hovered_NewGame();
    }
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ButtonEnter_NewGame();
    }
};

/*==================================
           テキスト：続ける
==================================*/
class Text_Continue : public FontGui
{
public:
    Text_Continue() : FontGui("続ける", { 19, 64 + 6 }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
        this->enableTextButtonArrow = true;
    }

public:
    virtual void OnMouseOver() override
    {
        Gui::OnMouseOver();
        Game::instance->CustomEvent__Hovered_Continue();
    }
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ButtonEnter_Continue();
    }
};

/*==================================
          テキスト：ステージ選択
==================================*/
class Text_StageSelect : public FontGui
{
public:
    Text_StageSelect() : FontGui("ステージ選択", { 19, 64 + 6 * 2 }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
        this->enableTextButtonArrow = true;
    }

public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ButtonEnter_StageSelect();
    }
};

/*==================================
    テキスト：設定（タイトル画面）
==================================*/
class Text_Settings_InTitle : public FontGui
{
public:
    Text_Settings_InTitle() : FontGui("設定", { 19, 64 + 6 * 3 }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton          = true;
        this->enableTextButtonArrow = true;
    }

public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ShowSettingsMenu();
    }
};

/*==================================
          ステージ選択ボタン
==================================*/
MAKE_STAGE_SELECT_BUTTON_TEXT(0)
MAKE_STAGE_SELECT_BUTTON_TEXT(1)
MAKE_STAGE_SELECT_BUTTON_TEXT(2)
MAKE_STAGE_SELECT_BUTTON_TEXT(3)
MAKE_STAGE_SELECT_BUTTON_TEXT(4)
MAKE_STAGE_SELECT_BUTTON_TEXT(5)
MAKE_STAGE_SELECT_BUTTON_TEXT(6)
MAKE_STAGE_SELECT_BUTTON_TEXT(7)
MAKE_STAGE_SELECT_BUTTON_TEXT(8)
MAKE_STAGE_SELECT_BUTTON_TEXT(9)

/*==================================
       テキスト：レベル選択：戻る
==================================*/
class Text_StageSelectBack : public FontGui
{
public:
    Text_StageSelectBack() : FontGui("戻る", { 19, 64 + 6 * 2 }, 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
        this->enableTextButtonArrow = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ButtonEnter_StageSelectBack();
    }
};

/*==================================
           テキスト：終了する
==================================*/
class Text_Quit_01 : public FontGui
{
public:
    Text_Quit_01() : FontGui("終了する", { 19, 64 + 6 * 4 }, 42, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
        this->enableTextButtonArrow = true;
    }

public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ButtonEnter_Quit();
    }
};

inline const float INFO_BOARD_Y_OFFSET = 29.0f;

/*==================================
         テキスト：ステージ番号
==================================*/
class Text_Stage : public FontGui
{
public:
    inline static Text_Stage* instance;

public:
    Text_Stage() : FontGui("", { 50 - 16, 5 - INFO_BOARD_Y_OFFSET }, 65, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), TextAlign::CENTER)
    {
        Text_Stage::instance = this;
    }
};
/*==================================
            テキスト：スコア
==================================*/
class Text_Score : public FontGui
{
public:
    inline static Text_Score* instance;

public:
    Text_Score() : FontGui("", { 50 + 16, 5 - INFO_BOARD_Y_OFFSET }, 65, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), TextAlign::CENTER)
    {
        Text_Score::instance = this;
    }

public:
    virtual void EventTick(float deltaTime) override
    {
        Gui::EventTick(deltaTime);
        this->SetText(std::format("{}", Game::instance->score));
    }
};

class Canvas_InfoBoard;
/*==================================
               コイン
==================================*/
class GUI_Coin : public SpriteGui
{
public:
    enum class Mode
    {
        SOLID_SPRITE,  // 同じ位置にずっといる
        MOVABLE_SPRITE // 移動を想定する
    };

public:
    GUI_Coin(Mode mode) : SpriteGui("resources/sprites/coin.png", { 80, 8 }, 60, 60), mode(mode)
    {
        if (mode == Mode::MOVABLE_SPRITE)
        {
            this->SetPositionByPercent(80, -8);
        }
    }

public:
    virtual void EventTick(float deltaTime) override
    {
        if (this->released) return;

        Gui::EventTick(deltaTime);

        // コインUPアニメーション（コイン投下）
        if (this->mode == Mode::MOVABLE_SPRITE)
        if (this->GetPercentPos().y < 7)
        {
            // 7以下になるようにclamp
            float posY    = this->position.y + Gui::GetPosYFromPercent(2.0f);
            float newPosY = std::clamp(posY, this->position.y, Gui::GetPosYFromPercent(7.0f));

            // あらかじめclampした値を適用
            this->position.y = newPosY;
        }
        else
        {
            this->SetPositionByPercent(80.0f, 7.0f);
            EventHandler::Call("pop-coin");
            this->Release();
        }
    }

private:
    Mode mode;
};

/*==================================
    ガイドテキスト：ボールをゴールに導け
==================================*/
class GuideText01 : public SpriteGui
{
public:
    GuideText01() : SpriteGui("resources/sprites/guid.png", { 50, -20 }, 1382, 320)
    {
        this->scale *= 0.5f;
    }
};

/*==================================
     テキスト：ゲームに戻る（ポーズ画面）
==================================*/
class Text_Resume : public FontGui
{
public:
    Text_Resume() : FontGui("ゲームに戻る", { 50, 50 - 10 }, 65, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }

public:
    virtual void OnMouseUp()
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__ResumeGame();
    }
};
/*==================================
       テキスト：設定（ポーズ画面）
==================================*/
class Text_Settings_InPause : public FontGui
{
public:
    Text_Settings_InPause() : FontGui("設定", { 50, 50 }, 65, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }

public:
    virtual void OnMouseUp() override
    {
        Game::instance->CustomEvent__ShowSettingsMenu();
    }
};
/*==================================
       テキスト：戻る（設定画面）
==================================*/
class Text_BackToPauseMenu : public FontGui
{
public:
    Text_BackToPauseMenu(float posX) : FontGui("戻る", { posX + 3.0f, 81 }, 64, PanelColor::WHITE, TextAlign::LEFT)
    {
        this->EnableInput();
        this->isTextButton = true;
    }

public:
    virtual void OnMouseUp() override
    {
        if (Game::instance->gameStatus == GameStatus::IN_SETTINGS)
        {
            // ポーズ画面に戻る
            Game::instance->CustomEvent__BackToPauseMenuFromSettings();
        }
        else
        {
            // タイトル画面に戻る
            Game::instance->CustomEvent__BackToTitleMenuFromSettings();
        }
    }
};
/*==================================
      テキスト：終了する（ポーズ画面）
==================================*/
class Text_Quit_02 : public FontGui
{
public:
    Text_Quit_02() : FontGui("セーブしてタイトルに戻る", { 50, 50 + 10 }, 65, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__BeginBackToTitleScreen();
    }
};

/*==================================
             ラジオボタン
==================================*/
class Gui_RadioButton : public PanelGui
{
public:
    // 全ラジオボタン
    inline static std::vector<Gui_RadioButton*> all;

public:
    Gui_RadioButton(float posX, float posY, LONG size) : PanelGui({ posX, posY }, size, size, PanelColor::WHITE, BorderType::INNER_MASK, (size * 0.5f) - (size * 0.5f * 0.25f))
    {
        Gui_RadioButton::all.push_back(this);
        this->EnableInput();

        this->outerBorderRadius = size * 0.5f;
        this->radio = new PanelGui({ this->GetPercentPos().x, this->GetPercentPos().y }, size / 2, size / 2, PanelColor::WHITE, BorderType::ELLIPSE);
        this->radio->scale = 0.0f;
    }

public:
    // 選択状態を設定
    // @param noInterp : 補間なし
    void SetSelectState(bool select, bool noInterp = false)
    {
        this->radioSelected = select;
        this->DisableInput();

        // 補間なし
        if (noInterp)
        {
            this->radio->scale = this->radio->initialScale;
        }
    }
    // 選択状態を取得
    bool GetSelectState()
    {
        return this->radioSelected;
    }

public:
    virtual void EventTick(float deltaTime) override
    {
        Gui::EventTick(deltaTime);

        // ラジオ選択アニメーション
        InterpTo(&this->radio->scale, this->radio->scale, this->radioSelected ? this->radio->initialScale : 0.0f, 2.0f);
    }
    virtual void OnMouseUp() override
    {
        // 全部選択解除
        for (auto radioBtn : Gui_RadioButton::all)
        {
            radioBtn->radioSelected = false;
            radioBtn->EnableInput();
        }

        // 自分を選択
        this->radioSelected = true;
        this->DisableInput();
    }

public:
    // ラジオボタンの真ん中のやつ
    PanelGui* radio;

    // 選択中
    bool radioSelected = false;
};

/*==================================
      テキスト：クリア：タイトルに戻る
==================================*/
class Text_BackToTitleAtClear : public FontGui
{
public:
    Text_BackToTitleAtClear() : FontGui(" ", { 50 - 12, 66 }, 100, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER, true)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__BeginBackToTitleScreen();
    }
};
/*==================================
      テキスト：クリア：次のステージへ
==================================*/
class Text_NextGame : public FontGui
{
public:
    Text_NextGame() : FontGui("  ", { 50, 66 }, 102, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER, true)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__NextGame();
    }
};
/*==================================
      テキスト：クリア：リトライ
==================================*/
class Text_Retry_InClear : public FontGui
{
public:
    Text_Retry_InClear() : FontGui(" ", { 50 + 12, 66 }, 80, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER, true)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__Retry();
    }
};

/*==================================
      テキスト：タイムアップ：リトライ
==================================*/
class Text_Retry_InTimesUp : public FontGui
{
public:
    Text_Retry_InTimesUp() : FontGui("リトライ", { 50, 50 } , 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__Retry();
    }
};
/*==================================
   テキスト：タイムアップ：タイトルに戻る
==================================*/
class Text_BackToTitle : public FontGui
{
public:
    Text_BackToTitle() : FontGui("タイトルに戻る", { 50, 50 + 10 }, 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__BeginBackToTitleScreen();
    }
};

/*==================================
        おめでとう：タイトルに戻る
==================================*/
class Text_Congratulation : public FontGui
{
public:
    Text_Congratulation() : FontGui("タイトルに戻る", { 50, 77 }, 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->isTextButton = true;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();
        Game::instance->CustomEvent__BeginBackToTitleScreen();
    }
};

const inline float DAY_NIGHT_BAR_POS_X = 87.0f;
const inline float DAY_NIGHT_BAR_POS_Y = 87.0f;

#define RGB3(rgb) rgb, rgb, rgb

/*==================================
        昼夜サイクルバースプライト
==================================*/
class DayNightCycleBar : public SpriteGui
{
public:
    inline static DayNightCycleBar* instance;

public:
    DayNightCycleBar() : SpriteGui("resources/sprites/day-night-bar.png", { DAY_NIGHT_BAR_POS_X, DAY_NIGHT_BAR_POS_Y }, 386, 40)
    {
        DayNightCycleBar::instance = this;
    }
};
/*==================================
        昼夜サイクルバースライダー
==================================*/
class DayNightCycleBar_SliderSprite : public PanelGui
{
public:
    inline static DayNightCycleBar_SliderSprite* instance;

public:
    DayNightCycleBar_SliderSprite() : PanelGui({ DAY_NIGHT_BAR_POS_X - 7.0f, DAY_NIGHT_BAR_POS_Y }, 6, 20, PanelColor::WHITE)
    {
        DayNightCycleBar_SliderSprite::instance = this;
    }
};
/*==================================
        昼夜サイクルバースライダー
            つかむ用のダミー
==================================*/
class DayNightCycleBar_SliderButton : public FontGui
{
public:
    DayNightCycleBar_SliderButton() : FontGui(" ", { DAY_NIGHT_BAR_POS_X - 7.0f, DAY_NIGHT_BAR_POS_Y }, 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->enableTextButtonArrow = false;

        this->targetPosX = DayNightCycleBar_SliderSprite::instance->position.x;
    }
public:
    virtual void OnMouseDown() override
    {
        Gui::OnMouseDown();

        // SkyBoxCycleModeを設定
        DayNightCycleSkyBox::instance->SetCycleMode(SkyBoxCycleMode::USER_PREFERENCE);
    }
public:
    virtual void EventTick(float deltaTime) override
    {
        Gui::EventTick(deltaTime);

        POINT cursor = Window::instance->GetMouseCursorPos();

        // dragしているとき
        if (this->inputState.dragging)
        {
            // 一定量マウス位置が離れたら
            if (cursor.x >= this->position.x + GetPosXFromPercent(slideAmo))
            {
                // スライドを右に動かす
                if (this->SlideRight())
                {
                    // SkyBoxのデータ更新
                    DayNightCycleSkyBox::instance->AddSkyBoxControllerSlide(1);
                }
            }

            // 一定量マウス位置が離れたら
            if (cursor.x <= this->position.x - GetPosXFromPercent(slideAmo))
            {
                // スライドを左に動かす
                if (this->SlideLeft())
                {
                    // SkyBoxのデータ更新
                    DayNightCycleSkyBox::instance->AddSkyBoxControllerSlide(-1);
                }
            }

            // マウス位置X変数の範囲を固定
            cursor.x = std::clamp(cursor.x, (long)this->GetPosXFromPercent(DAY_NIGHT_BAR_POS_X - 7.0f), (long)this->GetPosXFromPercent(DAY_NIGHT_BAR_POS_X - 7.0f) + (long)this->GetPosXFromPercent(slideAmo * 6.0f));

            // drag中はマウス位置を追従
            DayNightCycleBar_SliderSprite::instance->position.x = static_cast<float>(cursor.x);

            // ゲームのセーブデータに適用
            Game::instance->saveData.skyboxData.slideValue = this->slideValue;
        }
        else
        {
            // dragしてないときはゆっくり目標地点にスライド
            InterpTo(&DayNightCycleBar_SliderSprite::instance->position.x, DayNightCycleBar_SliderSprite::instance->position.x, this->targetPosX, 100.0f);
        }
    }

public:
    bool SlideRight(bool teleport = false)
    {
        // スライド幅固定
        if (this->slideValue < 6)
        {
            // 自分（ダミー）を移動
            this->AddOffsetPercent(this->slideAmo, 0.0f);
            this->targetPosX = this->position.x;

            if (teleport)
            {
                DayNightCycleBar_SliderSprite::instance->position.x = this->targetPosX;
            }

            this->slideValue++;

            return true;
        }

        return false;
    }

    bool SlideLeft(bool teleport = false)
    {
        // スライド幅固定
        if (this->slideValue > 0)
        {
            // 自分（ダミー）を移動
            this->AddOffsetPercent(-this->slideAmo, 0.0f);
            this->targetPosX = this->position.x;

            if (teleport)
            {
                DayNightCycleBar_SliderSprite::instance->position.x = this->targetPosX;
            }

            this->slideValue--;

            return true;
        }

        return false;
    }

public:
    float       targetPosX = 0.0f;
    int         slideValue = 0;
    const float slideAmo   = 1.9f; // 1スライド量パーセント
};
/*==================================
    昼夜サイクル RealTime Syncボタン
==================================*/
class DayNightCycleBar_SyncLocalDateTimeButtonSprite : public SpriteGui
{
public:
    inline static DayNightCycleBar_SyncLocalDateTimeButtonSprite* instance;

public:
    DayNightCycleBar_SyncLocalDateTimeButtonSprite() : SpriteGui("resources/sprites/day-night-bar-sync-local-datetime.png", { DAY_NIGHT_BAR_POS_X + 8.6f, DAY_NIGHT_BAR_POS_Y }, 30, 30)
    {
        DayNightCycleBar_SyncLocalDateTimeButtonSprite::instance = this;

        this->color = { RGB3(0.5f), 1.0f };
    }
};
/*==================================
    昼夜サイクル RealTime Syncボタン
            押す用のダミー
==================================*/
class DayNightCycleBar_SyncLocalDateTimeButtonDummy : public FontGui
{
public:
    DayNightCycleBar_SyncLocalDateTimeButtonDummy() : FontGui("  ", { DAY_NIGHT_BAR_POS_X + 8.6f, DAY_NIGHT_BAR_POS_Y }, 48, TEXT_BUTTON_DEFAULT_COLOR, TextAlign::CENTER)
    {
        this->EnableInput();
        this->enableTextButtonArrow = false;
    }
public:
    virtual void OnMouseUp() override
    {
        Gui::OnMouseUp();

        // SkyBoxCycleModeを設定
        DayNightCycleSkyBox::instance->SetCycleMode(SkyBoxCycleMode::SYNC_LOCAL_DATETIME);
    }
};
