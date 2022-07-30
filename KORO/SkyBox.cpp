#include "SkyBox.hpp"
#include "DX9.hpp"
#include "Camera.hpp"
#include "MathLib.hpp"
#include "resource.h"

SkyBox::SkyBox(int resourceID) :
    offset      ({ 0.0f, 0.0f, 0.0f }),
    rotation    ({ 0.0f, 0.0f, 0.0f }),
    sizeAddValue(0.0f),

    resourceID(resourceID),
    vertex_buffer(nullptr),
    index_buffer (nullptr)
{
    this->SetHLSL(HLSL::GetHLSLByTag("SKYBOX"));

    if (this->Init())
    {
        OutputDebugFormat("[ SkyBox ] {} : 初期化に成功しました\n", this->resourceID);
    }
    else
    {
        OutputDebugFormat("[ SkyBox ] {} : 初期化に失敗しました\n", this->resourceID);
    }
}

SkyBox::~SkyBox()
{
    SAFE_RELEASE(this->vertex_buffer);
    SAFE_RELEASE(this->index_buffer);
}

bool SkyBox::Init()
{
    this->hlslParam.CreateTexture_Diffuse(this->resourceID, 2048U, 2048U);

    // ########## Create vertices ##########
    UINT verts_length     = ARRAYSIZE(VERTICES_BLOCK_SKYBOX);
    UINT verts_array_size = sizeof(VertexPositionNormalTexture) * verts_length;

    // 作成
    auto result = DX9::instance->device3d->CreateVertexBuffer
    (
        verts_array_size,
        0,
        VertexPositionNormalTexture::FVF,
        D3DPOOL_MANAGED,
        &this->vertex_buffer,
        nullptr
    );

    // 失敗した場合
    if (FAILED(result)) return false;

    void* pVertices = nullptr;
    this->vertex_buffer->Lock(0, verts_array_size, (void**)&pVertices, D3DLOCK_NOSYSLOCK);
    memcpy(pVertices, VERTICES_BLOCK_SKYBOX, verts_array_size);
    this->vertex_buffer->Unlock();
    // ##################################

    // ########## Create Indices ##########
    std::vector<WORD> indices;

    for (WORD i = 0; i < 6; i++)
    {
        indices.push_back(INDICES_BLOCK_SKYBOX[0] + (4 * i));
        indices.push_back(INDICES_BLOCK_SKYBOX[1] + (4 * i));
        indices.push_back(INDICES_BLOCK_SKYBOX[2] + (4 * i));
        indices.push_back(INDICES_BLOCK_SKYBOX[3] + (4 * i));
        indices.push_back(INDICES_BLOCK_SKYBOX[4] + (4 * i));
        indices.push_back(INDICES_BLOCK_SKYBOX[5] + (4 * i));
    }

    indices.shrink_to_fit();

    // ########## Set Indices ##########
    UINT inds_length     = (UINT)indices.size();
    UINT inds_array_size = sizeof(WORD) * inds_length;

    // 作成
    result = DX9::instance->device3d->CreateIndexBuffer
    (
        inds_array_size,
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &this->index_buffer,
        nullptr
    );

    // 失敗した場合
    if (FAILED(result)) return false;

    void* pIndices = nullptr;
    this->index_buffer->Lock(0, inds_array_size, (void**)&pIndices, D3DLOCK_NOSYSLOCK);
    memcpy(pIndices, indices.data(), inds_array_size);
    this->index_buffer->Unlock();
    // #################################

    return true;
}

void SkyBox::SetParam(ID3DXEffect* effect)
{
    // 基本行列の値を設定
    this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");

    // カメラの目線位置
    D3DXVECTOR3 camera = Camera::instance->GetCameraLocation() + this->offset;

    // XYZのスケール
    float scaleXYZ = SKYBOX_SIZE + this->sizeAddValue;

    // ワールド行列の位置を目の位置にする
    D3DXMATRIX mat_LocalWorld, mat_location, mat_rotation, mat_scale;
    D3DXMatrixTranslation(&mat_location, camera.x, camera.y, camera.z);
    D3DXMatrixRotationYawPitchRoll(&mat_rotation, D3DXToRadian(this->rotation.yaw), D3DXToRadian(this->rotation.pitch), D3DXToRadian(this->rotation.roll));
    D3DXMatrixScaling(&mat_scale, scaleXYZ, scaleXYZ, scaleXYZ);

    // ローカルワールド行列を設定
    mat_LocalWorld = mat_scale * mat_rotation * mat_location;
    effect->SetMatrix("mat_LocalWorld", &mat_LocalWorld);

    // テクスチャ
    effect->SetTexture("tex_Diffuse", this->hlslParam.tex_Diffuse);

    // 乗算色を設定
    auto mulColor = this->hlslParam.GetMulColor();
    effect->SetVector("mulColor", &mulColor);

    // Mainテクニックを使用
    effect->SetTechnique("Main");
}

bool SkyBox::Render(ID3DXEffect* effect)
{
    // Vertexを設定
    DX9::instance->device3d->SetStreamSource(0, this->vertex_buffer, 0, sizeof(VertexPositionNormalTexture));

    // Indexを設定
    DX9::instance->device3d->SetIndices(this->index_buffer);

    // FVFを設定
    DX9::instance->device3d->SetFVF(VertexPositionNormalTexture::FVF);

    // ポリゴン描画
    DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, ARRAYSIZE(VERTICES_BLOCK_SKYBOX), 0, 12);

    return true;
}

void SkyBox::EventBeginPlay()
{
}

void SkyBox::EventTick(float deltaTime)
{
}

void SkyBox::EventKeyDown(KeyEvent e)
{
}

void SkyBox::EventKeyUp(KeyEvent e)
{
}

void SkyBox::EventMouseDown(MouseEvent e)
{
}

void SkyBox::EventMouseUp(MouseEvent e)
{
}
