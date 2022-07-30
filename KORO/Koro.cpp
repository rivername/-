#include "Koro.hpp"
#include "Game.hpp"
#include "Stage.hpp"
#include "GoalPoint.hpp"
#include "resource.h"

Koro::Koro() : GameObject(ObjectType::KORO)
{
    Koro::instance = this;                    // ポインタ
                                              
    this->SetWorldLocation(0.0f, 5.0f, 0.0f);
    this->radius = 0.48f;                     // 半径
                                              
    this->speed = D3DXVECTOR3(0, 0, 0);       // 速度
    this->decay = 0.99f;                      // 減衰(空気抵抗等)
    this->resistance = 0.98f;				  // 反発

    this->enableMove = true;

    materials = nullptr;
    numMaterials = 0;
    mesh = nullptr;

    this->SetHLSL(HLSL::GetHLSLByTag("KORO"));
}

Koro::~Koro()
{
    SAFE_RELEASE(this->materials);
    SAFE_RELEASE(this->mesh);
}

bool Koro::Init()
{
    HRESULT hr = D3DXLoadMeshFromX
    (
        "./resources/x/koro_kari.x",
        D3DXMESH_MANAGED,
        DX9::instance->device3d,
        NULL,
        &this->materials,
        NULL,
        &this->numMaterials,
        &this->mesh
    );

    if (FAILED(hr))
    {
        OutputDebugString("Koro::Init() Error\n");
        return false;
    }

    this->BecomeParents(Stage::instance);

    // テクスチャ設定
    this->hlslParam.CreateTexture_Diffuse("resources/textures/house.png", 0, 0);

    return true;
}

void Koro::SetParam(ID3DXEffect* effect)
{
    // 基本行列設定
    this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");

    // 行列設定
    {
        // ワールド行列から回転を抜いた行列の作成

        auto location = this->GetWorldLocation();
        auto scale = this->GetWorldScale();

        D3DXMATRIX mat_Location;
        D3DXMATRIX mat_Scale;

        D3DXMatrixTranslation(&mat_Location, location.x, location.y, location.z);
        D3DXMatrixScaling(&mat_Scale, scale.x, scale.y, scale.z);

        auto localWorldNoRotation = mat_Scale * mat_Location;

        // そのままのワールド行列を設定
        effect->SetMatrix("mat_LocalWorld", &this->worldMatrix);

        // 回転無しワールド行列を設定
        effect->SetMatrix("mat_LocalWorld_NoRotation", &localWorldNoRotation);
    }

    // ライト色
    this->hlslParam.lightColor = SkyBoxParam::ambientLightColor;
    auto color = this->hlslParam.GetLightColor();
    effect->SetVector("lightColor", &color);

    // テクスチャ
    effect->SetTexture("tex_Diffuse", this->hlslParam.tex_Diffuse);

    // Mainテクニックを使用
    effect->SetTechnique("Main");
}

bool Koro::Render(ID3DXEffect* effect)
{
    this->Render();
    return true;
}

void Koro::CollisionCube()
{
    Actor* actor = nullptr;
    D3DXVECTOR3 normal;
    float length = 0.0f;

    // ステージ番号
    byte stageNum = 0;

    if (Game::instance->gameStatus == GameStatus::IN_TITLE)
    {
        stageNum = Game::instance->selectedLevel;
    }
    else
    {
        stageNum = Game::instance->saveData.level;
    }

    for (auto& act : Stage::instance->actors[stageNum])
    {
        auto [nml, len] = act->CollisionWithKoro(this->GetWorldLocation(), this->radius);

        if (len > length)
        {
            actor = act;
            normal = nml;
            length = len;
        }
    }
        
    if (length != 0.0f)
    {
        // cube と xmodel の 当たり判定
        if (dynamic_cast<Cube*>(actor) || dynamic_cast<XModel*>(actor))
        {

            // 進行ベクトルの逆を取得
            D3DXVECTOR3 revVec = this->speed * -1;
            // 進行ベクトルをそのまま取得
            //D3DXVECTOR3 revVec = this->speed;

            // 内積を取得
            float dot = D3DXVec3Dot(&normal, &revVec);

            //if (dot > 0) {
            //	dot = fabsf(dot);
            //}
            //if (dot < -1.0) {
            //	// 強い反射
            //}

            // 反射ベクトルを取得
            D3DXVECTOR3 refVec = (this->speed * (1.0f - actor->friction)) + (normal * dot * (this->resistance + actor->resistance));
            this->SetSpeed(refVec);

            //this->SetSpeed(this->speed * (1.0 - cube->GetFriction()));
            //D3DXVECTOR3 refVec = normal * 2.0 * len * cube->GetResistance();
            //this->AddSpeed(refVec);

            // 埋まり防止応急措置
            normal.y = std::clamp(normal.y, 0.0f, 1.0f);

            // 埋まり防止
            normal = normal * length;	// *cube->GetResistance()* cube->GetResistance();
            this->AddWorldLocation(&normal);
        }
        // ゴールとの当たり判定
        else if (dynamic_cast<GoalPoint*>(actor) && Game::instance->gameStatus == GameStatus::PLAYING)
        {
            D3DXVECTOR3 goalLocation = actor->GetLocalLocation();
            D3DXVECTOR3 offset(0, 1.1f, 0);
            this->SetLocalLocation(&goalLocation);
            this->AddLocalLocation(&offset);

            // ごーーる
            this->HitGoal();
            return;
        }
        // hoge との当たり判定
        //else if (dynamic_cast<Hoge*>(actor))
        //{
        //
        //}
    }
}

void Koro::HitGoal()
{
    Game::instance->CustomEvent__StageCleared();
    this->SetSpeed(Stage::instance->GetUpVector() * Window::instance->deltaTime * 10.0f);
}

bool Koro::Render()
{
    DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);
    DX9::instance->device3d->SetTexture(0, Texture::instance->GetTexture("resources/textures/koro-texture.png"));

    for (DWORD i = 0; i < this->numMaterials; i++)
    {
        D3DXMATERIAL mat = ((D3DXMATERIAL*)(this->materials->GetBufferPointer()))[i];
        DX9::instance->device3d->SetMaterial(&(mat.MatD3D));
        this->mesh->DrawSubset(i);
    }

    return true;
}

void Koro::EventTick(float deltaTime)
{
    IF_NOT_PAUSING
    {
        if (this->enableMove)
        {
            // 重力加速処理
            if (Game::instance->gameStatus == GameStatus::CLEARED)
            {
                this->AddSpeed(Stage::instance->GetUpVector() * -Gravity::instance->GetGravityVelocity());
            }
            else
            {
                this->AddSpeed(Gravity::instance->GetGravityVector());
            }

            this->speed *= this->decay;

            // ステージ端ループ処理(仮)
            //this->EdgeLoop();

            // 移動処理
            this->AddWorldLocation(&this->speed);

            // 回転
            if (Game::instance->gameStatus != GameStatus::CLEARED)
            {
                if (this->speed.x != 0 || this->speed.z != 0) {
                    this->WorldRotateAroundAxis(this->speed.z, 0, -this->speed.x, (1.0f * (D3DXVec3Length(&this->speed) / 60)) / (this->radius / 60));
                }
            }

            // キューブとの当たり判定
            this->CollisionCube();
        }

        // 落下判定
        if (this->GetLocalLocation().y < -10)
        {
            // ステージの回転が戻り終わったら初期位置に移動
            if (Stage::instance->InterpToInitialRotation())
            {
                this->ResetLocation();
            }
        }
    }

    if (Game::instance->gameStatus == GameStatus::CLEARED ||
        Game::instance->gameStatus == GameStatus::ALL_CLEARED)
    {
        if (GetTickCount64() % 60 == 10)
        {
            
        }

        if (this->GetWorldLocation().y <= GoalFlag::instance->GetWorldLocation().y - 1.0f)
        {
            this->SetSpeed(Stage::instance->GetUpVector() * Window::instance->deltaTime * 12.0f);
        }
    }
}


void Koro::ResetLocation()
{
    // ステージの初期位置に移動

    auto location = Stage::instance->GetStartPoint() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    this->SetWorldLocation(&location);
    this->SetSpeed({ 0.0f, -0.1f, 0.0f });
}

void Koro::EventRender()
{
}

void Koro::EventKeyDown(KeyEvent e)
{
}

void Koro::EventKeyUp(KeyEvent e)
{
}

void Koro::EventMouseDown(MouseEvent e)
{
}

void Koro::EventMouseUp(MouseEvent e)
{
}

void Koro::EventBeginPlay()
{
}
