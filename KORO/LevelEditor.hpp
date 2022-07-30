#pragma once

#include <Windows.h>

// �p�X���
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

    // addActor ��I��z��ɒǉ�
    // args :
    //  addActor �ǉ�����A�N�^�[
    void AddSelectActor(Actor* addActor);

    // addActors ��I��z��ɒǉ�
    // args :
    //  addActors �ǉ�����A�N�^�[�B
    void AddSelectActors(std::vector<Actor*>* addActors);

    // removeActor ��I��z�񂩂珜��
    // args :
    //  removeActor �����A�N�^�[
    void RemoveSelectActor(Actor* removeActor);

    // �S�I������
    void RemoveSelectAll();

    // �I�𒆂̃A�N�^�[���폜
    void DeleteSelectActor();

    // �I���A�N�^�[������
    void HighlightSelectActors();

    // �I�𒆂̃A�N�^�[��ҏW
    void EditSelectActors(float x, float y, float z);

    // �ҏW�̈ړ��������v�Z
    D3DXVECTOR3 CalcMoveVector();

    // �I�𒆂̃A�N�^�[�� ���W/��]/�g�k �����Z�b�g
    // args :
    //  all ���W/��]/�g�k �̂��ׂĂ����Z�b�g���邩
    void ResetSelectActors(bool all);

    // �I�𒆂̃A�N�^�[�𕡐����A�I��
    void DuplicateSelectActors();

private:

    // point �̃A�N�^�[��I������
    // args :
    //  point �X�N���[�����W
    //  multiple �����I����
    void PickActor(POINT point, bool multiple);

    // rect �͈͓��̃A�N�^�[��S�đI������
    // args :
    //  rect �X�N���[�������`�͈�
    //  multiple �����I����
    void PickActorRectRange(RECT rect, bool multiple);

    // �͈͑I���̐���\��
    void RenderPickRectRange(RECT rect);


private:
    //////////////////////////////////////////////////
    // �T���v��

    enum class SampleType
    {
        CUBE,
        XMODEL,
        OTHER
    };

    // �T���v���B����ʉ��ɕ��ׂĕ\��
    void RenderSamples();

    void ChangeShowSampleType(SampleType type);

    // �T���v����I��
    void SelectSample();

    void ScrollSamples();

    // �I�������T���v�����O���b�h�ɍ��킹��
    void GridSnapSelectedSample();

    // �I�𒆃T���v�������̉�
    void PlaceSelectedSample();

    // �I�𒆃T���v����A���Ŏ��̉�
    void PlaceSelectedSampleSuccession();

private:
    //////////////////////////////////////////////////
    // �e�X�g�v���C

    void BeginTestPlay();

    void TickTestPlay();

    void EndTestPlay();

private:

    // ���낢��\��
    void Debugs();


private:
    // �ҏW����LevelData�ԍ�
    int editLevelNum;

    // �ҏW���[�h
    enum class EditMode
    {
        LOCATION,    // ���W�ҏW
        ROTATION,    // ��]�ҏW
        SCALING,     // �g�k�ҏW
        PLACE_ACTOR, // �A�N�^�[�z�u��
        TEST_PLAY    // �e�X�g�v���C��
    } editMode;

private:
    // �S�A�N�^�[
    std::vector<Actor*> actors;

    // �I�𒆂̃A�N�^�[
    std::vector<Actor*> selectActors;

    // �z�u�f�[�^
    std::vector<ActorSaveData> levelData;

private:
    // �A�N�^�[�ҏW���̈ړ���
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

    // �A���z�u�������W
    std::vector<D3DXVECTOR3> location_placeInSuccession;

private:
    //// �e�X�g�v���C�p�̐e�g�����X�t�H�[��
    //std::unique_ptr<Transform> testStage;

private:
    // �f�o�b�O�J����
    std::unique_ptr<EditorCamera> editorCamera;

private:
    // �}�E�X���W
    POINT currentMousePos, preMousePos, dragStartPos;

private:
    // �O���b�h�\��
    bool visibleGrid;

private:
    // �ۑ��`�F�b�N
    bool isSaved;

private:
    //////////////////////////////////////////////////
    // �N���b�N����`��p

    // ���݂̃T�[�t�F�C�X
    IDirect3DSurface9* defaultSurface;
    // �N���b�N����`��p�T�[�t�F�C�X
    IDirect3DSurface9* objectJudgeSurface;

    ID3DXBuffer* shader;
    ID3DXBuffer* error;
    IDirect3DPixelShader9* shaderHandler;

private:
    // GameObject �p��
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
// 3D����
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
    // ��x�����C���X�^���X��
    static void Instantiate()
    {
        if (!Line3D::instance)
        {
            Line3D::instance.reset(new Line3D());
        }
    }

private:
    //////////////////////////////////////////////////
    // ���_�̏�����

    // �J�X�^�����_
    struct CustomVertex_xyz_normal_diffuse
    {
        float x, y, z;
        float nx, ny, nz;
        float color;

        static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
    };

    // ���_�o�b�t�@����
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
    // 3d��Ԃɐ�����`��
    // args :
    //    start �n�_
    //    end   �I�_
    //    color �����̐F
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
        DX9::instance->device3d->DrawPrimitive(D3DPT_LINELIST, 0, 2); // �����̂ŏd�˂ĕ`��
        DX9::instance->device3d->DrawPrimitive(D3DPT_LINELIST, 0, 2); // �����̂ŏd�˂ĕ`��
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
    // ��x�����C���X�^���X��
    static void Instantiate()
    {
        if (!DebugBox::instance)
        {
            DebugBox::instance.reset(new DebugBox());
        }
    }

private:
    //////////////////////////////////////////////////
    // ���_�̏�����

    // �J�X�^�����_
    struct CustomVertex_xyz_normal_diffuse
    {
        float x, y, z;
        float nx, ny, nz;
        float color;

        static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
    };

    // ���_�o�b�t�@����
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
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // �����Ȃ̂ő�4������0��ok
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // �����̂ŏd�˂ĕ`��
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 0, 0, 12); // �����̂ŏd�˂ĕ`��
    }

private:
    static inline std::unique_ptr<DebugBox> instance = nullptr;

    IDirect3DVertexBuffer9* vertexBuffer;
    IDirect3DIndexBuffer9* indexBuffer;

};


//////////////////////////////////////////////////
// UE4�̂悤�ȃJ����
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

    // �����x�N�g�����擾 (normalized)
    D3DXVECTOR3 GetLookVector() const
    {
        return D3DXVECTOR3(this->view._13, this->view._23, this->view._33);
    }

public:
    // eye, at, up�����Z�b�g
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

    // UE4�̂悤�Ȉړ�
    void Move(POINT currentMousePos, POINT preMousePos)
    {
        // �E�N���b�N���ȊO�͈ړ����Ȃ�
        if (GetKeyState(VK_RBUTTON) >= 0) return;

        // �}�E�X�̑O�t���[������̈ړ�����(��������)
        D3DXVECTOR3 mouseDirection =
        {
            static_cast<float>(currentMousePos.x - preMousePos.x),
            -static_cast<float>(currentMousePos.y - preMousePos.y),
            0.0f
        };

        // �����x�N�g��(���K��)
        D3DXVECTOR3 lookVec = this->GetLookVector();

        // ���_�ړ�
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
    // eye,at���ړ�
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

    // �t�@�[�X�g�p�[�\���ړ����x
    float moveSpeed;
};

#else // Release�r���h��

//////////////////////////////////////////////////
// �_�~�[

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
        OutputDebugFormat("�_�~�[���x���G�f�B�^�[���I�����܂����B\n");
    }

private:

    // GameObject ����Čp������܂���
    virtual void EventBeginPlay() override
    {
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        OutputDebugString("LevelEditor �� �f�o�b�O�r���h�Ŏ��s���Ă��������B\n");
        OutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        MessageBox(NULL, TEXT("LevelEditor �� �f�o�b�O�r���h�Ŏ��s���Ă��������B"), TEXT("LevelEditor"), MB_OK | MB_ICONWARNING);
        PostQuitMessage(-1);
    };

    virtual void EventTick(float deltaTime) override {};

    virtual void EventKeyDown(KeyEvent e) override {};

    virtual void EventKeyUp(KeyEvent e) override {};

    virtual void EventMouseDown(MouseEvent e) override {};

    virtual void EventMouseUp(MouseEvent e) override {};

};

#endif // _DEBUG