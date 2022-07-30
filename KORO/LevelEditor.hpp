#pragma once

#include <Windows.h>

// パス情報
namespace level_data
{
    const LPCSTR folder_path = "data/LevelDatas/";
    const LPCSTR file_name   = "LevelData";
    const LPCSTR extension   = "KORO";
}

#if _DEBUG

#include "DX9.hpp"
#include "Debug.hpp"
#include "Stage.hpp"

class Actor;

class EditorCamera;
class Line3D;


class LevelEditor : public GameObject
{
public:
    static inline LevelEditor* instance = nullptr;

public:
	LevelEditor(int editLevelNum);
	~LevelEditor();

private:

    void Init();

    void LoadOrCreateLevelData();

    void SaveLevelData();

private:

    // addActor を選択配列に追加
    // args :
    //  addActor 追加するアクター
    void AddSelectActor(Actor* addActor);

    // addActors を選択配列に追加
    // args :
    //  addActors 追加するアクター達
    void AddSelectActors(std::vector<Actor*>* addActors);

    // removeActor を選択配列から除く
    // args :
    //  removeActor 除くアクター
    void RemoveSelectActor(Actor* removeActor);

    // 全選択解除
    void RemoveSelectAll();

    // 選択中のアクターを削除
    void DeleteSelectActor();

    // 選択アクターを強調
    void HighlightSelectActors();

    // 選択中のアクターを編集
    void EditSelectActors(float x, float y, float z);

    // 編集の移動方向を計算
    D3DXVECTOR3 CalcMoveVector();

    // 選択中のアクターの 座標/回転/拡縮 をリセット
    // args :
    //  all 座標/回転/拡縮 のすべてをリセットするか
    void ResetSelectActors(bool all);

    // 選択中のアクターを複製し、選択
    void DuplicateSelectActors();

private:

    // point のアクターを選択する
    // args :
    //  point スクリーン座標
    //  multiple 複数選択か
    void PickActor(POINT point, bool multiple);

    // rect 範囲内のアクターを全て選択する
    // args :
    //  rect スクリーン長方形範囲
    //  multiple 複数選択か
    void PickActorRectRange(RECT rect, bool multiple);

    // 範囲選択の線を表示
    void RenderPickRectRange(RECT rect);


private:
    //////////////////////////////////////////////////
    // サンプル

    enum class SampleType
    {
        CUBE,
        XMODEL,
        OTHER
    };

    // サンプル達を画面下に並べて表示
    void RenderSamples();

    void ChangeShowSampleType(SampleType type);

    // サンプルを選択
    void SelectSample();

    void ScrollSamples();

    // 選択したサンプルをグリッドに合わせる
    void GridSnapSelectedSample();

    // 選択中サンプルを実体化
    void PlaceSelectedSample();

    // 選択中サンプルを連続で実体化
    void PlaceSelectedSampleSuccession();

private:
    //////////////////////////////////////////////////
    // テストプレイ

    void BeginTestPlay();

    void TickTestPlay();

    void EndTestPlay();

private:

    // いろいろ表示
    void Debugs();


private:
    // 編集するLevelData番号
    int editLevelNum;

    // 編集モード
    enum class EditMode
    {
        LOCATION,    // 座標編集
        ROTATION,    // 回転編集
        SCALING,     // 拡縮編集
        PLACE_ACTOR, // アクター配置中
        TEST_PLAY    // テストプレイ中
    } editMode;

private:
    // 全アクター
    std::vector<Actor*> actors;

    // 選択中のアクター
    std::vector<Actor*> selectActors;

    // 配置データ
    std::vector<ActorSaveData> levelData;

private:
    // アクター編集時の移動量
    float actorEditUnit;

private:
    std::map<SampleType, std::vector<Actor*>> samplesMap;
    SampleType showSampleType;
    DWORD sampleViewportSize;
    POINT sampleViewportsOrigin;
    int selectedSampleIndex;
    int hoveredSampleIndex;
    Actor* selectedSample;
    float selectedSampleDistance;

    // 連続配置した座標
    std::vector<D3DXVECTOR3> location_placeInSuccession;

private:
    //// テストプレイ用の親トランスフォーム
    //std::unique_ptr<Transform> testStage;

private:
    // デバッグカメラ
    std::unique_ptr<EditorCamera> editorCamera;

private:
    // マウス座標
    POINT currentMousePos, preMousePos, dragStartPos;

private:
    // グリッド表示
    bool visibleGrid;

private:
    // 保存チェック
    bool isSaved;

private:
    //////////////////////////////////////////////////
    // クリック判定描画用

    // 現在のサーフェイス
    IDirect3DSurface9* defaultSurface;
    // クリック判定描画用サーフェイス
    IDirect3DSurface9* objectJudgeSurface;

    ID3DXBuffer* shader;
    ID3DXBuffer* error;
    IDirect3DPixelShader9* shaderHandler;

private:
    // GameObject 継承
    virtual void EventRender() override;
    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;

public:
    virtual void EventMouseWheelMove(int delta) override;
};

//////////////////////////////////////////////////
// 3D線分
class Line3D
{
private:
    Line3D() : vertexBuffer(nullptr)
    {
        this->Init();
    }

public:
    ~Line3D()
    {
        SAFE_RELEASE(this->vertexBuffer);

        OutputDebugFormat("-- [ {} ] called. --\n", __func__);
    }

private:
    // 一度だけインスタンス化
    static void Instantiate()
    {
        if (!Line3D::instance)
        {
            Line3D::instance.reset(new Line3D());
        }
    }

private:
    //////////////////////////////////////////////////
    // 頂点の初期化

    // カスタム頂点
    struct CustomVertex_xyz_normal_diffuse
    {
        float x, y, z;
        float nx, ny, nz;
        float color;

        static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
    };

    // 頂点バッファ生成
    void Init()
    {
        //////////////////////////////////////////////////
        // create vertexbuffer

        // direction : {1, 0, 0}
        // length : 1
        CustomVertex_xyz_normal_diffuse vertices[2] =
        {
            {0, 0, 0, -1, 0, 0,  0xffffff},
            {1, 0, 0,  1, 0, 0,  0xffffff}
        };

        if (SUCCEEDED(DX9::instance->device3d->CreateVertexBuffer(sizeof(vertices), 0, (CustomVertex_xyz_normal_diffuse::FVF), D3DPOOL_MANAGED, &this->vertexBuffer, 0)))
        {
            void* tempVB;
            if (SUCCEEDED(this->vertexBuffer->Lock(0, sizeof(vertices), &tempVB, 0)))
            {
                memcpy(tempVB, vertices, sizeof(vertices));
                this->vertexBuffer->Unlock();

                return;
            }
        }
    };

public:
    // 3d空間に線分を描画
    // args :
    //    start 始点
    //    end   終点
    //    color 線分の色
    static void Render(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DCOLORVALUE color = { 1,1,1,1 })
    {
        //////////////////////////////////////////////////
        // instantiate only once
        Line3D::Instantiate();

        //////////////////////////////////////////////////
        // rotate line segment
        D3DXVECTOR3 direction = end - start;

        float length = D3DXVec3Length(&direction);

        D3DXVec3Normalize(&direction, &direction);

        D3DXVECTOR3 baseDirection(1,0,0);
        D3DXVECTOR3 rotationAxis(0,0,0);
        D3DXVec3Cross(&rotationAxis, &baseDirection, &direction);
        if (D3DXVec3LengthSq(&rotationAxis) <= FLT_EPSILON) rotationAxis = { 0,0,1 };

        float radian = acosf(D3DXVec3Dot(&baseDirection, &direction));

        D3DXMATRIX rotationMatrix;
        D3DXQUATERNION tempQuat(0,0,0,1);
        D3DXQuaternionRotationAxis(&tempQuat, &rotationAxis, radian);
        D3DXMatrixRotationQuaternion(&rotationMatrix, &tempQuat);

        D3DXMATRIX worldMatrix;
        D3DXMATRIX scaleMatrix;
        D3DXMatrixScaling(&scaleMatrix, length, 1, 1);
        worldMatrix = scaleMatrix * rotationMatrix;
        worldMatrix._41 = start.x;
        worldMatrix._42 = start.y;
        worldMatrix._43 = start.z;

        //////////////////////////////////////////////////
        // material
        D3DMATERIAL9 material =
        {
            .Emissive = color
        };

        //////////////////////////////////////////////////
        // render
        DX9::instance->device3d->SetTransform(D3DTS_WORLD, &worldMatrix);
        DX9::instance->device3d->SetTexture(0, nullptr);
        DX9::instance->device3d->SetStreamSource(0, Line3D::instance->vertexBuffer, 0, sizeof(CustomVertex_xyz_normal_diffuse));
        DX9::instance->device3d->SetFVF(CustomVertex_xyz_normal_diffuse::FVF);
        DX9::instance->device3d->SetMaterial(&material);
        DX9::instance->device3d->DrawPrimitive(D3DPT_LINELIST, 0, 2);
        DX9::instance->device3d->DrawPrimitive(D3DPT_LINELIST, 0, 2); // 薄いので重ねて描画
        DX9::instance->device3d->DrawPrimitive(D3DPT_LINELIST, 0, 2); // 薄いので重ねて描画
    }

private:
    static inline std::unique_ptr<Line3D> instance = nullptr;

    IDirect3DVertexBuffer9* vertexBuffer;

};

//////////////////////////////////////////////////
// DebugBox
class DebugBox
{
private:
    DebugBox() : vertexBuffer(nullptr), indexBuffer(nullptr)
    {
        Init();
    }

public:
    ~DebugBox()
    {
        SAFE_RELEASE(this->indexBuffer);
        SAFE_RELEASE(this->vertexBuffer);

        OutputDebugFormat("-- [ {} ] called. --\n", __func__);
    }

private:
    // 一度だけインスタンス化
    static void Instantiate()
    {
        if (!DebugBox::instance)
        {
            DebugBox::instance.reset(new DebugBox());
        }
    }

private:
    //////////////////////////////////////////////////
    // 頂点の初期化

    // カスタム頂点
    struct CustomVertex_xyz_normal_diffuse
    {
        float x, y, z;
        float nx, ny, nz;
        float color;

        static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
    };

    // 頂点バッファ生成
    void Init()
    {
        //////////////////////////////////////////////////
        // create vertex buffer

        CustomVertex_xyz_normal_diffuse vertices[8] =
        {
            {-0.5f,  0.5f,  0.5f, -1,  1,  1,  0xffffff},
            { 0.5f,  0.5f,  0.5f,  1,  1,  1,  0xffffff},
            { 0.5f,  0.5f, -0.5f, -1,  1, -1,  0xffffff},
            {-0.5f,  0.5f, -0.5f,  1,  1, -1,  0xffffff},
            {-0.5f, -0.5f,  0.5f, -1, -1,  1,  0xffffff},
            { 0.5f, -0.5f,  0.5f,  1, -1,  1,  0xffffff},
            { 0.5f, -0.5f, -0.5f, -1, -1, -1,  0xffffff},
            {-0.5f, -0.5f, -0.5f,  1, -1, -1,  0xffffff}
        };

        if (SUCCEEDED(DX9::instance->device3d->CreateVertexBuffer(sizeof(vertices), 0, (CustomVertex_xyz_normal_diffuse::FVF), D3DPOOL_MANAGED, &this->vertexBuffer, 0)))
        {
            void* tempVB;
            if (SUCCEEDED(this->vertexBuffer->Lock(0, sizeof(vertices), &tempVB, 0)))
            {
                memcpy(tempVB, vertices, sizeof(vertices));
                this->vertexBuffer->Unlock();
            }
        }

        //////////////////////////////////////////////////
        // create index buffer

        WORD indices[] =
        {
            0,1,  1,2,  2,3,  3,0,
            4,5,  5,6,  6,7,  7,4,
            0,4,  1,5,  2,6,  3,7
        };

        if (SUCCEEDED(DX9::instance->device3d->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &this->indexBuffer, 0)))
        {
            void* tempIB;
            if (SUCCEEDED(this->indexBuffer->Lock(0, sizeof(indices), &tempIB, 0)))
            {
                memcpy(tempIB, indices, sizeof(indices));
                this->indexBuffer->Unlock();
            }
        }
    };

public:
    static void Render(const D3DXMATRIX* world, D3DCOLORVALUE color = { 1,1,1,1 })
    {
        //////////////////////////////////////////////////
        // instantiate only once
        DebugBox::Instantiate();

        //////////////////////////////////////////////////
        // material

        D3DMATERIAL9 material =
        {
            .Emissive = color
        };

        //////////////////////////////////////////////////
        // render

        if (world)
            DX9::instance->device3d->SetTransform(D3DTS_WORLD, world);
        else
        {
            D3DXMATRIX identity;
            D3DXMatrixIdentity(&identity);
            DX9::instance->device3d->SetTransform(D3DTS_WORLD, &identity);
        }

        DX9::instance->device3d->SetTexture(0, nullptr);
        DX9::instance->device3d->SetStreamSource(0, DebugBox::instance->vertexBuffer, 0, sizeof(CustomVertex_xyz_normal_diffuse));
        DX9::instance->device3d->SetFVF(CustomVertex_xyz_normal_diffuse::FVF);
        DX9::instance->device3d->SetIndices(DebugBox::instance->indexBuffer);
        DX9::instance->device3d->SetMaterial(&material);
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // 線分なので第4引数は0でok
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // 薄いので重ねて描画
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // 薄いので重ねて描画
    }

private:
    static inline std::unique_ptr<DebugBox> instance = nullptr;

    IDirect3DVertexBuffer9* vertexBuffer;
    IDirect3DIndexBuffer9* indexBuffer;

};


//////////////////////////////////////////////////
// UE4のようなカメラ
class EditorCamera
{
public:
    EditorCamera() : eye(), at(), up(), view(), proj(), moveSpeed(0.2f)
    {
        this->Reset();
    }

    ~EditorCamera()
    {
        OutputDebugFormat("-- [ {} ] called. --\n", __func__);
    }

public:
    const D3DXMATRIX& GetViewMatrix() const
    {
        return this->view;
    };

    const D3DXMATRIX& GetProjectionMatrix() const
    {
        return this->proj;
    };

    const D3DXVECTOR3& GetLookAtLocation() const
    {
        return this->at;
    }

    const D3DXVECTOR3& GetEyeLocation() const
    {
        return this->eye;
    }

    // 視線ベクトルを取得 (normalized)
    D3DXVECTOR3 GetLookVector() const
    {
        return D3DXVECTOR3(this->view._13, this->view._23, this->view._33);
    }

public:
    // eye, at, upをリセット
    void Reset()
    {
        this->eye = D3DXVECTOR3(0.0f, 10.0f, -10.0f);
        this->at  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        this->up  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

        D3DXMatrixLookAtLH(&this->view, &this->eye, &this->at, &this->up);
        DX9::instance->device3d->SetTransform(D3DTS_VIEW, &view);

        D3DXMatrixPerspectiveFovLH(&this->proj, D3DX_PI * 0.25f, (static_cast<float>(Window::instance->width) / Window::instance->height), 0.1f, 1000.0f);
        DX9::instance->device3d->SetTransform(D3DTS_PROJECTION, &proj);
    }

    // UE4のような移動
    void Move(POINT currentMousePos, POINT preMousePos)
    {
        // 右クリック時以外は移動しない
        if (GetKeyState(VK_RBUTTON) >= 0) return;

        // マウスの前フレームからの移動方向(長さあり)
        D3DXVECTOR3 mouseDirection =
        {
            static_cast<float>(currentMousePos.x - preMousePos.x),
            -static_cast<float>(currentMousePos.y - preMousePos.y),
            0.0f
        };

        // 視線ベクトル(正規化)
        D3DXVECTOR3 lookVec = this->GetLookVector();

        // 視点移動
        this->at += D3DXVECTOR3(lookVec.z * mouseDirection.x, mouseDirection.y, -lookVec.x * mouseDirection.x) * 0.1f;

        if (GetKeyState('W') < 0) this->MoveBody(0, 0,  this->moveSpeed, &lookVec);
        if (GetKeyState('S') < 0) this->MoveBody(0, 0, -this->moveSpeed, &lookVec);
        if (GetKeyState('D') < 0) this->MoveBody( this->moveSpeed, 0, 0, &lookVec);
        if (GetKeyState('A') < 0) this->MoveBody(-this->moveSpeed, 0, 0, &lookVec);
        if (GetKeyState('E') < 0) this->MoveBody(0,  this->moveSpeed, 0, &lookVec);
        if (GetKeyState('Q') < 0) this->MoveBody(0, -this->moveSpeed, 0, &lookVec);

        D3DXMatrixLookAtLH(&this->view, &this->eye, &this->at, &this->up);
        DX9::instance->device3d->SetTransform(D3DTS_VIEW, &view);
    }

private:
    // eye,atを移動
    void MoveBody(float right, float up, float forward, D3DXVECTOR3* lookVec)
    {
        D3DXVECTOR3 moveRightVec(lookVec->z * right, 0, -lookVec->x * right);
        D3DXVECTOR3 moveUpVec(0, up, 0);

        D3DXVECTOR3 moveVec = forward * *lookVec + moveRightVec + moveUpVec;
        this->eye += moveVec;
        this->at += moveVec;
    }

private:
    D3DXVECTOR3 eye, at, up;
    D3DXMATRIX view, proj;

    // ファーストパーソン移動速度
    float moveSpeed;
};

#else // Releaseビルド時

//////////////////////////////////////////////////
// ダミー

#include "GameObject.hpp"

class LevelEditor : public GameObject
{
public:
    static inline LevelEditor* instance = nullptr;

public:
    LevelEditor()
    {}

    LevelEditor(int n)
    {}

    ~LevelEditor()
    {
        OutputDebugFormat("ダミーレベルエディターが終了しました。\n");
    }

private:

    // GameObject を介して継承されました
    virtual void EventBeginPlay() override
    {
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        OutputDebugString("LevelEditor は デバッグビルドで実行してください。\n");
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        MessageBox(NULL, TEXT("LevelEditor は デバッグビルドで実行してください。"), TEXT("LevelEditor"), MB_OK | MB_ICONWARNING);
        PostQuitMessage(-1);
    };

    virtual void EventTick(float deltaTime) override {};

    virtual void EventKeyDown(KeyEvent e) override {};

    virtual void EventKeyUp(KeyEvent e) override {};

    virtual void EventMouseDown(MouseEvent e) override {};

    virtual void EventMouseUp(MouseEvent e) override {};

};

#endif // _DEBUG