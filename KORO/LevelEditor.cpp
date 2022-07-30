#include "LevelEditor.hpp"

#if _DEBUG

#include "GUIs.hpp"
#include "Camera.hpp"
#include "Koro.hpp"
#include "ValueConnector.hpp"
#include "Particle.hpp"
#include "ConvexHull.hpp"

#include "TriggerBox.hpp"
#include "GoalPoint.hpp"
#include "StartPoint.hpp"

#include "ActorManager.hpp"

#include "Viewport.hpp"

#include "Queue.hpp"
#include "FontManager.hpp"
#include "resource.h"


LevelEditor::LevelEditor(int editLevelNum)
    : GameObject()
    , editLevelNum(editLevelNum), editMode(EditMode::LOCATION), editorCamera(new EditorCamera())
    , actors(), selectActors(), levelData()

    , actorEditUnit(1.0f)

    , samplesMap(), showSampleType(SampleType::CUBE)
    , sampleViewportSize(static_cast<LONG>(Window::instance->width / 10U))
    , sampleViewportsOrigin({ 0, static_cast<LONG>(Window::instance->height - (LONG)sampleViewportSize) })
    , selectedSampleIndex(-1), hoveredSampleIndex(-1)
    , selectedSample(nullptr), selectedSampleDistance(0)
    , currentMousePos(), preMousePos(), dragStartPos()
    , visibleGrid(false)
    , isSaved(false)
    , defaultSurface(nullptr), objectJudgeSurface(nullptr)
    , shader(nullptr), error(nullptr), shaderHandler(nullptr)
{
    LevelEditor::instance = this;
	this->Init();
}

LevelEditor::~LevelEditor()
{
    // フォント開放
    FontManager::ResourceManager::ReleaseAllFontResource();

    // フォント描画アイテム開放
    FontManager::RenderManager::ReleaseAllRenderItems();

    SAFE_RELEASE(this->shaderHandler);
    SAFE_RELEASE(this->error);
    SAFE_RELEASE(this->shader);
    SAFE_RELEASE(this->objectJudgeSurface);
    SAFE_RELEASE(this->defaultSurface);

	OutputDebugFormat("-- [ {} ] called. --\n", __func__);
}

void LevelEditor::Init()
{
	// クリアカラー変更
	Window::instance->clearColor = D3DCOLOR_XRGB(100,150,150);

	this->EnableInput();

    // フォント読み込み
    FontManager::ResourceManager::LoadFontFromResource(FONT_MAIN);

    // パターンアクター生成準備
    ActorManager::Setup();

    // HLSLインスタンス化
    new HLSL("resources/shader/DayNightCycleSkyBox.hlsl", "SKYBOX");
    new HLSL("resources/shader/Actor.hlsl", "ACTOR");

    new HLSL("resources/shader/Koro.hlsl", "KORO");
    new HLSL("resources/shader/GuiDefault.hlsl", "GUI-DEFAULT");

    new Texture();

    new GoalFlag();


    // デバッグGUIスタート
    Debug::Start();

    //////////////////////////////////////////////////
    // クリック判定描画用 設定

    // デバイスのバックバッファを取得
    DX9::instance->device3d->GetRenderTarget(0, &this->defaultSurface);

    // バックバッファサーフェイスの属性を取得
    D3DSURFACE_DESC surfaceDesc;
    this->defaultSurface->GetDesc(&surfaceDesc);

    // オブジェクト判定サーフェイスの新規作成
    DX9::instance->device3d->CreateRenderTarget(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, surfaceDesc.MultiSampleType, 0, true, &this->objectJudgeSurface, NULL);

    // ピクセルシェーダプログラム
    // レジスタc0の色でオブジェクトを塗りつぶします
    const char PxShader[] =
        "ps_1_1 \n"
        "mov      r0,   c0\n";   // 色をc0に決定

                                    // シェーダ命令のコンパイル
    D3DXAssembleShader(PxShader, sizeof(PxShader) - 1, 0, NULL, 0, &this->shader, &this->error);

    // シェーダハンドラ
    DX9::instance->device3d->CreatePixelShader((DWORD*)shader->GetBufferPointer(), &this->shaderHandler);

    // 参考 : http://marupeke296.com/DXPS_S_No2_PointingDrawObject.html
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
    // サンプル生成

    for (auto& data : ActorManager::patternLoader->datas)
    {
        if (data.actorType == KoroActorType::__num) continue;

        // Cube と X_Model に振り分け
        switch (data.actorType)
        {
        case KoroActorType::CUBE:

            this->samplesMap[SampleType::CUBE].push_back(ActorManager::Generate(data.actorType, nullptr, nullptr, data.id));

            break;

        case KoroActorType::X_MODEL:

            this->samplesMap[SampleType::XMODEL].push_back(ActorManager::Generate(data.actorType, nullptr, nullptr, data.id));

            break;

        default:
            break;
        }
    }

    for (int i = 0; i < (int)KoroActorType::__num; ++i)
    {
        if (i != (int)KoroActorType::CUBE
            && i != (int)KoroActorType::X_MODEL
            && i != (int)KoroActorType::START_POINT
            && i != (int)KoroActorType::GOAL_POINT)
        {
            this->samplesMap[SampleType::OTHER].push_back(ActorManager::Generate((KoroActorType)i));
        }
    }

    //////////////////////////////////////////////////
    // 編集するデータをロード / 新規作成
    this->LoadOrCreateLevelData();

}

void LevelEditor::LoadOrCreateLevelData()
{
    // 読み込み / 新規作成
    ValueConnector::Connect(std::format("{}{}{}.{}", level_data::folder_path, level_data::file_name, this->editLevelNum, level_data::extension), &this->levelData);

    this->actors.reserve(this->levelData.size());

    //////////////////////////////////////////////////
    // デフォルト生成

    // スタートポイント
    auto startPoint = StartPoint::Instantiate();
    startPoint->visible = true;
    startPoint->SetWorldLocation(-1, 0, 1);
    this->actors.push_back(startPoint);

    // ゴールポイント
	auto goalPoint = GoalPoint::Instantiate();
	goalPoint->visible = true;
	goalPoint->SetWorldLocation(1, 0, -1);
	this->actors.push_back(goalPoint);

    // 読み込んだデータからActorを生成
    for (auto& data : this->levelData)
    {
        auto newActor = ActorManager::Generate((KoroActorType)data.actorType, &data.worldMatrix, nullptr, data.id);

        if (newActor)
        {
            // スタートポイントとゴールポイント
            // 行列だけ設定
            if (data.actorType == KoroActorType::START_POINT) startPoint->SetLocalMatrix(&data.worldMatrix);
            else if (data.actorType == KoroActorType::GOAL_POINT)  goalPoint->SetLocalMatrix(&data.worldMatrix);
            else
            {
                this->actors.push_back(newActor);
            }
        }
    }
}

void LevelEditor::SaveLevelData()
{
    this->levelData.clear();
    this->levelData.reserve(this->actors.size());

    for (auto& actor : this->actors)
    {
        ActorSaveData tempLevelData =
        {
            .id = "",
            .actorType = actor->GetActorType(),
            .worldMatrix  = actor->GetWorldMatrix()
        };

        strcpy_s(tempLevelData.id, actor->id);

        this->levelData.push_back(tempLevelData);
    }

    // 書き込み
    ValueConnector::SaveAll();

    // 保存チェック
    this->isSaved = true;
}

void LevelEditor::AddSelectActor(Actor* addActor)
{
    if (!addActor) return;

    for (auto& selectActor : this->selectActors)
    {
        selectActor->BecomeParents(addActor);
    }

    // 選択配列に追加
    this->selectActors.push_back(addActor);
}

void LevelEditor::AddSelectActors(std::vector<Actor*>* addActors)
{
    if (!addActors || addActors->empty()) return;

    // 領域確保
    this->selectActors.reserve(this->selectActors.size() + addActors->size());

    // 既に選択済みのactorを子に
    for (auto& selectActor : this->selectActors)
    {
        selectActor->BecomeParents(addActors->back());
    }

    for (auto& actor : *addActors)
    {
        if (!std::any_of(this->selectActors.begin(), this->selectActors.end(), [&](const Actor* selectActor) { return selectActor == actor; }))
        {
            // 選択配列に追加
            this->selectActors.push_back(actor);
        }
        if (actor != addActors->back())
        {
            actor->BecomeParents(addActors->back());
        }
    }
}

void LevelEditor::RemoveSelectActor(Actor* removeActor)
{
	if (!removeActor) return;

	for (auto& selectActor : this->selectActors)
	{
		selectActor->BreakParents();
	}

	// 配列から削除
	this->selectActors.erase
	(
		std::remove
		(
			this->selectActors.begin(),
			this->selectActors.end(),
			removeActor
		),
		this->selectActors.end()
	);

	for (auto& selectActor : this->selectActors)
	{
		if (selectActor != this->selectActors.back())
		{
			selectActor->BecomeParents(this->selectActors.back());
		}
	}
}

void LevelEditor::RemoveSelectAll()
{
    for (auto& selectActor : this->selectActors)
    {
        selectActor->BreakParents();
    }

    this->selectActors.clear();
}

void LevelEditor::DeleteSelectActor()
{
    if (this->selectActors.empty()) return;
    int deleteNum = 0; // debug

    for (auto& selectActor : this->selectActors)
    {
        selectActor->BreakParents();
    }

    for (auto& selectActor : this->selectActors)
    {
        if (dynamic_cast<StartPoint*>(selectActor) || dynamic_cast<GoalPoint*>(selectActor)) continue;

        this->actors.erase
        (
            std::remove
            (
                this->actors.begin(),
                this->actors.end(),
                selectActor
            ),
            this->actors.end()
        );
        HLSL::GetHLSLByTag("ACTOR")->RemoveHLSLObjectFromList(selectActor);
        SAFE_RELEASE(selectActor);
        ++deleteNum;
    }

    OutputDebugFormat("\ndelete num : {}", deleteNum);

    this->selectActors.clear();
    this->selectActors.shrink_to_fit();
    this->actors.shrink_to_fit();

    // 保存チェック用
    this->isSaved = false;
}

void LevelEditor::HighlightSelectActors()
{
    for (auto& actor : this->selectActors)
    {
        DebugBox::Render(&actor->GetWorldMatrix(), { 1,1,0 });
    }
}

void LevelEditor::EditSelectActors(float x, float y, float z)
{
    if (this->selectActors.empty()) return;

    // 右クリック中(カメラ操作中)は編集しない
    if (GetKeyState(VK_RBUTTON) < 0) return;

    switch (this->editMode)
    {
    case LevelEditor::EditMode::LOCATION:
    {
        this->selectActors.back()->AddWorldLocation(x, y, z);

        break;
    }
    case LevelEditor::EditMode::ROTATION:
    {
        D3DXVECTOR3 tempLocation = this->selectActors.back()->GetWorldLocation();
        D3DXMATRIX  rotMat, actorMat = this->selectActors.back()->GetWorldMatrix();
        D3DXQUATERNION quat;

        D3DXQuaternionRotationYawPitchRoll(&quat, y * D3DX_PI * 0.25f, z * D3DX_PI * 0.25f, -x * D3DX_PI * 0.25f);
        D3DXMatrixRotationQuaternion(&rotMat, &quat);

        actorMat._41 = 0;
        actorMat._42 = 0;
        actorMat._43 = 0;
        actorMat *= rotMat;
        actorMat._41 = tempLocation.x;
        actorMat._42 = tempLocation.y;
        actorMat._43 = tempLocation.z;

        this->selectActors.back()->SetWorldMatrix(&actorMat);

        break;
    }
    case LevelEditor::EditMode::SCALING:
    {
        // scale を 負の値にしない
        auto tempScale = this->selectActors.back()->GetWorldScale();
        if (tempScale.x + x < FLT_EPSILON) x = 0;
        if (tempScale.y + y < FLT_EPSILON) y = 0;
        if (tempScale.z + z < FLT_EPSILON) z = 0;
        this->selectActors.back()->AddWorldScale(x, y, z);

        break;
    }
    default:
        break;
    }

    this->isSaved = false;
}

D3DXVECTOR3 LevelEditor::CalcMoveVector()
{
    // y無視 視線ベクトル
    D3DXVECTOR3 tempVec = this->editorCamera->GetLookVector();
    tempVec.y = 0;
    D3DXVec3Normalize(&tempVec, &tempVec);

    if (abs(tempVec.x) <= abs(tempVec.z))
    {
        tempVec.x = 0;
        tempVec.z = std::roundf(tempVec.z);
    }
    else
    {
        tempVec.x = std::roundf(tempVec.x);
        tempVec.z = 0;
    }

    tempVec *= this->actorEditUnit;

    return tempVec;
}

void LevelEditor::ResetSelectActors(bool all)
{
    if (this->selectActors.empty()) return;

    // 全部リセット
    if (all)
    {
        this->selectActors.back()->SetWorldMatrix(nullptr);
    }
    // 編集モード別リセット
    else
    {
        switch (this->editMode)
        {
        case EditMode::LOCATION:

            this->selectActors.back()->SetWorldLocation(0, 0, 0);
            break;

        case EditMode::ROTATION:

            this->selectActors.back()->SetWorldRotation(0, 0, 0);
            break;

        case EditMode::SCALING:

            this->selectActors.back()->SetWorldScale(1, 1, 1);
            break;

        default:
            break;
        }
    }
}

void LevelEditor::DuplicateSelectActors()
{
    if (this->selectActors.empty()) return;

    if (   this->editMode != EditMode::LOCATION
        && this->editMode != EditMode::ROTATION
        && this->editMode != EditMode::SCALING  )
    {
        return;
    }

    // 領域確保
    std::vector<Actor*> duplicateActors;
    duplicateActors.reserve(this->selectActors.size());
    this->actors.reserve(this->actors.size() + this->selectActors.size());

    for (auto& selectActor : this->selectActors)
    {
        auto newActor = ActorManager::Generate(selectActor->GetActorType(), &selectActor->GetWorldMatrix(), nullptr, selectActor->id);
        duplicateActors.push_back(newActor);
        this->actors.push_back(newActor);
    }

    this->RemoveSelectAll();
    this->AddSelectActors(&duplicateActors);

    this->isSaved = false;
}

void LevelEditor::PickActor(POINT point, bool multiple)
{
    D3DLOCKED_RECT lockedRect = {};
    RECT lockRect = { point.x, point.y, point.x + 1, point.y + 1 };
    DWORD color = {};

    // クリックした場所の判定色を取得
    if(SUCCEEDED( this->objectJudgeSurface->LockRect( &lockedRect, &lockRect, D3DLOCK_READONLY)))
    {
        if (lockedRect.pBits)
        {
            color = *reinterpret_cast<DWORD*>(lockedRect.pBits);
        }

        // サーフェイスアンロック
        this->objectJudgeSurface->UnlockRect();
    }
    else return;

    Actor* selectActor = nullptr;

    // 単一選択時、選択配列をクリア
    if (!multiple) this->RemoveSelectAll();

    // 判定
    for (auto& actor : this->actors)
    {
        if (actor->GetJudgeColor() == color)
        {
            selectActor = actor;
        }
    }


    // 選択配列に追加or削除
    if (selectActor)
    {
        // 複数選択時、同じものを選択した時は選択解除
        if (multiple)
        {
            // 未選択でないとき
            if (this->selectActors.size() > 0)
            {
                // 複数選択中に 同じものを選択した
                if (std::any_of(this->selectActors.begin(), this->selectActors.end(), [&](const Actor* actor) { return actor == selectActor; }))
                {
                    // 選択解除
                    this->RemoveSelectActor(selectActor);
                }
                else
                {
                    this->AddSelectActor(selectActor);
                }
            }
            // 新規選択
            else this->AddSelectActor(selectActor);
        }
        // 単一選択
        else this->AddSelectActor(selectActor);
    }
}

void LevelEditor::PickActorRectRange(RECT rect, bool multiple)
{
    D3DLOCKED_RECT lockedRect = {};
    std::vector<DWORD> colors;

    if (rect.left - rect.right > 0)
    {
        // 左右入れ替え
        rect.left   ^= rect.right;
        rect.right  ^= rect.left;
        rect.left   ^= rect.right;
    }

    if (rect.top - rect.bottom > 0)
    {
        // 上下入れ替え
        rect.top    ^= rect.bottom;
        rect.bottom ^= rect.top;
        rect.top    ^= rect.bottom;
    }

    // 長方形内の判定色を取得
    if(SUCCEEDED( this->objectJudgeSurface->LockRect( &lockedRect, &rect, D3DLOCK_READONLY)))
    {
        if (lockedRect.pBits)
        {
            for (DWORD i = 0; i < abs(rect.top - rect.bottom); ++i)
            {
                for (DWORD j = 0; j < abs(rect.right - rect.left); ++j)
                {
                    DWORD tempColor = *(reinterpret_cast<DWORD*>((BYTE*)(lockedRect.pBits) + lockedRect.Pitch * i + 4 * j));

                    // 重複チェック
                    if (!std::any_of(colors.begin(), colors.end(), [&](const DWORD color) { return color == tempColor; }))
                    {
                        colors.push_back(tempColor);
                    }
                }
            }
        }

        // サーフェイスをアンロック
        this->objectJudgeSurface->UnlockRect();
    }
    else return;

    // 単一選択時、選択配列をクリア
    if (!multiple) this->RemoveSelectAll();

    std::vector<Actor*> selectActors;

    // 判定
    for (auto& color : colors)
    {
        for (auto& actor : this->actors)
        {
            if (actor->GetJudgeColor() == color)
            {
                selectActors.push_back(actor);
                break;
            }
        }
    }

    // 選択配列に追加
    if (!selectActors.empty())
    {
        //選択
        this->AddSelectActors(&selectActors);
    }
}

void LevelEditor::RenderPickRectRange(RECT rect)
{
    if (GetKeyState(VK_LBUTTON) >= 0) return;

    if (   this->editMode != EditMode::LOCATION
        && this->editMode != EditMode::ROTATION
        && this->editMode != EditMode::SCALING  )
    {
        return;
    }

    if (rect.top > this->sampleViewportsOrigin.y) return;

    D3DXVECTOR3 leftUpper((FLOAT)rect.left, (FLOAT)rect.top, 0.1f);
    D3DXVECTOR3 leftDowner((FLOAT)rect.left, (FLOAT)rect.bottom, 0.1f);
    D3DXVECTOR3 rightUpper((FLOAT)rect.right, (FLOAT)rect.top, 0.1f);
    D3DXVECTOR3 rightDowner((FLOAT)rect.right, (FLOAT)rect.bottom, 0.1f);

    D3DXVECTOR3 leftUpperLocation(0, 0, 0);
    D3DXVECTOR3 leftDownerLocation(0, 0, 0);
    D3DXVECTOR3 rightUpperLocation(0, 0, 0);
    D3DXVECTOR3 rightDownerLocation(0, 0, 0);

    D3DVIEWPORT9 vp;
    DX9::instance->device3d->GetViewport(&vp);
    D3DXVec3Unproject(&leftUpperLocation, &leftUpper, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);
    D3DXVec3Unproject(&leftDownerLocation, &leftDowner, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);
    D3DXVec3Unproject(&rightUpperLocation, &rightUpper, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);
    D3DXVec3Unproject(&rightDownerLocation, &rightDowner, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);

    Line3D::Render(leftUpperLocation, rightUpperLocation);

    Line3D::Render(rightUpperLocation, rightDownerLocation);

    Line3D::Render(leftDownerLocation, rightDownerLocation);

    Line3D::Render(leftDownerLocation, leftUpperLocation);
}

void LevelEditor::RenderSamples()
{
    if (this->selectedSample)
    {
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(this->selectedSample);

        auto location = this->selectedSample->GetWorldLocation();

        // 補助線  (シフト押しているときは別の補助線 (PlaceSelectedSampleSuccession関数内で))
        if(GetKeyState(VK_SHIFT) >= 0)
        {
            for (int i = 0; i < 3; ++i)
            {
                D3DXVECTOR3 v(0, 0, 0);
                ((FLOAT*)v)[i] = 500;
                Line3D::Render(location + v, location - v, { v.x, v.z, v.y });
            }
        }

        // 強調
        DebugBox::Render(&this->selectedSample->GetWorldMatrix(), { 1,1,0 });
    }

    // 射影変更
    D3DXMATRIX sampleProj;
    D3DXMatrixPerspectiveFovLH(&sampleProj, D3DX_PI * 0.25f, 1, 0.01f, 100.0f);
    DX9::instance->device3d->SetTransform(D3DTS_PROJECTION, &sampleProj);

    Viewport::Render
    (
        []()
        {
            DX9::instance->device3d->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x223a70, 1.0f, 0);
        },
        0, (DWORD)this->sampleViewportsOrigin.y, Window::instance->width, this->sampleViewportSize
    );

    DWORD i = 0;

    // ホバー判定
    auto y = this->currentMousePos.y - this->sampleViewportsOrigin.y;
    if (0 <= y && y <= (LONG)this->sampleViewportSize)
    {
        this->hoveredSampleIndex = (this->currentMousePos.x - this->sampleViewportsOrigin.x) / (LONG)this->sampleViewportSize;
    }
    else
    {
        this->hoveredSampleIndex = -1;
    }

    for (auto& sample : this->samplesMap.at(this->showSampleType))
    {
        if (!sample) continue;

        LONG currentOriginX = i * this->sampleViewportSize + this->sampleViewportsOrigin.x;
        if (currentOriginX < 0) { ++i; continue; }

        D3DCOLOR clearColor = 0x192f60;
        if (i == this->selectedSampleIndex)
        {
            clearColor = 0x165e83;
        }

        if (i == this->hoveredSampleIndex && i != this->selectedSampleIndex)
        {
            clearColor = 0x223a70;
            sample->AddWorldRotation(D3DX_PI * 0.005f, 0, 0);
        }
        else
        {
            sample->SetWorldRotation(0,0,0);
        }

        // アクターが収まるようなカメラ設定
        auto roughSphere = sample->GetWorldRoughSphere();
        D3DXVECTOR3 eye(0, 0, 0);
        D3DXVECTOR3 at(roughSphere.first);
        D3DXVECTOR3 up(0, 1, 0);

        D3DXVECTOR3 direction(0.5, 0.75, -1);
        D3DXVec3Normalize(&direction, &direction);

        eye = at + direction * roughSphere.second * 3;

        D3DXMATRIX view;
        D3DXMatrixLookAtLH(&view, &eye, &at, &up);
        DX9::instance->device3d->SetTransform(D3DTS_VIEW, &view);

        Viewport::Render
        (
            [&]()
            {
                DX9::instance->device3d->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0);

                //描画
                HLSL::GetHLSLByTag("ACTOR")->RenderProcess(sample);
            },
            (DWORD)currentOriginX, (DWORD)this->sampleViewportsOrigin.y, this->sampleViewportSize, this->sampleViewportSize, 0, 0.1f
        );
        ++i;
    }

    // カメラ戻す
    DX9::instance->device3d->SetTransform(D3DTS_VIEW, &this->editorCamera->GetViewMatrix());

    // 射影戻す
    DX9::instance->device3d->SetTransform(D3DTS_PROJECTION, &this->editorCamera->GetProjectionMatrix());
}

void LevelEditor::ChangeShowSampleType(SampleType type)
{
    this->showSampleType = type;
    this->sampleViewportsOrigin.x = 0;
    this->selectedSample = nullptr;
    this->selectedSampleIndex = -1;
    this->hoveredSampleIndex = -1;
}

void LevelEditor::SelectSample()
{
    // 選択中の場合、解除
    if (this->selectedSampleIndex == this->hoveredSampleIndex)
    {
        this->selectedSampleIndex = -1;
        this->selectedSample = nullptr;
        this->editMode = EditMode::LOCATION;
    }
    // 選択する
    else
    {
        this->selectedSampleIndex = this->hoveredSampleIndex;
        if (0 <= this->selectedSampleIndex && this->selectedSampleIndex < this->samplesMap[this->showSampleType].size())
        {
            this->selectedSample = this->samplesMap[this->showSampleType][this->selectedSampleIndex];
            this->editMode = EditMode::PLACE_ACTOR;
        }
    }
}

void LevelEditor::ScrollSamples()
{
    auto y = this->currentMousePos.y - this->sampleViewportsOrigin.y;
    if (0 <= y && y <= (LONG)this->sampleViewportSize)
    {
        static LONG preX = this->sampleViewportsOrigin.x;
        if (GetKeyState(VK_LBUTTON) < 0)
        {
            if (this->dragStartPos.y >= this->sampleViewportsOrigin.y)
            {
                this->sampleViewportsOrigin.x = preX + this->currentMousePos.x - this->dragStartPos.x;
                if (this->sampleViewportsOrigin.x > 10) this->sampleViewportsOrigin.x = 10;
            }
        }
        else
        {
            preX = this->sampleViewportsOrigin.x;
        }
    }
}

void LevelEditor::GridSnapSelectedSample()
{
    if (!this->selectedSample) return;

    D3DXVECTOR3 t;
    D3DVIEWPORT9 vp;
    D3DXVECTOR3 sampleLocation(0, 0, 0);

    // 視点の射影空間でのz値を取得
    DX9::instance->device3d->GetViewport(&vp);
    D3DXVECTOR3 at = this->editorCamera->GetLookAtLocation() + this->editorCamera->GetLookVector() * this->selectedSampleDistance;
    D3DXVec3Project(&t, &at, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);

    // マウスのスクリーン座標 と 視点の射影空間でのz値 から サンプルの空間座標を求める
    D3DXVECTOR3 mouseLocation((float)currentMousePos.x, (float)currentMousePos.y, t.z);
    D3DXVec3Unproject(&sampleLocation, &mouseLocation, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);

    sampleLocation =
    {
        round(sampleLocation.x),
        round(sampleLocation.y),
        round(sampleLocation.z)
    };
    this->selectedSample->SetWorldLocation(&sampleLocation);
}

void LevelEditor::PlaceSelectedSample()
{
    if (!this->selectedSample) return;

    this->actors.push_back(ActorManager::Generate(this->selectedSample->GetActorType(), &this->selectedSample->GetWorldMatrix(), nullptr, this->selectedSample->id));

    this->isSaved = false;
}

void LevelEditor::PlaceSelectedSampleSuccession()
{
    if (!this->selectedSample) return;

    if (this->currentMousePos.y >= this->sampleViewportsOrigin.y) return;

    auto location = this->selectedSample->GetWorldLocation();

    // 連続で配置する時に、視線ベクトルに一番近い軸を固定する

    auto look = this->editorCamera->GetLookVector();

    // xyz軸
    D3DXVECTOR3 axes[3] =
    {
        D3DXVECTOR3(1,0,0),
        D3DXVECTOR3(0,1,0),
        D3DXVECTOR3(0,0,1)
    };
    float maxAbsDot = FLT_MIN;  // 視線ベクトルと各軸との 内積の 絶対値の 最大値
    int fixedAxisIndex = 0;     // 固定する軸のインデックス
    for (int i = 0; i < 3; ++i)
    {
        auto absDot = abs(D3DXVec3Dot(&look, &axes[i]));
        if (maxAbsDot < absDot)
        {
            maxAbsDot = absDot;
            fixedAxisIndex = i;
        }
    }

    //////////////////////////////////////////////////
    // 連続配置の最初以外の座標を求める
    if (!this->location_placeInSuccession.empty())
    {
        // 固定する軸の値
        float fixedNum = ((FLOAT*)(this->location_placeInSuccession.front()))[fixedAxisIndex];

        //////////////////////////////////////////////////
        // マウス直線と固定平面との交点に配置する

        D3DXVECTOR3 mouseRayStart(0, 0, 0), mouseRayEnd(0, 0, 0);
        D3DXVECTOR3 mousePosNear((FLOAT)this->currentMousePos.x, (FLOAT)this->currentMousePos.y, 0);
        D3DXVECTOR3 mousePosFar((FLOAT)this->currentMousePos.x, (FLOAT)this->currentMousePos.y, 1);

        D3DVIEWPORT9 vp;
        DX9::instance->device3d->GetViewport(&vp);

        // マウス直線を求める
        D3DXVec3Unproject(&mouseRayStart, &mousePosNear, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);
        D3DXVec3Unproject(&mouseRayEnd, &mousePosFar, &vp, &this->editorCamera->GetProjectionMatrix(), &this->editorCamera->GetViewMatrix(), nullptr);

        ////////////////////////////////////////
        // 平面を生成する3点を決める
        D3DXVECTOR3 p1 = location, p2(0,0,0), p3(0,0,0);

        ((FLOAT*)p1)[fixedAxisIndex] = fixedNum;
        ((FLOAT*)p2)[fixedAxisIndex] = fixedNum;
        ((FLOAT*)p3)[fixedAxisIndex] = fixedNum;

        int p2_index = (fixedAxisIndex + 1) % 3;
        int p3_index = (fixedAxisIndex + 2) % 3;

        ((FLOAT*)p2)[p2_index] = 1234567890.0f;
        ((FLOAT*)p3)[p3_index] = -1234567890.0f;

        // 平面生成
        D3DXPLANE plane(0, 0, 0, 0);
        D3DXPlaneFromPoints(&plane, &p1, &p2, &p3);
        D3DXPlaneNormalize(&plane, &plane);

        // 平面とマウス直線との交点
        D3DXVECTOR3 intersection(0,0,0);
        auto b = D3DXPlaneIntersectLine(&intersection, &plane, &mouseRayStart, &mouseRayEnd);

        auto mouseDirection = mouseRayEnd - mouseRayStart;
        D3DXVec3Normalize(&mouseDirection, &mouseDirection);

        // 平行の場合は
        if (b == NULL)
        {
            return;
        }

        location = intersection;

        // グリッドに合わせる
        location =
        {
            round(location.x),
            round(location.y),
            round(location.z)
        };

    }

    //////////////////////////////////////////////////
    // グリッドを表示する

    D3DXVECTOR3 direction[2] = { {0,0,0}, {0,0,0} };

    for (int i = 0; i < 2; ++i)
    {
        ((FLOAT*)(direction[i]))[ (fixedAxisIndex + (2-i)) % 3 ] = 20;
    }

    Line3D::Render(location + direction[0], location - direction[0], { direction[0].x, direction[0].z, direction[0].y });
    Line3D::Render(location + direction[1], location - direction[1], { direction[1].x, direction[1].z, direction[1].y });

    //////////////////////////////////////////////////
    // 配置する

    if (GetKeyState(VK_LBUTTON) < 0)
    {
        // 配置する座標をセットする
        this->selectedSample->SetWorldLocation(&location);

        // 配置済みチェック
        bool b = std::any_of
        (
            this->location_placeInSuccession.begin(),
            this->location_placeInSuccession.end(),
            [this](const D3DXVECTOR3& location)
            {
                D3DXVECTOR3 diff = this->selectedSample->GetWorldLocation() - location;
                auto l = D3DXVec3LengthSq(&diff);
                return (l < FLT_EPSILON);
            }
        );

        if (b) return;

        this->location_placeInSuccession.push_back(this->selectedSample->GetWorldLocation());

        // 生成
        this->actors.push_back(ActorManager::Generate(this->selectedSample->GetActorType(), &this->selectedSample->GetWorldMatrix(), nullptr, this->selectedSample->id));

        this->isSaved = false;
    }
    else
    {
        this->location_placeInSuccession.clear();
    }
}

void LevelEditor::BeginTestPlay()
{
    return;
    this->RemoveSelectAll();

    this->selectedSample = nullptr;

    this->selectedSampleIndex = -1;
    this->hoveredSampleIndex = -1;

    this->editMode = EditMode::TEST_PLAY;
}

void LevelEditor::TickTestPlay()
{

}

void LevelEditor::EndTestPlay()
{

    for (auto& actor : this->actors)
    {
        actor->BreakParents();
    }

    this->editMode = EditMode::LOCATION;
}

void LevelEditor::Debugs()
{
    Debug::AddLine("EDIT MODE : {}", nameof::nameof_enum(this->editMode));
    Debug::AddLine("---------------------------");
    Debug::AddLine("ACTOR NUM : {}", this->actors.size());
    Debug::AddLine("SELECT ACTOR NUM : {}", this->selectActors.size());
    Debug::AddLine("---------------------------");

    if (this->selectedSample)
    {
        Debug::AddLine("SAMPLE NAME : {}", this->samplesMap[this->showSampleType][this->selectedSampleIndex]->id);
        auto location = this->selectedSample->GetWorldLocation();
        Debug::AddLine(" LOCATION : ({:.2f}, {:.2f}, {:.2f})", location.x, location.y, location.z);
    }
    else if (0 <= this->hoveredSampleIndex && this->hoveredSampleIndex < this->samplesMap[this->showSampleType].size())
    {
        Debug::AddLine("SAMPLE NAME : {}", this->samplesMap[this->showSampleType][this->hoveredSampleIndex]->id);
    }
    else if (!this->selectActors.empty())
    {
        D3DXVECTOR3 location = this->selectActors.back()->GetWorldLocation();
        Debug::AddLine("LOCATION : ({:.2f}, {:.2f}, {:.2f})", location.x, location.y, location.z);
    }

    Debug::AddLine("ACTOR EDIT UNIT {:.2f}", this->actorEditUnit);

    std::string str = "\n保存されていま";
    this->isSaved ?  str += "す" : str += "せん !";
    Debug::AddLine("{}", str.c_str());
}








void LevelEditor::EventRender()
{
    //////////////////////////////////////////////////
    // 通常描画

    for (auto& actor : this->actors)
    {
        HLSL::GetHLSLByTag("ACTOR")->RenderProcess(actor);
    }

    //HLSL::GetHLSLByTag("ACTOR")->RenderProcess(GoalFlag::instance);

    if (this->editMode != EditMode::TEST_PLAY)
    {
        //////////////////////////////////////////////////
        // クリック判定用描画

        DX9::instance->device3d->SetRenderTarget(0, this->objectJudgeSurface);                // 描画先を変更
        DX9::instance->device3d->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);   // クリア
        DX9::instance->device3d->SetPixelShader(this->shaderHandler);                         // ピクセルシェーダを登録
        for (auto& actor : this->actors)
        {
            DX9::instance->device3d->SetTransform(D3DTS_WORLD, &actor->GetWorldMatrix());
            DX9::instance->device3d->SetPixelShaderConstantF(0, actor->GetJudgeColorFloat4(), 1);   // ピクセルシェーダに通達
            actor->RenderInEditor();
        }
        DX9::instance->device3d->SetPixelShader(nullptr);                                 // ピクセルシェーダを無効化
        DX9::instance->device3d->SetRenderTarget(0, this->defaultSurface);                // 描画先を戻す

        //////////////////////////////////////////////////
        // 軸描画
        for (int i = 0; i < 3; ++i)
        {
            D3DXVECTOR3 end(0, 0, 0);
            ((FLOAT*)end)[i] = 500;
            Line3D::Render({ 0,0,0 }, end, { end.x, end.z, end.y });
        }

        //////////////////////////////////////////////////
        // グリッド
        if (this->visibleGrid)
        {
            for (int i = 0; i < 25; ++i)
            {
                D3DXVECTOR3 p(0, 0, 0), direction(0, 0, 25);
                p.x = i + 0.5f;
                Line3D::Render(p + direction, p - direction);

                p.x *= -1;
                Line3D::Render(p + direction, p - direction);

                p.z = p.x;
                p.x = 0;
                direction = { 25,0,0 };
                Line3D::Render(p + direction, p - direction);

                p.z *= -1;
                Line3D::Render(p + direction, p - direction);
            }
        }

        this->HighlightSelectActors();

        this->RenderSamples();
    }
}

void LevelEditor::EventBeginPlay()
{}

void LevelEditor::EventTick(float deltaTime)
{
    //////////////////////////////////////////////////
    // マウス座標保持
    this->preMousePos = this->currentMousePos;
    this->currentMousePos = Window::instance->GetMouseCursorPos();

    // サンプルビューポートをスクロール
    this->ScrollSamples();

    switch (this->editMode)
    {
    case LevelEditor::EditMode::LOCATION:
    case LevelEditor::EditMode::ROTATION:
    case LevelEditor::EditMode::SCALING:
        // 範囲選択表示
        this->RenderPickRectRange({ this->dragStartPos.x, this->dragStartPos.y, this->currentMousePos.x, this->currentMousePos.y });

        break;

    case LevelEditor::EditMode::PLACE_ACTOR:
        this->GridSnapSelectedSample();
        if(GetKeyState(VK_SHIFT) < 0)
            this->PlaceSelectedSampleSuccession();

        break;

    case LevelEditor::EditMode::TEST_PLAY:
        this->TickTestPlay();

        break;
    }

    this->editorCamera->Move(this->currentMousePos, this->preMousePos);

    this->Debugs();

    for (auto& list : Gui::GetSortedByZIndexList())
    {
        for (auto hlslObj : list)
        {
            // SpriteGuiとFontGui
            if (!dynamic_cast<PanelGui*>(hlslObj))
            {
                HLSL::GetHLSLByTag("GUI-DEFAULT")->RenderProcess(hlslObj);
            }
            // PanelGui
            else
            {
                HLSL::GetHLSLByTag("GUI-PANEL")->RenderProcess(hlslObj);
            }
        }
    }
}

void LevelEditor::EventKeyDown(KeyEvent e)
{
    if (this->editMode == EditMode::TEST_PLAY)
    {
        // テストプレイ終了
        if (e.keyCode == 'T') this->EndTestPlay();

        return;
    }

    switch (e.keyCode)
    {
    // テストプレイ開始
    case 'T':
        this->BeginTestPlay();
        break;

    // サンプル表示切替
    case 'C':
        this->ChangeShowSampleType(SampleType::CUBE);

        break;

    // サンプル表示切替
    case 'X':
        this->ChangeShowSampleType(SampleType::XMODEL);

        break;

    // サンプル表示切替
    case 'Z':
        this->ChangeShowSampleType(SampleType::OTHER);

        break;

    case 'W':
    {
        auto v = this->CalcMoveVector();
        this->EditSelectActors(v.x, 0, v.z);
        break;
    }
    case 'A':
    {
        // 全アクター選択
        if (e.withCtrl)
        {
            this->RemoveSelectAll();
            this->AddSelectActors(&this->actors);
        }
        else
        {
            auto v = this->CalcMoveVector();
            this->EditSelectActors(-v.z, 0, v.x);
        }

        break;
    }
    case 'S':
    {
        // セーブ
        if (e.withCtrl)
        {
            this->SaveLevelData();
        }
        else
        {
            auto v = this->CalcMoveVector();
            this->EditSelectActors(-v.x, 0, -v.z);
        }

        break;
    }
    case 'D':
    {
        if (e.withCtrl)
        {
            this->DuplicateSelectActors();
        }
        else
        {
            auto v = this->CalcMoveVector();
            this->EditSelectActors(v.z, 0, -v.x);
        }
        break;
    }
    case 'E':
        this->EditSelectActors(0,  this->actorEditUnit, 0);
        break;

    case 'Q':
        this->EditSelectActors(0, -this->actorEditUnit, 0);
        break;

    // 選択中のアクターをリセット(座標、回転、拡縮)
    case 'R':
        this->ResetSelectActors(e.withShift);
        break;

    // グリッド表示切替
    case 'G':
        this->visibleGrid = !this->visibleGrid;
        break;

    // 編集モード切替
    case VK_SPACE:
        if (!e.withShift && !e.withCtrl)
        {
            switch (this->editMode)
            {
            case EditMode::LOCATION:
                this->editMode = EditMode::ROTATION;
                break;

            case EditMode::ROTATION:
                this->editMode = EditMode::SCALING;
                break;

            case EditMode::SCALING:
                this->editMode = EditMode::LOCATION;
                break;

            }
        }
        // 逆順
        else if (e.withShift)
        {
            switch (this->editMode)
            {
            case EditMode::LOCATION:
                this->editMode = EditMode::SCALING;
                break;

            case EditMode::ROTATION:
                this->editMode = EditMode::LOCATION;
                break;

            case EditMode::SCALING:
                this->editMode = EditMode::ROTATION;
                break;
            }
        }

        break;

    // 選択中のアクター削除
    case VK_DELETE:
    case VK_BACK:
        this->DeleteSelectActor();
        break;

    // デバッグ表示切替
    case  VK_OEM_3:
        Debug::instance->visible = !Debug::instance->visible;

        if (!Debug::instance->visible)
        {
            SendToastNotice("KORO", "デバッグテキストを非表示にしました\nもう一度表示するには@キーを押します");
        }

         break;

    case VK_ESCAPE:
        PostQuitMessage(0);
        break;

    default:
        break;
    }
}

void LevelEditor::EventKeyUp(KeyEvent e)
{}

void LevelEditor::EventMouseDown(MouseEvent e)
{
    if (this->editMode == EditMode::TEST_PLAY) return;

    switch (e.button)
    {
    case MouseEvent::Button::LEFT:
        this->dragStartPos = e.position;
        break;

    case MouseEvent::Button::RIGHT:
        break;

    default:
        break;
    }
}

void LevelEditor::EventMouseUp(MouseEvent e)
{
    if (this->editMode == EditMode::TEST_PLAY) return;

    switch (e.button)
    {
    case MouseEvent::Button::LEFT:
        // クリック
        if ((abs(this->dragStartPos.x - e.position.x) <= 1) && (abs(this->dragStartPos.y - e.position.y) <= 1))
        {
            // サンプル達をクリックした
            if (e.position.y > this->sampleViewportsOrigin.y)
            {
                this->SelectSample();
            }
            // アクター設置
            else if (this->editMode == EditMode::PLACE_ACTOR)
            {
                this->PlaceSelectedSample();
            }
            // アクター選択
            else
            {
                this->PickActor(e.position, e.withCtrl);
            }
        }
        // ドラッグ
        else
        {
            if(this->editMode != EditMode::PLACE_ACTOR && ((e.position.y <= this->sampleViewportsOrigin.y)))
            {
                this->PickActorRectRange(RECT(this->dragStartPos.x, this->dragStartPos.y, e.position.x + 1, e.position.y + 1), e.withCtrl);
            }
        }

        break;

    case MouseEvent::Button::RIGHT:
        break;

    default:
        break;
    }
}

void LevelEditor::EventMouseWheelMove(int delta)
{
    switch (this->editMode)
    {
    case LevelEditor::EditMode::LOCATION:
    case LevelEditor::EditMode::ROTATION:
    case LevelEditor::EditMode::SCALING:

        if (this->actorEditUnit >= 0)
        {
            this->actorEditUnit += delta * 0.1f;
        }
        else
        {
            this->actorEditUnit = 0;
        }

        break;

    case LevelEditor::EditMode::PLACE_ACTOR:

        this->selectedSampleDistance += delta;

        break;

    case LevelEditor::EditMode::TEST_PLAY:

        break;

    default:
        break;
    }
}



#endif // _DEBUG
