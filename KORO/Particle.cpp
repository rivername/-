#include "Particle.hpp"
#include "DX9.hpp"
#include <algorithm>
#include "Game.hpp"
#include "Stage.hpp"

void Particle::ReleaseAll()
{
    for (auto& p : Particle::all_particles) p->Release();
}

Particle::Particle(size_t count, ParticleName type) : texture(nullptr), count(count), type(type), spawnLocation(D3DXVECTOR3(-1.0f, -1.0f, -1.0f)), elapsedTick(0U), released(false)
{
    Particle::all_particles.push_back(this);

    this->SetHLSL(HLSL::GetHLSLByTag("KORO"));

    // テクスチャの作成
    auto result = D3DXCreateTextureFromFileExA
    (
        DX9::instance->device3d,
        PE_NAME_WITH_FILENAME[this->type],
        PE_NAME_WITH_TEX_SIZE[this->type].x, PE_NAME_WITH_TEX_SIZE[this->type].y, // Width, Height
        0, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        0x00,
        nullptr,
        nullptr,
        &this->texture
    );

    if (FAILED(result)) return;

    // 枚数分バッファ作成
    for (UINT i = 0; i < count; i++)
    {
        // ########## 頂点バッファ作成 ##########
        UINT verts_length     = ARRAYSIZE(VERTICES_BLOCK_PARTICLE);
        UINT verts_array_size = sizeof(VertexPositionNormalTexture) * verts_length;

        VertexPositionNormalTexture vert1 = VERTICES_BLOCK_PARTICLE[0];
        VertexPositionNormalTexture vert2 = VERTICES_BLOCK_PARTICLE[1];
        VertexPositionNormalTexture vert3 = VERTICES_BLOCK_PARTICLE[2];
        VertexPositionNormalTexture vert4 = VERTICES_BLOCK_PARTICLE[3];

        if (this->type == ParticleName::PAPER_SHOWER)
        {
            float vPlus = GetRandomInt(0, 9) / 10.0f;

            vert1.u = vert1.u / 10.0f + vPlus;
            vert2.u = vert2.u / 10.0f + vPlus;
            vert3.u = vert3.u / 10.0f + vPlus;
            vert4.u = vert4.u / 10.0f + vPlus;
        }

        VertexPositionNormalTexture vertices[4] =
        {
            vert1,
            vert2,
            vert3,
            vert4
        };

        // バッファ
        IDirect3DVertexBuffer9* vertex_buffer;

        // 作成
        auto result = DX9::instance->device3d->CreateVertexBuffer
        (
            verts_array_size,
            0,
            VertexPositionNormalTexture::FVF,
            D3DPOOL_MANAGED,
            &vertex_buffer,
            nullptr
        );

        // 失敗した場合
        if (FAILED(result)) return;

        void* pVertices = nullptr;
        vertex_buffer->Lock(0, verts_array_size, (void**)&pVertices, D3DLOCK_NOSYSLOCK);
        memcpy(pVertices, vertices, verts_array_size);
        vertex_buffer->Unlock();

        this->vertex_buffers.push_back(vertex_buffer);
        // ##################################

        // ########## Set Indices ##########
        UINT inds_length     = ARRAYSIZE(INDICES_BLOCK_SKYBOX);
        UINT inds_array_size = sizeof(WORD) * inds_length;

        // バッファ
        IDirect3DIndexBuffer9* index_buffer;

        // 作成
        result = DX9::instance->device3d->CreateIndexBuffer
        (
            inds_array_size,
            D3DUSAGE_WRITEONLY,
            D3DFMT_INDEX16,
            D3DPOOL_MANAGED,
            &index_buffer,
            nullptr
        );

        // 失敗した場合
        if (FAILED(result)) return;

        void* pIndices = nullptr;
        index_buffer->Lock(0, inds_array_size, (void**)&pIndices, D3DLOCK_NOSYSLOCK);
        memcpy(pIndices, INDICES_BLOCK_SKYBOX, inds_array_size);
        index_buffer->Unlock();

        this->index_buffers.push_back(index_buffer);
        // #################################
    }
}

Particle::~Particle()
{
    this->Release();
}

void Particle::SetParam(ID3DXEffect* effect)
{
    this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");

    this->hlslParam.lightColor = SkyBoxParam::ambientLightColor;
    auto lightColor = this->hlslParam.GetLightColor();
    effect->SetVector("lightColor", &lightColor);

    effect->SetTexture("tex_Diffuse", this->texture);

    D3DXVECTOR4 mulColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    effect->SetVector("mulColor", &mulColor);

    effect->SetTechnique("Main");
}

bool Particle::Render(ID3DXEffect* effect)
{
    if (this->count <= 0) return false;

    bool shouldRelease = true;

    // カリング設定を変更：ポリゴンの両面を描画
    DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // 枚数分描画
    for (size_t i = 0; i < this->count; i++)
    {
        // Y=-20より下にの場合は描画しない
        if (this->locations[i].y < PE_NAME_WITH_KILL_Y[this->type]) continue;

        // それぞれの位置を行列に変換
        D3DXMATRIX world, location, rot_x, rot_y, rot_z, scale;
        D3DXMatrixTranslation(&location, this->locations[i].x, this->locations[i].y, this->locations[i].z);
        D3DXMatrixRotationX(&rot_x, this->rotations[i].x);
        D3DXMatrixRotationY(&rot_y, this->rotations[i].y);
        D3DXMatrixRotationZ(&rot_z, this->rotations[i].z);

        if (this->type == ParticleName::PAPER_SHOWER)
        {
            D3DXMatrixScaling(&scale, 0.5f, 0.5f, 1.0f);
        }

        world = scale * rot_x * rot_y * rot_z * location;

        effect->SetMatrix("mat_LocalWorld", &world);
        effect->CommitChanges();

        // Vertexを設定
        DX9::instance->device3d->SetStreamSource(0, this->vertex_buffers[i], 0, sizeof(VertexPositionNormalTexture));

        // Indexを設定
        DX9::instance->device3d->SetIndices(this->index_buffers[i]);

        DX9::instance->device3d->SetFVF(VertexPositionNormalTexture::FVF);

        // ポリゴン描画
        DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, ARRAYSIZE(VERTICES_BLOCK_PARTICLE), 0, ARRAYSIZE(INDICES_BLOCK_SKYBOX) / 3);

        shouldRelease = false;
    }

    // カリングを元に戻す
    DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // すべてのポリゴンが消えたら
    if (shouldRelease)
    if (!this->released)
    {
        this->Release();
    }

    return true;
}

bool Particle::Release()
{
    for (auto& v : this->vertex_buffers) SAFE_RELEASE(v);
    for (auto& i : this->index_buffers)  SAFE_RELEASE(i);

    this->vertex_buffers.clear();
    this->index_buffers.clear();

    this->locations.clear();
    this->rotations.clear();

    this->accelerationsXZ.clear();
    this->accelerationsY.clear();

    SAFE_RELEASE(this->texture);

    this->count    = 0;
    this->released = true;

    return true;
}

void Particle::SetLocationAndRotation()
{
    // 枚数分ループ
    if (this->type == ParticleName::PAPER_SHOWER)
    for (size_t i = 0; i < this->count; i++)
    {
        // 加速度を0.1f~100.0fに制限
        this->accelerationsXZ[i] = std::clamp(this->accelerationsXZ[i], 0.1f, 100.0f);

        // 360度それぞれの方向を取得
        float angle = 360.0f / this->count * i;

        // 角度から方向を計算
        D3DXVECTOR3 direction = D3DXVECTOR3(cosf(angle), 0.0f, sinf(angle));
        D3DXVec3Normalize(&direction, &direction);

        // ランダムに加速させる
        this->locations[i] += direction * static_cast<float>(GetRandomInt(1, 100)) / 2000.0f * this->accelerationsXZ[i];

        // 加速度を減速させる
        this->accelerationsXZ[i] -= 0.05f;
    }

    if (this->type == ParticleName::PAPER_SHOWER)
    for (size_t i = 0; i < this->count; i++)
    {
        float x = GetRandomInt(1, 70) * 0.0001f;
        float z = GetRandomInt(1, 70) * 0.0001f;

        // ランダムな値分回転させる
        this->rotations[i] += D3DXVECTOR3(D3DXToDegree(x), 0.0f, D3DXToDegree(z));
    }
}

void Particle::ApplyGravity(float deltaTime)
{
    if (this->type == ParticleName::PAPER_SHOWER)
    for (size_t i = 0; i < this->count; i++)
    {
        // 重力を適用する
        this->locations[i].y -= this->accelerationsY[i];

        // ランダムな値の空気抵抗
        float air = GetRandomInt(1, 20) * 0.08f;

        // 重力加速度の計算
        this->accelerationsY[i] += 9.807f * air * deltaTime * deltaTime;
    }
}

void Particle::Summon(D3DXVECTOR3 spawnLocation)
{
    this->spawnLocation = spawnLocation;
    this->ResetValue();
}

void Particle::ResetValue()
{
    // すべてクリア

    this->locations.clear();
    this->rotations.clear();

    this->accelerationsXZ.clear();
    this->accelerationsY.clear();

    if (this->type == ParticleName::PAPER_SHOWER)
    {
        for (size_t i = 0; i < count; i++)
        {
            this->locations.push_back(this->spawnLocation + D3DXVECTOR3(0.0f, 3.0f, 0.0f));
            this->rotations.push_back(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

            this->accelerationsXZ.push_back(4.0f);
            this->accelerationsY.push_back(-GetRandomInt(10, 20) / 100.0f);
        }
    }
}

void Particle::EventBeginPlay()
{
}

void Particle::EventRender()
{
    // 描画
    HLSL::GetHLSLByTag("ACTOR")->RenderProcess(this);
}

void Particle::EventTick(float deltaTime)
{
    // ポーズ中でないときは動き関係を実行
    IF_NOT_PAUSING
    {
        this->SetLocationAndRotation();
        this->ApplyGravity(deltaTime);
    }
}

void Particle::EventKeyDown(KeyEvent e)
{
}

void Particle::EventKeyUp(KeyEvent e)
{
}

void Particle::EventMouseDown(MouseEvent e)
{
}

void Particle::EventMouseUp(MouseEvent e)
{
}
