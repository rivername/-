#include "Stage.hpp"
#include "LevelEditor.hpp"
#include "ValueConnector.hpp"
#include "Koro.hpp"
#include "StencilHighlight.hpp"
#include "StartPoint.hpp"
#include "GoalPoint.hpp"
#include "Camera.hpp"
#include "Queue.hpp"
#include "Canvases.hpp"
#include "Viewport.hpp"

Stage::Stage()
    : GameObject(ObjectType::STAGE), Transform()
    , prevLocalMatrix()
    , yawAngle(0), l_x_axis(0), l_y_axis(0)
    , beginDragPos({}), endDragPos({})
    , isMouseDragging(false), dragDirection({ 0.0f, 0.0f }), dragLength(0.0f)
{
    Stage::instance = this;

    D3DXMatrixIdentity(&this->worldMatrix);
    D3DXMatrixIdentity(&this->prevLocalMatrix);

    // あらかじめ要領を確保
    this->levelDatas.resize(MAX_STAGE_NUM + 1);
    this->actors.resize(MAX_STAGE_NUM + 1);

    new GoalFlag();
}

inline IDirect3DCubeTexture9* cubeTex;

Stage::~Stage()
{
    SAFE_RELEASE(cubeTex);
}

bool Stage::Init()
{
    return D3DXCreateCubeTextureFromFileA(
        DX9::instance->device3d, "resources/textures/ReflectMap.dds", &cubeTex
    ) == S_OK;
}

bool Stage::IsMoving()
{
    return (this->localMatrix != this->prevLocalMatrix);
}

bool Stage::LoadStageByNum(UINT num)
{
    using namespace level_data;

    // 既に読み込んでいるかどうか
    bool alreadyLoaded = (!this->levelDatas[num].empty());

    // ロード済でなかった場合
    if (!alreadyLoaded)
    {
        // ファイルとリンク // 読み込み専用
        ValueConnector::Connect
        (
            std::format("{}{}{}.{}", folder_path, file_name, num, extension),
            &this->levelDatas[num],
            false, true
        );
    }

    // 読み込むステージActor以外のIgnore=true
    for (UINT i = 0; i < this->actors.size(); i++)
    {
        if (i == num) continue;

        for (auto actor : this->actors[i])
        {
            if (actor->GetActorType() != KoroActorType::GOAL_POINT &&
                actor->GetActorType() != KoroActorType::START_POINT)
            {
                actor->isIgnore = true;
            }
        }
    }

    // スタートポイントとゴールポイントを設定
    // @param data : ActorSaveData
    static auto SetStartPointAndGoalPoint = [](ActorSaveData data)
    {
        // 行列だけ設定
        if (data.actorType == KoroActorType::START_POINT) StartPoint::instance->SetLocalMatrix(&data.worldMatrix);
        if (data.actorType == KoroActorType::GOAL_POINT)  GoalPoint::instance->SetLocalMatrix(&data.worldMatrix);
    };

    // ロード済でない場合は領域確保
    if (!alreadyLoaded)
    {
        this->actors[num].reserve(this->levelDatas[num].size());
    }

    // データから該当するパターンデータを読み込み
    for (auto& data : this->levelDatas[num])
    {
        // ロード済でない場合は
        if (!alreadyLoaded)
        {
            // 生成・追加
            this->actors[num].push_back
            (
                ActorManager::Generate(data.actorType, &data.worldMatrix, this, data.id)
            );

            // スタートポイントとゴールポイントの設定
            SetStartPointAndGoalPoint(data);
        } else
        // 読み込んでいたら
        {
            // isIgnoreをfalseに
            for (auto actor : this->actors[num])
            {
                actor->isIgnore = false;
            }

            // スタートポイントとゴールポイントの設定
            SetStartPointAndGoalPoint(data);
        }
    }

    return true;
}

bool Stage::MinimalizeData()
{
    // 現在のレベル以外の読み込んだレベルデータを削除
    for (byte i = 0; i < this->levelDatas.size(); i++)
    {
        if (i != Game::instance->saveData.level) this->levelDatas[i].clear();
    }

    // 10個ずつ削除
    QUEUE_SET_STEP(10);

    // 現在のレベル以外のActorを解放
    // 負担軽減処理(Queue)を使用
    for (byte i = 0; i < this->actors.size(); i++)
    {
        if (i == Game::instance->saveData.level) continue;

        for (auto actor : this->actors[i])
        {
            if (actor->GetActorType() == KoroActorType::START_POINT) continue;
            if (actor->GetActorType() == KoroActorType::GOAL_POINT)  continue;

            QUEUE << [&]()
            {
                SAFE_RELEASE(actor);
            };
        }

        this->actors[i].clear();
    }

    // 元に戻す
    QUEUE_SET_STEP(1);

    return true;
}

D3DXVECTOR3 Stage::GetStartPoint()
{
    D3DXVECTOR3 result = { 0.0f, 5.0f, 0.0f };

    if (StartPoint::Instantiate())
    {
        result = StartPoint::Instantiate()->GetWorldLocation();
    }

    return result;
}

void Stage::TitleStageRotation()
{
    Debug::AddLine("Yaw: {}", D3DXToDegree(this->yawAngle));

    // Yawをゆっくり回転させる
    if (Game::instance->gameStatus == GameStatus::IN_TITLE) this->yawAngle += D3DXToRadian(0.1f);

    // 0 ~ 360で循環
    if (this->yawAngle >= D3DXToRadian(359.99f))
    {
        this->yawAngle = 0.0f;
    }
}

void Stage::ReturnStageRotationLerpToInit()
{
    if (Game::instance->gameStatus != GameStatus::IN_MOVIE) return;

    if (this->yawAngle != 0.0f)
    {
        if (D3DXToDegree(this->yawAngle) < 180)
        {
            // ゆっくり戻す
            this->yawAngle -= 0.1f;

            // 0まで
            this->yawAngle = std::clamp(this->yawAngle, 0.0f, this->yawAngle + 0.1f);
        }
        else
        {
            // ゆっくり戻す
            this->yawAngle += 0.1f;

            // 0まで
            this->yawAngle = std::clamp(this->yawAngle, this->yawAngle - 0.1f, D3DXToRadian(359.99f));
        }
    }
}

void Stage::GetMouseInput()
{
    // カーソール位置
    POINT cursor = Window::instance->GetMouseCursorPos();

    // ドラッグ中に計算
    if (this->isMouseDragging)
    {
        // 終端座標はドラッグ中は常にマウス座標
        this->endDragPos = { cursor.x, cursor.y };

        // 方向 x, y
        float x = static_cast<float>(this->endDragPos.x - this->beginDragPos.x);
        float y = static_cast<float>(this->endDragPos.y - this->beginDragPos.y);

        // ドラッグ方向
        this->dragDirection = { x, -y };

        // ドラッグの長さ
        this->dragLength = D3DXVec2Length(&this->dragDirection);

        ::D3DXVec2Normalize(&this->dragDirection, &this->dragDirection);
    }

    Debug::AddLine("Drag Length: {}", this->dragLength);
    Debug::AddLine("Drag Direction: {}, {}", this->dragDirection.x, this->dragDirection.y);

    // ドラッグ前の値
    static float prev_l_x_axis = this->l_x_axis;
    static float prev_l_y_axis = this->l_y_axis;

    if (isMouseDragging)
    {
        // ステージを傾ける

        this->l_x_axis = prev_l_x_axis + (this->dragDirection.x * this->dragLength * 0.002f);
        this->l_y_axis = prev_l_y_axis + (this->dragDirection.y * this->dragLength * 0.002f);
    }
    else
    {
        // 現在の値を保存

        prev_l_x_axis = this->l_x_axis;
        prev_l_y_axis = this->l_y_axis;
    }
}

void Stage::RotateStage()
{
    /***** ステージ回転 *****/
    D3DXQUATERNION quat, temp;

    temp = { 0.0f, 0.0f, 0.0f, 1.0f };

    this->l_x_axis = std::clamp(this->l_x_axis, -1.0f, 1.0f);
    this->l_y_axis = std::clamp(this->l_y_axis, -1.0f, 1.0f);

    D3DXVECTOR3 vec = { 1,0,0 };
    D3DXQuaternionRotationAxis(&quat, &vec, stage::MAX_ANGLE_Y * this->l_y_axis);
    temp *= quat;
    this->SetWorldQuaternion(&temp, false);

    vec = { 0, this->GetUpVector().z, -abs(this->GetUpVector().y) };
    D3DXQuaternionRotationAxis(&quat, &vec, stage::MAX_ANGLE_X * this->l_x_axis);
    temp *= quat;
    this->SetWorldQuaternion(&temp, false);

    vec = this->GetUpVector();
    D3DXQuaternionRotationAxis(&quat, &vec, this->yawAngle);
    temp *= quat;
    this->SetWorldQuaternion(&temp);
}

void Stage::ResetHandle()
{
    this->yawAngle        = 0;
    this->l_x_axis        = 0;
    this->l_y_axis        = 0;
    this->isMouseDragging = false;
    this->beginDragPos    = {};
    this->endDragPos      = {};
}

bool Stage::InterpToInitialRotation()
{
    this->isMouseDragging = false;
    this->beginDragPos    = {};
    this->endDragPos      = {};

    bool x = InterpTo(&this->l_x_axis, this->l_x_axis, 0.0f, 1.0f);
    bool z = InterpTo(&this->l_y_axis, this->l_y_axis, 0.0f, 1.0f);
    
    return (x && z);
}

void Stage::GetSortedHLSLObjects(std::vector<HLSLObject*>* pOutFront, std::vector<HLSLObject*>* pOutBack)
{
    // 描画するステージの番号
    UINT renderStageNum =
        Game::instance->gameStatus != GameStatus::IN_TITLE ?
        Game::instance->saveData.level : // 普通の時
        Game::instance->selectedLevel;   // タイトル画面の時

    // 該当するステージのActorループ
    for (auto actor : this->actors[renderStageNum])
    {
        if (actor->isIgnore) continue;

        D3DXVECTOR3 eye        = Camera::instance->camera_position;
        D3DXVECTOR3 koroLoc    = Koro::instance->GetWorldLocation();
        D3DXVECTOR3 dirToKoro  = (koroLoc - eye);
        D3DXVECTOR3 dirToActor = (actor->GetWorldLocation() - eye);

        // カメラとの距離でKOROとの前後を判定
        bool inFront = D3DXVec3Length(&dirToKoro) > D3DXVec3Length(&dirToActor);

        // それぞれのリストに追加
        (inFront ? pOutFront : pOutBack)->push_back(actor);
    }
}

void Stage::DisableInput()
{
    this->GameObject::DisableInput();

    this->isMouseDragging = false;
}

void Stage::EventBeginPlay()
{
    auto hlsl = HLSL::GetHLSLByTag("ACTOR");
    hlsl->effect->SetTexture("cubeTex", cubeTex);
    hlsl->effect->CommitChanges();
}

void Stage::EventTick(float deltaTime)
{
    static D3DXVECTOR3 cameraLoc = { 0.0f, 0.0f, 0.0f };

    // 環境マップ使用設定
    HLSL::GetHLSLByTag("ACTOR")->effect->SetBool("useEnvMap", Game::instance->gameStatus != GameStatus::IN_TITLE);

    if (Camera::instance)
    {
        // カメラ位置
        if (Game::instance->gameStatus != GameStatus::CLEARED)
        {
            cameraLoc = Camera::instance->GetCameraLocation();
        }

        D3DXVECTOR3 camPos3 = cameraLoc;
        D3DXVECTOR4 camPos4 = { camPos3.x, camPos3.y, camPos3.z, 1.0f };

        // カメラ位置設定
        HLSL::GetHLSLByTag("ACTOR")->effect->SetVector("cameraPos", &camPos4);
    }

    IF_NOT_PAUSING
    {
        this->prevLocalMatrix = this->localMatrix;

        if (!GameObject::IsGamepadConnected()) // キーボード時
        {
            this->GetMouseInput();
        }

        DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);

        if (Game::instance->appMode != AppMode::LEVEL_EDITOR)
        {
            this->TitleStageRotation();
            this->ReturnStageRotationLerpToInit();
        }

        // アシストモード：レベル2
        // マウスを離すとステージの回転がゆっくりと元に戻る
        if (Game::instance->saveData.assistModeLevel == 2)
        if (!this->isMouseDragging)
        {
            this->InterpToInitialRotation();
        }
    }

    // ステージの回転制御
    this->RotateStage();

    std::vector<HLSLObject*> frontList; // KOROより手前
    std::vector<HLSLObject*> backList;  // KOROより後ろ
    this->GetSortedHLSLObjects(&frontList, &backList);

    Debug::AddLine("Front Of KORO: {}", frontList.size());
    Debug::AddLine("Behind KORO: {}", backList.size());

    {
        /*==================================
                        初期化
        ==================================*/
        StencilHighlight::Fun_1_Init(DX9::instance->device3d);
    }

    {
        /*==================================
                マスクするオブジェクトより後ろ
        ==================================*/
        StencilHighlight::Fun_2_StartRenderingBackgroundObject();

        HLSL::GetHLSLByTag("SKYBOX")->RenderProcess();

        HLSL::GetHLSLByTag("ACTOR")->effect->SetBool("bHighlight", false);
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(backList);
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(GoalFlag::instance);
    }

    {
        /*==================================
                マスクに使うオブジェクト
        ==================================*/
        StencilHighlight::Fun_3_StartRenderingHighlightTarget();

        HLSL::GetHLSLByTag("KORO")->RenderProcess();
    }

    {
        /*==================================
                   オブジェクトより前
                   重なった部分を描画
        ==================================*/
        StencilHighlight::Fun_4_StartRenderingHighlight();

        HLSL::GetHLSLByTag("ACTOR")->effect->SetBool("bHighlight", true);
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(frontList);
    }

    {
        /*==================================
                   オブジェクトより前
                 重なった部分以外を描画
        ==================================*/
        StencilHighlight::Fun_5_StartRenderingFrontObject();

        HLSL::GetHLSLByTag("ACTOR")->effect->SetBool("bHighlight", false);
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(frontList);
    }

    {
        /*==================================
                      終了処理
        ==================================*/
        StencilHighlight::Fun_6_EndRendering();
    }

    auto hlsl_gui_defalt = HLSL::GetHLSLByTag("GUI-DEFAULT");
    auto hlsl_gui_panel  = HLSL::GetHLSLByTag("GUI-PANEL");

    for (auto& list : Gui::GetSortedByZIndexList())
    {
        for (auto hlslObj : list)
        {
            // SpriteGuiとFontGui
            if (!dynamic_cast<PanelGui*>(hlslObj))
            {
                hlsl_gui_defalt->RenderProcess(hlslObj);
            }
            // PanelGui
            else
            {
                hlsl_gui_panel->RenderProcess(hlslObj);
            }
        }
    }

    Debug::AddLine("\n\nDelta Time: {}", Window::instance->deltaTime);
}

void Stage::EventKeyDown(KeyEvent e)
{
    IF_DEBUG
    {
        if (Game::instance->appMode != AppMode::LEVEL_EDITOR)
        {
            if (e.keyCode == '0')
            {
                this->LoadStageByNum(0);
            }
            if (e.keyCode == '1')
            {
                this->LoadStageByNum(1);
            }
            if (e.keyCode == '2')
            {
                this->LoadStageByNum(2);
            }
            if (e.keyCode == '3')
            {
                this->LoadStageByNum(3);
            }
            if (e.keyCode == '4')
            {
                this->LoadStageByNum(4);
            }
        }
    }
}

void Stage::EventKeyUp(KeyEvent e)
{
}

void Stage::EventMouseDown(MouseEvent e)
{
    if (e.button == MouseEvent::Button::LEFT)
    {
        this->beginDragPos = e.position;
        this->isMouseDragging = true;
    }
}

void Stage::EventMouseUp(MouseEvent e)
{
    if (e.button == MouseEvent::Button::LEFT)
    {
        this->endDragPos = e.position;
        this->isMouseDragging = false;
    }
}

void Stage::EventGamepadAxisInput(GamepadAxisEvent e)
{}

void Stage::EventTransformUpdated()
{}
