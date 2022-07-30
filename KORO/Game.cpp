#include "Game.hpp"
#include "Stage.hpp"
#include "Canvases.hpp"
#include "Queue.hpp"
#include "Delay.hpp"
#include "Camera.hpp"
#include "Timer.hpp"
#include "Particle.hpp"
#include "GoalFlag.hpp"
#include "FontManager.hpp"
#include "resource.h"

Game::Game(AppMode appMode) :
    GameObject            (ObjectType::GAME),
    appMode               (appMode),
    inputMode             (InputMode::RAW),
    gameStatus            (GameStatus::IN_TITLE),
    gameStatus_BeforePause(GameStatus::IN_TITLE),
    saveData              ({}),
    selectedLevel         (0),
    score                 (0),
    cursorMode            (CursorMode::MOUSE_ARROW)
{
    Game::instance = this;
    this->EnableInput();

    Game::LISTEN_EVENT_ROOT = (appMode == AppMode::LEVEL_EDITOR);

    // フォント読み込み
    FontManager::ResourceManager::LoadFontFromResource(FONT_MAIN);

    // CursorModeとカーソール画像MAP
    this->cursorModeWithHCURSOR =
    {
        { CursorMode::MOUSE_ARROW, (HCURSOR)LoadImageA(nullptr, "resources/icon/mouse-arrow.ico", IMAGE_CURSOR, 64, 64, LR_LOADFROMFILE) },
        { CursorMode::MOUSE_HAND,  (HCURSOR)LoadImageA(nullptr, "resources/icon/mouse-hand.ico",  IMAGE_CURSOR, 52, 52, LR_LOADFROMFILE) },
        
        { CursorMode::HAND_OPEN,          (HCURSOR)LoadImageA(nullptr, "resources/icon/hand-open.ico",          IMAGE_CURSOR, 120, 120, LR_LOADFROMFILE) },
        { CursorMode::HAND_HOLD,          (HCURSOR)LoadImageA(nullptr, "resources/icon/hand-hold.ico",          IMAGE_CURSOR,  96,  96, LR_LOADFROMFILE) },
        { CursorMode::HAND_HOLD_IN_MOVIE, (HCURSOR)LoadImageA(nullptr, "resources/icon/hand-hold-in-movie.ico", IMAGE_CURSOR,  96,  96, LR_LOADFROMFILE) },
        
        { CursorMode::WAITING, LoadCursorA(nullptr, IDC_WAIT) }
    };

    ActorManager::Setup();

    // サウンドインスタンス化
    this->soundDevice = new Sound();
}

Game::~Game()
{
    // フォント開放
    FontManager::ResourceManager::ReleaseAllFontResource();

    // フォント描画アイテム開放
    FontManager::RenderManager::ReleaseAllRenderItems();

    // カーソールアイコン解放
    for (auto& map : this->cursorModeWithHCURSOR)
    {
        DeleteObject(map.second);
    }

    SAFE_DELETE(this->soundDevice);
}

void Game::CustomEvent__LaunchedGame()
{
    Game::LISTEN_EVENT_ROOT = true;
}

void Game::CustomEvent__ButtonEnter_NewGame()
{
    this->saveData.level = 0;
    this->CustomEvent__StartGame();
}

void Game::CustomEvent__ButtonEnter_Continue()
{
    this->CustomEvent__StartGame();
}

void Game::CustomEvent__ButtonEnter_StageSelect()
{
    // タイトル要素隠す
    this->InstanceCanvas<TitleScreen>()->StartHideAnimation();

    // ステージ選択画面を出す
    this->InstanceCanvas<StageSelectScreen>()->StartShowAnimation();
}

void Game::CustomEvent__ButtonEnter_StageSelect(UINT num)
{
    // せーうデータを更新
    this->saveData.level = static_cast<byte>(num);

    // スタートゲーム
    this->CustomEvent__StartGame();
}

void Game::CustomEvent__ButtonEnter_StageSelectBack()
{
    // タイトル要素を出す
    this->InstanceCanvas<TitleScreen>()->StartShowAnimation();

    // ステージ選択画面を隠す
    this->InstanceCanvas<StageSelectScreen>()->StartHideAnimation();
}

void Game::CustomEvent__ButtonEnter_Quit()
{
    // ROOT = false
    Game::LISTEN_EVENT_ROOT = false;

    // 入力停止
    for (auto& gameObject : GameObject::all) gameObject->DisableInput();

    // FadeAnimをゲーム終了モードに
    this->InstanceCanvas<Canvas_FadeAnimation>()->SetFadeSpeed(Canvas_FadeAnimation::Speed::LAUNCH_AND_QUIT);
    this->InstanceCanvas<Canvas_FadeAnimation>()->SetFadeMode(Canvas_FadeAnimation::Mode::FADE_OUT_WITH_QUIT_GAME);
}

void Game::CustomEvent__Hovered_NewGame(bool enforce)
{
    if (this->selectedLevel != 0 || enforce)
    {
        // 読み込み
        Stage::instance->LoadStageByNum(0);

        // KORO位置リセット
        DELAY([&]()
        {
            Koro::instance->ResetLocation();
        }, 10);

        this->selectedLevel = 0;
    }
}

void Game::CustomEvent__Hovered_Continue(bool enforce)
{
    if (this->selectedLevel != this->saveData.level || enforce)
    {
        // 読み込み
        Stage::instance->LoadStageByNum(this->saveData.level);

        // KORO位置リセット
        DELAY([&]()
        {
            Koro::instance->ResetLocation();
        }, 10);

        this->selectedLevel = this->saveData.level;
    }
}

void Game::CustomEvent__Hovered_StageSelect(UINT num)
{
    // =========================================
    // バグ Fixed
    // なぜかこの関数がステージ選択時に呼び出される
    if (Game::instance->gameStatus != GameStatus::IN_TITLE) return;

    if (this->selectedLevel != num)
    {
        // 読み込み
        Stage::instance->LoadStageByNum(num);

        // KORO位置リセット
        DELAY([&]()
        {
            Koro::instance->ResetLocation();
        }, 10);

        this->selectedLevel = num;
    }
}

void Game::CustomEvent__StartGame()
{
    this->gameStatus = GameStatus::IN_MOVIE;

    // カメラを動かす
    Camera::instance->MoveTitleCamera();

    // カメラの入力を無効化
    Camera::instance->DisableInput();

    // 不要なデータの解放
    Stage::instance->MinimalizeData();

    // タイトルキャンバスを非表示
    this->InstanceCanvas<TitleScreen>()->Hide();
    this->InstanceCanvas<TitleScreen>()->StartHideAnimation();
    this->InstanceCanvas<StageSelectScreen>()->StartHideAnimation();

    // DayNightCycleSkyBoxコントローラーを非表示
    this->InstanceCanvas<DayNightCycleSkyBoxController>()->Hide();

    // 操作ガイド
    this->InstanceCanvas<Canvas_ControlGuid>(true);

    // BGM再生
    this->soundDevice->StopAll();
    this->soundDevice->PlayBGM(SoundFileBGM::Playing2, true);
}

void Game::CustomEvent__TitleCameraMoveEnded()
{
    this->InstanceCanvas<GuideText>(true);

    DELAY([&]()
    {
        this->CustomEvent__ShowInfoBoardAndLevelBoard();
    }, 3000);
}

void Game::CustomEvent__SetInfoBoardAndLevelBoard()
{
    // レベル表示ボードのテキストを現在のレベルに
    this->InstanceCanvas<LevelBoard>()->GetGuiChildByName<FontGui>("text")->SetText(std::format("{}", this->saveData.level + 1));

    // モード0に設定
    this->InstanceCanvas<LevelBoard>()->mode = 0;

    DELAY([&]
    {
        // モード1に設定
        this->InstanceCanvas<LevelBoard>()->mode = 1;
    }, 2000);
}

void Game::CustomEvent__ShowInfoBoardAndLevelBoard()
{
    // レベル表示を適用
    //this->InstanceCanvas<Canvas_InfoBoard>(true)->GetGuiChildByName("stage")->SetText(std::format("{}", this->saveData.level + 1));
    this->InstanceCanvas<Canvas_InfoBoard>(true);

    // 試行（リトライ後はここで再生開始）
    this->soundDevice->PlayBGM(SoundFileBGM::Playing2, true);

    // タイマーを生成
    new Timer(60);

    DELAY([&]
    {
        this->CustomEvent__SetInfoBoardAndLevelBoard();

        // BGM再生（ステージクリア遷移後はここで再生開始）
        this->soundDevice->PlayBGM(SoundFileBGM::Playing2, true);
    }, 500);

    DELAY([&]
    {
        // ステージの入力を有効化
        Stage::instance->EnableInput();

        // カメラの入力を有効化
        Camera::instance->EnableInput();

        // PLAYINGに変更
        this->gameStatus = GameStatus::PLAYING;

        // タイマースタート
        Timer::instance->SetTime(60.0f);
        Timer::instance->Start();
    }, 500 + 2000 + 500);
}

void Game::CustomEvent__StageCleared()
{
    // レベルエディターモードの時は無視
    if (this->appMode == AppMode::LEVEL_EDITOR) return;

    // CLEAREDに変更
    this->gameStatus = GameStatus::CLEARED;

    // タイマー停止
    Timer::instance->Pause();

    // ステージの入力を無効化
    Stage::instance->DisableInput();

    // カメラの入力を無効化
    Camera::instance->DisableInput();

    // サウンド全停止
    this->soundDevice->StopAll();

    DELAY([&]()
    {
        // ステージの回転をリセット
        Stage::instance->ResetHandle();
        Koro::instance->SetSpeed({ 0,0,0 });
    }, 490);

    DELAY([&]()
    {
        // カメラをクリアモードに
        Camera::instance->MoveClearCamera();

        // DayNightCycleSkyBoxのオフセットモードをクリアモードに設定
        DayNightCycleSkyBox::instance->SetOffsetMode(SkyBoxOffsetMode::CLEARED);
    }, 500);

    DELAY([&]()
    {
        // 全クリ
        if (this->saveData.level >= MAX_STAGE_NUM)
        {
            // おめでとうスプライト
            this->InstanceCanvas<Congratulation>()->Show();

            // クリア音再生
            this->soundDevice->PlaySE(SoundFileSE::ClearSound1, false);

            // 全クリ
            this->gameStatus = GameStatus::ALL_CLEARED;

            // 全クリしました
            this->saveData.allCleared = true;
        }
        // それ以外
        else
        {
            // ステージクリアスプライト
            this->InstanceCanvas<StageClearedCanvas>(true)->SetClearTimeText();

            // 操作ガイド非表示
            this->InstanceCanvas<Canvas_ControlGuid>()->Hide();

            // パーティクルを出す
            (new Particle(100, ParticleName::PAPER_SHOWER))->Summon(GoalFlag::instance->GetWorldLocation());

            // KORO跳ねさせる
            Koro::instance->SetSpeed(Stage::instance->GetUpVector() * Window::instance->deltaTime * 10.0f);

            // クリア音再生
            this->soundDevice->PlaySE(SoundFileSE::ClearSound2, false);
        }

        this->InstanceCanvas<Canvas_InfoBoard>()->Hide();

    }, 500 + 1500);
}

void Game::CustomEvent__TimesUp()
{
    // タイプアップキャンバス非表示
    this->InstanceCanvas<TimesUpCanvas>()->Show();

    // 入力を無効化
    Stage::instance->DisableInput();
    Camera::instance->DisableInput();

    // サウンド再生
    this->soundDevice->StopAll();
    this->soundDevice->PlaySE(SoundFileSE::TimeUp, false);
}

void Game::CustomEvent__Retry()
{
    this->gameStatus = GameStatus::IN_MOVIE;

    // レベル情報表示
    this->CustomEvent__ShowInfoBoardAndLevelBoard();

    // 操作ガイド表示
    this->InstanceCanvas<Canvas_ControlGuid>(true)->Show();

    // タイプアップキャンバス非表示
    this->InstanceCanvas<TimesUpCanvas>()->Hide();

    // ステージクリアキャンバス非表示
    this->InstanceCanvas<StageClearedCanvas>()->Hide();

    // パーティクル全開放
    Particle::ReleaseAll();

    // タイマーリセット
    Timer::instance->Reset();

    // ステージ回転リセット
    Stage::instance->ResetHandle();
    Stage::instance->SetWorldRotation(0.0f, 0.0f, 0.0f);

    // KORO位置リセット
    Koro::instance->ResetLocation();

    // カメラリセット
    Camera::instance->CameraResetToInGame();

    // スコアを0に戻す
    this->score = 0;

    DELAY([&]
    {
        // ステージの入力を有効化
        Stage::instance->LoadStageByNum(this->saveData.level);
    }, 500 + 2000 + 500);
}

void Game::CustomEvent__NextGame()
{
    if (this->gameStatus == GameStatus::CLEARED)
    {
        // サウンド再生
        this->soundDevice->StopAll();
        this->soundDevice->PlaySE(SoundFileSE::NextStage1, false);

        // カットインアニメーション開始
        this->InstanceCanvas<CutInAnimationCanvas>(true);

        this->cursorMode = CursorMode::MOUSE_ARROW;

        DELAY([&]()
        {
            this->saveData.level++;

            this->CustomEvent__ShowInfoBoardAndLevelBoard();

            // ステージ生成
            Stage::instance->LoadStageByNum(this->saveData.level);

            // KORO位置リセット
            DELAY([&]()
            {
                Koro::instance->ResetLocation();
            }, 10);

            // ステージ回転リセット
            Stage::instance->ResetHandle();
            Stage::instance->SetWorldRotation(0.0f, 0.0f, 0.0f);

            this->gameStatus = GameStatus::IN_MOVIE;

            // ステージクリア非表示
            this->InstanceCanvas<StageClearedCanvas>()->Hide();

            // 操作ガイド表示
            this->InstanceCanvas<Canvas_ControlGuid>()->Show();

            Camera::instance->CameraResetToInGame();

            // 全パーティクル解放
            Particle::ReleaseAll();

            // スコアを0に戻す
            this->score = 0;

            Timer::instance->Reset();
        }, 2900);
    }
}

void Game::CustomEvent__BeginBackToTitleScreen()
{
    // ポーズ画面からの場合はResume呼び出し
    if (this->gameStatus == GameStatus::PAUSING)
    {
        this->CustomEvent__ResumeGame();
    }

    // 入力無効化
    Stage::instance->DisableInput();
    Camera::instance->DisableInput();

    // サウンド停止
    this->soundDevice->StopAll();

    // FadeAnimをタイトル戻るモードに
    this->InstanceCanvas<Canvas_FadeAnimation>()->SetFadeSpeed(Canvas_FadeAnimation::Speed::BACK_TO_TITLE);
    this->InstanceCanvas<Canvas_FadeAnimation>()->SetFadeMode(Canvas_FadeAnimation::Mode::FADE_OUT);
}

void Game::CustomEvent__BackToTitleScreen()
{
    if (this->gameStatus == GameStatus::CLEARED)
    {
        // 全クリ
        if (this->saveData.level >= MAX_STAGE_NUM)
        {
            this->saveData.level = 0;
        }
        // それ以外
        else
        {
            this->saveData.level++;
        }
    }

    this->gameStatus = GameStatus::IN_TITLE;
    Camera::instance->CameraResetToTitle();

    // ステージの回転リセット
    Stage::instance->ResetHandle();

    // パーティクル全開放
    Particle::ReleaseAll();

    // タイトル画面表示
    this->InstanceCanvas<TitleScreen>(true);

    // ステージクリア表示を隠す
    this->InstanceCanvas<StageClearedCanvas>()->Hide();

    // タイムアップ表示を隠す
    this->InstanceCanvas<TimesUpCanvas>()->Hide();

    // InfoBoardを隠す
    this->InstanceCanvas<Canvas_InfoBoard>()->Hide();

    // 全クリ表示を隠す
    this->InstanceCanvas<Congratulation>()->Hide();

    // DayNightCycleSkyBoxコントローラーを表示
    this->InstanceCanvas<DayNightCycleSkyBoxController>()->Show();

    this->InstanceCanvas<Canvas_ControlGuid>()->Hide();

    // タイマーリセット
    if (Timer::instance) Timer::instance->Reset();

    // スコアリセット
    this->score = 0;

    // 選択中ステージ設定
    this->selectedLevel = this->saveData.level;

    // KORO位置リセット
    Koro::instance->ResetLocation();

    DELAY([&]()
    {
        // 続けるボタンホバー時の関数を呼び出し次のステージが表示された状態にする
        this->CustomEvent__Hovered_Continue(true);
    }, 100);

    DELAY([&]()
    {
        // FadeAnimを普通のフェードインモードに
        this->InstanceCanvas<Canvas_FadeAnimation>()->SetFadeMode(Canvas_FadeAnimation::Mode::FADE_IN);
    }, 100);

    DELAY([&]()
    {
        // BGM再生
        this->soundDevice->PlayBGM(SoundFileBGM::Title1, true);
    }, 100);
}

void Game::CustomEvent__AddCoin(UINT amo)
{
    // アニメーション用コインを生成
    for (UINT i = 0; i < amo; i++)
    {
        DELAY([&]()
        {
            new GUI_Coin(GUI_Coin::Mode::MOVABLE_SPRITE);
        }, 90 * i);
    }
}

void Game::CustomEvent__PauseGame()
{
    // プレイ中のみ
    if (this->gameStatus == GameStatus::PLAYING)
    {
        // ポーズ前の状態を保存
        this->gameStatus_BeforePause = this->gameStatus;

        // ポーズ
        this->gameStatus = GameStatus::PAUSING;

        // ポーズ画面を表示
        this->InstanceCanvas<PauseMenu>(true)->Show();

        // 操作ガイド非表示
        this->InstanceCanvas<Canvas_ControlGuid>()->Hide();

        // ステージの入力を無効化
        Stage::instance->DisableInput();

        // カメラの入力を無効化
        Camera::instance->DisableInput();
    }
}

void Game::CustomEvent__ResumeGame()
{
    // ポーズ前の状態を適用
    this->gameStatus = this->gameStatus_BeforePause;

    // ポーズ画面を非表示
    this->InstanceCanvas<PauseMenu>()->Hide();

    // PLAYINGの場合
    if (this->gameStatus == GameStatus::PLAYING)
    {
        // ステージの入力を有効化
        Stage::instance->EnableInput();

        // カメラの入力を有効化
        Camera::instance->EnableInput();

        // 操作ガイド表示
        this->InstanceCanvas<Canvas_ControlGuid>()->Show();
    }
}

void Game::CustomEvent__ShowSettingsMenu()
{
    this->InstanceCanvas<Canvas_Settings>(true)->Show();
    
    if (this->gameStatus == GameStatus::PAUSING)
    {
        this->gameStatus = GameStatus::IN_SETTINGS;
        this->InstanceCanvas<PauseMenu>()->Hide();
    } else

    if (this->gameStatus == GameStatus::IN_TITLE)
    {
        this->InstanceCanvas<TitleScreen>()->Hide();
        this->InstanceCanvas<DayNightCycleSkyBoxController>()->Hide();
    }
}

void Game::CustomEvent__BackToPauseMenuFromSettings()
{
    // ポーズからの遷移はGameStatus切り替え
    this->gameStatus = GameStatus::PAUSING;

    // 表示切替

    this->InstanceCanvas<PauseMenu>()->Show();
    this->InstanceCanvas<Canvas_Settings>()->Hide();
}

void Game::CustomEvent__BackToTitleMenuFromSettings()
{
    // 表示切替

    this->InstanceCanvas<TitleScreen>()->Show();
    this->InstanceCanvas<Canvas_Settings>()->Hide();
    this->InstanceCanvas<DayNightCycleSkyBoxController>()->Show();
}

void Game::CustomEvent__CongratulationParticle(bool restart)
{
    static int elapsedTick = 0;

    if (restart) elapsedTick = -90;

    // Nフレームごとに出す
    if (elapsedTick % 90 == 0)
    {
        // パーティクルを出す
        (new Particle(50, ParticleName::PAPER_SHOWER))->Summon(GoalFlag::instance->GetWorldLocation());
    }

    elapsedTick++;
}

template<class T>
T* Game::InstanceCanvas(bool re_instantiate)
{
    static T* p = nullptr;

    if (re_instantiate)
    if (p)
    {
        p->Release();
        p = nullptr;
    }

    if (!p)
    {
        p = new T();
    }

    return p;
}

void Game::UpdateTextButtonArrowState()
{
    if (TextButtonArrow::instance == nullptr) return;

    if (Game::LISTEN_EVENT_ROOT)
    for (auto& e : GameObject::all)
    {
        if (!e->isEnableInput)                 continue;
        if ( e->objectType != ObjectType::GUI) continue;

        if (auto gui = dynamic_cast<FontGui*>(e.get()))
        if (gui->enableTextButtonArrow)
        if (gui->inputState.mouseOver)
        {
            // 位置Xをテキストボタンの少し左に設定
            TextButtonArrow::instance->position.x = gui->position.x - gui->width * 0.5f - gui->GetPosXFromPercent(2);

            // 位置Yをテキストの高さに設定
            TextButtonArrow::instance->position.y = gui->position.y;

            // 可視化
            TextButtonArrow::instance->visible = true;

            // アピールアニメーション
            TextButtonArrow::instance->position.x += cosf(GetTickCount64() * 0.007f) * 3.5f;

            // サイズを設定
            TextButtonArrow::instance->SetScaleByFontSize(gui->GetFontSize());

            return;
        }
    }

    TextButtonArrow::instance->visible = false;
}

void Game::EnableAllGameObjectInput()
{
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (GameObject::all[i]->IsValid()) GameObject::all[i]->EnableInput();
    }
}

void Game::DisableAllGameObjectInput()
{
    for (size_t i = 0; i < GameObject::all.size(); i++)
    {
        if (GameObject::all[i]->IsValid()) GameObject::all[i]->DisableInput();
    }
}

void Game::EventBeginPlay()
{
    // レベルエディターの場合はBeginPlayの処理は実行しない
    if (this->appMode == AppMode::LEVEL_EDITOR) return;

    // 処理中
    this->cursorMode = CursorMode::WAITING;

    // セーブデータを読み込み
    QUEUE << [&]()
    {
        // 読み込み
        auto connector = ValueConnector::Connect(SAVEDATA_PATH, &this->saveData);

        // 選択中のステージを設定
        this->selectedLevel = this->saveData.level;

        // セーブデータが存在しなかった場合は80%の音量に設定
        if (!connector->fileExist)
        {
            this->saveData.masterVolume = 80;
        }

        // 音量を適用
        this->soundDevice->SetVolumeBuffer(this->saveData.masterVolume * 0.01f);
    };

    // HLSLシェーダーのインスタンス化
    QUEUE << [&]()
    {
        new HLSL("resources/shader/DayNightCycleSkyBox.hlsl", "SKYBOX");
        new HLSL("resources/shader/Actor.hlsl",               "ACTOR");
        new HLSL("resources/shader/Koro.hlsl",                "KORO");
        new HLSL("resources/shader/GuiDefault.hlsl",          "GUI-DEFAULT");
        new HLSL("resources/shader/GuiPanel.hlsl",            "GUI-PANEL");
    };

    // ゲーム要素初期化
    QUEUE << [&]()
    {
        // Texture
        new Texture();

        // Gravity
        new Gravity();

        // Camera
        new Camera
        (
            D3DXVECTOR3(0.0f, 0.0f, 0.0f),
            D3DXToRadian(45.0f),
            50.0f
        );

        // 昼夜サイクルSkyBox
        new DayNightCycleSkyBox();

        // Stage
        (new Stage())->Init();

        // Stageの入力を無効化
        Stage::instance->DisableInput();

        // KORO
        (new Koro())->Init();

        // FadeAnimation
        this->InstanceCanvas<Canvas_FadeAnimation>();
    };

    // ステージを生成
    QUEUE << [&]()
    {
        Stage::instance->LoadStageByNum(this->saveData.level);
    };

    // タイトル画面Canvas読み込み
    // ステージ選択画面Canvas読み込み
    // テキストボタン矢印読み込み
    QUEUE << [&]()
    {
        // 水平メーター
        this->InstanceCanvas<HorizonMeter>();

        this->InstanceCanvas<TitleScreen>()->Show();
        this->InstanceCanvas<StageSelectScreen>()->Hide();

        auto textArrow = new TextButtonArrow();
        textArrow->visible = false;
    };

    // イベント登録
    QUEUE << [&]()
    {
        EventHandler::RegisterEvent("pop-coin") << [&]()
        {
            this->InstanceCanvas<Canvas_InfoBoard>()->PopCoin();
        };
    };

    // 読み込み終了
    QUEUE << [&]()
    {
        this->cursorMode = CursorMode::MOUSE_ARROW;
    };

    QUEUE << [&]()
    {
        // BGM再生
        auto result = this->soundDevice->PlayBGM(SoundFileBGM::Title1, true);

        // デバッグGUIスタート
        Debug::Start();

        // KORO位置リセット
        Koro::instance->ResetLocation();
    };
}

void Game::EventTick(float deltaTime)
{
    Debug::AddLine("SaveData");
    Debug::AddLine(" - Stage             : {}", this->saveData.level);
    Debug::AddLine(" - SkyBox Cycle Mode : {}", nameof::nameof_enum((SkyBoxCycleMode)this->saveData.skyboxData.cycleMode));
    Debug::AddLine(" - SkyBox Slide      : {}", this->saveData.skyboxData.slideValue);
    Debug::AddLine("");

    // プレイ中とムービー中はHAND
    if (this->gameStatus == GameStatus::IN_MOVIE ||
        this->gameStatus == GameStatus::PLAYING)
    {
        // マウスクリック
        if (GetKeyState(VK_LBUTTON) < 0)
        {
            ::SetCursor(this->cursorModeWithHCURSOR[this->gameStatus != GameStatus::IN_MOVIE ? CursorMode::HAND_HOLD : CursorMode::HAND_HOLD_IN_MOVIE]);
        }
        else
        {
            ::SetCursor(this->cursorModeWithHCURSOR[CursorMode::HAND_OPEN]);
        }
    }
    // それ以外はモード準拠
    else
    {
        // 現在のカーソールモードのカーソールアイコンに設定
        ::SetCursor(this->cursorModeWithHCURSOR[this->cursorMode]);
    }

    // ボタンテキストの状態を更新
    this->UpdateTextButtonArrowState();

    // ポーズ中は実行しない
    IF_PAUSE_RETURN;

    // レベルエディターの場合はEventTickの処理は実行しない
    if (this->appMode == AppMode::LEVEL_EDITOR) return;

    // キュー実行
    queue::excute_queue();

    // 全クリパーティクル
    if (this->gameStatus == GameStatus::ALL_CLEARED)
    {
        this->CustomEvent__CongratulationParticle(false);
    }

    if (Timer::instance->IsValid())
    {
        // 残り0秒になった場合
        if (Timer::instance->GetCurrentSecond() == 0)
        if (this->gameStatus != GameStatus::TIMES_UP)
        {
            QUEUE << [&]()
            {
                // TimedUp呼び出し
                this->CustomEvent__TimesUp();
            };

            // TIMED_UPに変更
            this->gameStatus = GameStatus::TIMES_UP;
        }
    }
}

void Game::EventKeyDown(KeyEvent e)
{
    if (this->appMode == AppMode::LEVEL_EDITOR) return;

    this->inputMode = InputMode::RAW;
}

void Game::EventKeyUp(KeyEvent e)
{
    this->inputMode = InputMode::RAW;

    if (!Game::LISTEN_EVENT_ROOT) return;

    // ポーズ切り替え
    if (e.keyCode == VK_ESCAPE)
    {
        if (this->gameStatus == GameStatus::IN_SETTINGS)
        {
            this->CustomEvent__BackToPauseMenuFromSettings();
            return;
        }

        if (this->gameStatus == GameStatus::IN_TITLE)
        {
            this->CustomEvent__BackToTitleMenuFromSettings();
            return;
        }

        this->gameStatus != GameStatus::PAUSING ? this->CustomEvent__PauseGame() : this->CustomEvent__ResumeGame();
    }

    // Debug Keys
    IF_DEBUG
    if (this->appMode == AppMode::GAME)
    {
        // Enter Pad
        if (e.keyCode == VK_RETURN)
        {
            this->InstanceCanvas<CutInAnimationCanvas>(true);
        }

        // Space Pad
        if (e.keyCode == VK_SPACE)
        {
            this->InstanceCanvas<DayNightCycleSkyBoxController>();
            (new Particle(100, ParticleName::PAPER_SHOWER))->Summon({ 0.0f, 0.0f, 0.0f });
        }

        // F1
        if (e.keyCode == VK_F1)
        {
            for (auto& e : GameObject::all)
            {
                if (auto canvas = dynamic_cast<Canvas*>(e.get()))
                {
                    canvas->Hide();
                }
            }
        }

        // R
        if (e.keyCode == 'R')
        {
            if (Timer::instance)
            {
                Timer::instance->SetTime(1.0f);
                Timer::instance->Start();
            }
        }

        if (e.keyCode == 'B')
        {
            this->InstanceCanvas<StageClearedCanvas>(true);
        }

        // S
        if (e.keyCode == 'S')
        {
            this->InstanceCanvas<Canvas_InfoBoard>(true);
        }

        // F
        if (e.keyCode == 'F')
        {
            auto fadeAnim = this->InstanceCanvas<Canvas_FadeAnimation>();
            fadeAnim->SetZIndex(0U);
        }

        // O
        if (e.keyCode == 'O')
        {
            this->InstanceCanvas<Canvas_FadeAnimation>()->ToggleFadeMode();
        }

        // @
        if (e.keyCode == VK_OEM_3)
        {
            Debug::instance->visible = !Debug::instance->visible;

            if (!Debug::instance->visible)
            {
                SendToastNotice("KORO", "デバッグテキストを非表示にしました\nもう一度表示するには@キーを押します");
            }
        }
    }
}

void Game::EventMouseDown(MouseEvent e)
{
    this->inputMode = InputMode::RAW;

    if (this->appMode == AppMode::GAME)
    {
        //this->CustomEvent__AddCoin(5);
    }
}

void Game::EventMouseUp(MouseEvent e)
{
    this->inputMode = InputMode::RAW;
}

void Game::EventGamepadButtonPressed(GamepadButtonEvent e)
{
    this->inputMode = InputMode::GAMEPAD;
}
