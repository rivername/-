#include "GameObject.hpp"
#include "DayNightCycleSkyBox.hpp"
#include "Sound.hpp"
#include "PatternDataLoader.hpp"
#include "KoroActorPatternDataParam.hpp"

#include <string>
#include <functional>
#pragma once

using std::string;

/*==================================
            ゲームステータス
==================================*/
enum class GameStatus : byte
{
    IN_TITLE,    // タイトル画面
    IN_MOVIE,    // ムービー中
    PLAYING,     // プレイ中
    PAUSING,     // ポーズ中
    IN_SETTINGS, // 設定画面
    CLEARED,     // ステージをクリアした
    ALL_CLEARED, // 全クリした
    TIMES_UP,    // タイムアップした
};

/*==================================
             セーブデータ
==================================*/
struct SaveData
{
    byte       level;           // レベル（ステージ番号）
    bool       allCleared;      // 全クリしているか
    SkyBoxData skyboxData;      // SkyBoxデータByte
    byte       assistModeLevel; // アシストモードのレベル
    byte       masterVolume;    // 全体音量
};

/*==================================
              入力モード
==================================*/
enum class InputMode : byte
{
    RAW,    // キーボード・マウス
    GAMEPAD // ゲームパッド
};

/*==================================
         アプリケーションのモード
==================================*/
enum class AppMode : byte
{
    GAME,        // ゲームのみ
    LEVEL_EDITOR // レベルエディターのみ
};

/*==================================
           カーソールモード
==================================*/
enum class CursorMode : byte
{
    MOUSE_ARROW, // デフォルト
    MOUSE_HAND,  // ポインター

    HAND_OPEN,          // 手
    HAND_HOLD,          // 手（握り）
    HAND_HOLD_IN_MOVIE, // 手（握り）（ムービー中）

    WAITING  // 処理中
};

/*==================================
         セーブデータファイル名
==================================*/
inline const string SAVEDATA_FOLDERNAME = "data/";
inline const string SAVEDATA_FILENAME   = "save.KORO";
inline const string SAVEDATA_PATH       = SAVEDATA_FOLDERNAME + SAVEDATA_FILENAME;

/*==================================
            最大ステージ番号
               (0 ~ )
==================================*/
inline const UINT MAX_STAGE_NUM = 7;

// ポーズ中でないとき
#define IF_NOT_PAUSING if (Game::instance->gameStatus != GameStatus::PAUSING && Game::instance->gameStatus != GameStatus::IN_SETTINGS)

// ポーズ中はこれ以降の処理は実行しない
#define IF_PAUSE_RETURN if (Game::instance->gameStatus == GameStatus::PAUSING || Game::instance->gameStatus == GameStatus::IN_SETTINGS) return

class Game : public GameObject
{
public:
    // インスタンス
    inline static Game* instance;

    // イベント取得系元栓
    inline static bool LISTEN_EVENT_ROOT = false;

public:
    Game(AppMode appMode);
    ~Game();

public:
    /*==================================
                カスタムイベント
    ==================================*/

    // ゲームが起動した
    void CustomEvent__LaunchedGame();

    /// ---:

    // ボタンが押された：新しいゲーム
    void CustomEvent__ButtonEnter_NewGame();

    // ボタンが押された：続ける
    void CustomEvent__ButtonEnter_Continue();

    // ボタンが押された：レベル選択
    void CustomEvent__ButtonEnter_StageSelect();

        // ステージを選択
        // @param num : ステージ番号 (0 ~)
        void CustomEvent__ButtonEnter_StageSelect(UINT num);

        // ステージを選択：戻る
        void CustomEvent__ButtonEnter_StageSelectBack();

    // ボタンが押された：終了
    void CustomEvent__ButtonEnter_Quit();

    /// ---:

    // ボタンがホバーされた：新しいゲーム
    // @param enforce : 強制的に
    void CustomEvent__Hovered_NewGame(bool enforce = false);

    // ボタンがホバーされた：新しいゲーム
    // @param enforce : 強制的に
    void CustomEvent__Hovered_Continue(bool enforce = false);

    // ステージボタンホバー
    // @param num : ステージ番号 (0 ~)
    void CustomEvent__Hovered_StageSelect(UINT num);

    /// ---:

    // ゲームスタート
    void CustomEvent__StartGame();

    // カメラのタイトル動作終了イベント
    void CustomEvent__TitleCameraMoveEnded();

    // タイマーなどのボードとレベル表示ボードを表示
    void CustomEvent__ShowInfoBoardAndLevelBoard();

    // タイマーなどのボードとレベル表示ボード
    void CustomEvent__SetInfoBoardAndLevelBoard();

    // ステージをクリアした
    void CustomEvent__StageCleared();

    // タイムアップ
    void CustomEvent__TimesUp();

    // リトライ
    void CustomEvent__Retry();

    // 次のステージへ
    void CustomEvent__NextGame();

    // タイトル画面に戻る : Begin
    void CustomEvent__BeginBackToTitleScreen();

    // タイトル画面に戻る : Process
    void CustomEvent__BackToTitleScreen();

    /// ---;

    // コインを増やす
    void CustomEvent__AddCoin(UINT amo);

    /// ---:

    // ゲームをポーズ
    void CustomEvent__PauseGame();

    // ゲームを再開
    void CustomEvent__ResumeGame();

    // 設定画面を表示
    void CustomEvent__ShowSettingsMenu();

    // 設定画面からポーズ画面に戻る
    void CustomEvent__BackToPauseMenuFromSettings();

    // 設定画面からタイトル画面に戻る
    void CustomEvent__BackToTitleMenuFromSettings();

    // 全クリ時の紙吹雪演出
    void CustomEvent__CongratulationParticle(bool restart);

    // ===== ゲーム中 ====
public:
    AppMode   appMode;

    // 入力モード
    InputMode inputMode;

    // ゲームステータス
    GameStatus gameStatus;

    // ポーズする前のGameStatus
    GameStatus gameStatus_BeforePause;

    // タイトル画面で選択中のレベル
    UINT selectedLevel;

    // スコア
    UINT score;

    // カーソールモード
    CursorMode cursorMode;

    // カーソールモードとカーソール画像オブジェクト
    std::map<CursorMode, HCURSOR> cursorModeWithHCURSOR;

    // ==== データ ====
public:
    // サウンドデバイス
    Sound* soundDevice;

    // セーブデータ
    SaveData saveData;

public:
    // Canvasオブジェクトをインスタンス化
    // @param re_instantiate : 解放して再生成
    template <class T>
    T* InstanceCanvas(bool re_instantiate = false);

    // ボタンテキストの状態を更新
    void UpdateTextButtonArrowState();

    // すべてのゲームオブジェクトの入力を有効化
    void EnableAllGameObjectInput();

    // すべてのゲームオブジェクトの入力を無効化
    void DisableAllGameObjectInput();

public:
    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;

    virtual void EventGamepadButtonPressed(GamepadButtonEvent e);
};
