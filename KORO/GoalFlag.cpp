#include "GoalFlag.hpp"
#include "Texture.hpp"
#include "DayNightCycleSkyBoxParam.hpp"

GoalFlag* GoalFlag::instance;

GoalFlag::GoalFlag(D3DXVECTOR3 location) : Transform(nullptr, &location, nullptr, nullptr)
{
    GoalFlag::instance = this;

    this->vertexBuffer = nullptr;
    this->indexBuffer  = nullptr;

    this->material =
    {
        .Diffuse  = {1,1,1,1},
        .Ambient  = {1,1,1,1}
    };

    this->Init();

    // HLSLシェーダークラスセット
    this->SetHLSL(HLSL::GetHLSLByTag("ACTOR"));
}

bool GoalFlag::Init()
{
    deg = 0;

    GoalFlagVertex karivertices[goalFlag::NUM_VERTICES] =
    {
        // x      y      z

        // flag1
        { { 0.0f,  2.5f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.2f,  2.4f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.1f} },
        { { 0.0f,  1.5f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.2f,  1.6f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.9f} },

        // flag2
        { { 0.2f,  2.4f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.1f} },
        { { 0.4f,  2.3f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.4f, 0.2f} },
        { { 0.2f,  1.6f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.9f} },
        { { 0.4f,  1.7f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.4f, 0.8f} },

        // flag3
        { { 0.4f,  2.3f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.4f, 0.2f} },
        { { 0.6f,  2.2f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.6f, 0.3f} },
        { { 0.4f,  1.7f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.4f, 0.8f} },
        { { 0.6f,  1.8f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.6f, 0.7f} },

        // flag4
        { { 0.6f,  2.2f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.6f, 0.3f} },
        { { 0.8f,  2.1f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.4f} },
        { { 0.6f,  1.8f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.6f, 0.7f} },
        { { 0.8f,  1.9f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.6f} },

        // flag5
        { { 0.8f,  2.1f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.4f} },
        { { 1.0f,  2.0f, 0.01f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.5f} },
        { { 0.8f,  1.9f, 0.01f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.6f} },
        { { 1.0f,  2.0f, 0.01f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.5f} },

        // poal1
        { { 0.00f,  2.5f, -0.05f}, {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.05f,  2.5f,  0.00f}, {1.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { { 0.00f, -0.5f, -0.05f}, {1.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.05f, -0.5f,  0.00f}, {1.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },

        // poal2
        { { 0.05f,  2.5f, 0.00f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f, 0.05f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { { 0.05f, -0.5f, 0.00f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { { 0.00f, -0.5f, 0.05f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },

        // poal3
        { { 0.00f,  2.5f, 0.05f}, {-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
        { {-0.05f,  2.5f, 0.00f}, {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { { 0.00f, -0.5f, 0.05f}, {-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { {-0.05f, -0.5f, 0.00f}, {-1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },

        // poal4
        { {-0.05f,  2.5f,  0.00f}, {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f, -0.05f}, {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { {-0.05f, -0.5f,  0.00f}, {-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.00f, -0.5f, -0.05f}, {-1.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },

        // top
        { {-0.05f,  2.5f,  0.00f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f,  0.05f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.00f,  2.5f, -0.05f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.05f,  2.5f,  0.00f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },

        // bottom
        { {-0.05f, -0.5f,  0.00f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.00f, -0.5f, -0.05f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.00f, -0.5f,  0.05f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.05f, -0.5f,  0.00f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} }
    };

    memcpy(this->vertices, karivertices, sizeof(karivertices));

    if (FAILED(DX9::instance->device3d->CreateVertexBuffer(sizeof(vertices), 0, GoalFlagVertex::FVF,
        D3DPOOL_MANAGED, &this->vertexBuffer, 0)))
    {
        return false;
    }
    void* tmpVertexBuff;
    if (SUCCEEDED(this->vertexBuffer->Lock(0, sizeof(vertices), &tmpVertexBuff, 0)))
    {
        memcpy(tmpVertexBuff, this->vertices, sizeof(vertices));
        this->vertexBuffer->Unlock();
    }
    else return false;

    WORD indices[goalFlag::NUM_INDICES] =
    {
        0,  1,  2,
        2,  1,  3
    };

    if (FAILED(DX9::instance->device3d->CreateIndexBuffer(sizeof(indices), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &this->indexBuffer, 0)))
    {
        return false;
    }
    void* tmpIndexBuffer;
    if (SUCCEEDED(this->indexBuffer->Lock(0, sizeof(indices), &tmpIndexBuffer, 0)))
    {
        memcpy(tmpIndexBuffer, indices, sizeof(indices));
        this->indexBuffer->Unlock();
    }
    else return false;
    return true;
}

void GoalFlag::SetParam(ID3DXEffect* effect)
{
    //effect->SetTechnique("Main");

    // 行列設定
    this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");
    effect->SetMatrix("mat_LocalWorld", &this->worldMatrix);

    // ライト方向
    D3DXVECTOR4 ld1 = { 0.5f, 0.4f, -0.5f, 1.0f };
    effect->SetVector("vec_LightDirection", &ld1);

    // ライト色
    this->hlslParam.lightColor = SkyBoxParam::ambientLightColor;
    auto color = this->hlslParam.GetLightColor();
    effect->SetVector("lightColor", &color);

    // カラーを設定
    auto mulColor = D3DXVECTOR4(1, 1, 1, 1);
    effect->SetVector("mulColor", &mulColor);
}

bool GoalFlag::Render(ID3DXEffect* effect)
{
    DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);

    DX9::instance->device3d->SetMaterial(&this->material);

    for (int i = 0; i < 11; i++)
    {

        if (i < 5)
        {
            DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            //Texture::instance->SetTextureByType(TextureResource::GOAL_FLAG);

            auto tex = Texture::instance->GetTexture("resources/textures/goal-flag-texture.png");
            effect->SetTexture("tex_Diffuse", tex);
            effect->CommitChanges();
        }
        else
        {
            //DX9::instance->device3d->SetTexture(0, nullptr);

            auto tex = Texture::instance->GetTexture("resources/textures/BaseWood.png");
            effect->SetTexture("tex_Diffuse", tex);
            effect->CommitChanges();
        }

        DX9::instance->device3d->SetStreamSource(0, this->vertexBuffer, 0, sizeof(GoalFlagVertex));
        DX9::instance->device3d->SetFVF(GoalFlagVertex::FVF);
        DX9::instance->device3d->SetIndices(this->indexBuffer);
        auto hr = DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, i * 4, 0, goalFlag::NUM_INDICES, 0, goalFlag::NUM_INDICES / 3);

        DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

        if (hr != S_OK) OutputDebugFormat("Draw GoalFlag failed.\n");
    }

	return true;
}

bool GoalFlag::Render()
{
    DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);

    DX9::instance->device3d->SetMaterial(&this->material);

    for (int i = 0; i < 11; i++)
    {

        if (i < 5)
        {
            DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            DX9::instance->device3d->SetTexture(0, Texture::instance->GetTexture("resources/textures/goal-flag-texture.png"));
        }
        else
        {
            DX9::instance->device3d->SetTexture(0, nullptr);
        }

        DX9::instance->device3d->SetStreamSource(0, this->vertexBuffer, 0, sizeof(GoalFlagVertex));
        DX9::instance->device3d->SetIndices(this->indexBuffer);
        DX9::instance->device3d->SetFVF(GoalFlagVertex::FVF);
        auto hr = DX9::instance->device3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, i * 4, 0, goalFlag::NUM_INDICES, 0, goalFlag::NUM_INDICES / 3);

        DX9::instance->device3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

        if (hr != S_OK) OutputDebugFormat("Draw GoalFlag failed.\n");
    }

    return true;
}

void GoalFlag::EventRender()
{

}

void GoalFlag::EventBeginPlay()
{
}

void GoalFlag::EventTick(float deltaTime)
{
    deg < 355 ? deg += 5 : deg = 0;
    float rad = (deg * D3DX_PI) / 180;

    GoalFlagVertex karivertices[goalFlag::NUM_VERTICES] =
    {
        // x      y      z

        // flag1
        { { 0.0f,  2.5f, 0.01f * sinf(rad + 0.0f)}, {0.02f * cosf(rad + 0.0f), 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.2f,  2.4f, 0.02f * sinf(rad + 2.0f)}, {0.02f * cosf(rad + 2.0f), 0.0f, -1.0f}, {0.2f, 0.1f} },
        { { 0.0f,  1.5f, 0.01f * sinf(rad + 0.0f)}, {0.02f * cosf(rad + 0.0f), 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.2f,  1.6f, 0.02f * sinf(rad + 2.0f)}, {0.02f * cosf(rad + 2.0f), 0.0f, -1.0f}, {0.2f, 0.9f} },

        // flag2
        { { 0.2f,  2.4f, 0.02f * sinf(rad + 2.0f)}, {0.02f * cosf(rad + 2.0f), 0.0f, -1.0f}, {0.2f, 0.1f} },
        { { 0.4f,  2.3f, 0.03f * sinf(rad + 4.0f)}, {0.02f * cosf(rad + 4.0f), 0.0f, -1.0f}, {0.4f, 0.2f} },
        { { 0.2f,  1.6f, 0.02f * sinf(rad + 2.0f)}, {0.02f * cosf(rad + 2.0f), 0.0f, -1.0f}, {0.2f, 0.9f} },
        { { 0.4f,  1.7f, 0.03f * sinf(rad + 4.0f)}, {0.02f * cosf(rad + 4.0f), 0.0f, -1.0f}, {0.4f, 0.8f} },

        // flag3
        { { 0.4f,  2.3f, 0.03f * sinf(rad + 4.0f)}, {0.02f * cosf(rad + 4.0f), 0.0f, -1.0f}, {0.4f, 0.2f} },
        { { 0.6f,  2.2f, 0.04f * sinf(rad + 6.0f)}, {0.02f * cosf(rad + 6.0f), 0.0f, -1.0f}, {0.6f, 0.3f} },
        { { 0.4f,  1.7f, 0.03f * sinf(rad + 4.0f)}, {0.02f * cosf(rad + 4.0f), 0.0f, -1.0f}, {0.4f, 0.8f} },
        { { 0.6f,  1.8f, 0.04f * sinf(rad + 6.0f)}, {0.02f * cosf(rad + 6.0f), 0.0f, -1.0f}, {0.6f, 0.7f} },

        // flag4
        { { 0.6f,  2.2f, 0.04f * sinf(rad + 6.0f)}, {0.02f * cosf(rad + 6.0f), 0.0f, -1.0f}, {0.6f, 0.3f} },
        { { 0.8f,  2.1f, 0.04f * sinf(rad + 8.0f)}, {0.02f * cosf(rad + 8.0f), 0.0f, -1.0f}, {0.8f, 0.4f} },
        { { 0.6f,  1.8f, 0.04f * sinf(rad + 6.0f)}, {0.02f * cosf(rad + 6.0f), 0.0f, -1.0f}, {0.6f, 0.7f} },
        { { 0.8f,  1.9f, 0.04f * sinf(rad + 8.0f)}, {0.02f * cosf(rad + 8.0f), 0.0f, -1.0f}, {0.8f, 0.6f} },

        // flag5
        { { 0.8f,  2.1f, 0.04f * sinf(rad + 8.0f)}, {0.02f * cosf(rad + 8.0f), 0.0f, -1.0f}, {0.8f, 0.4f} },
        { { 1.0f,  2.0f, 0.03f * sinf(rad +10.0f)}, {0.02f * cosf(rad +10.0f), 0.0f, -1.0f}, {1.0f, 0.5f} },
        { { 0.8f,  1.9f, 0.04f * sinf(rad + 8.0f)}, {0.02f * cosf(rad + 8.0f), 0.0f, -1.0f}, {0.8f, 0.6f} },
        { { 1.0f,  2.0f, 0.03f * sinf(rad +10.0f)}, {0.02f * cosf(rad +10.0f), 0.0f, -1.0f}, {1.0f, 0.5f} },

        // poal1
        { { 0.00f,  2.5f, -0.05f}, {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.05f,  2.5f,  0.00f}, {1.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { { 0.00f, -1.5f, -0.05f}, {1.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.05f, -1.5f,  0.00f}, {1.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },

        // poal2
        { { 0.05f,  2.5f, 0.00f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f, 0.05f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { { 0.05f, -1.5f, 0.00f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { { 0.00f, -1.5f, 0.05f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },

        // poal3
        { { 0.00f,  2.5f, 0.05f}, {-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
        { {-0.05f,  2.5f, 0.00f}, {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { { 0.00f, -1.5f, 0.05f}, {-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { {-0.05f, -1.5f, 0.00f}, {-1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },

        // poal4
        { {-0.05f,  2.5f,  0.00f}, {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f, -0.05f}, {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { {-0.05f, -1.5f,  0.00f}, {-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.00f, -1.5f, -0.05f}, {-1.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },

        // top
        { {-0.05f,  2.5f,  0.00f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.00f,  2.5f,  0.05f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.00f,  2.5f, -0.05f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.05f,  2.5f,  0.00f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },

        // bottom
        { {-0.05f, -1.5f,  0.00f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.00f, -1.5f, -0.05f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.00f, -1.5f,  0.05f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.05f, -1.5f,  0.00f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} }
    };

    memcpy(this->vertices, karivertices, sizeof(karivertices));

    void* tmpVertexBuff;
    if (SUCCEEDED(this->vertexBuffer->Lock(0, sizeof(vertices), &tmpVertexBuff, 0)))
    {
        memcpy(tmpVertexBuff, this->vertices, sizeof(vertices));
        this->vertexBuffer->Unlock();
    }
    else return;
}

void GoalFlag::EventKeyDown(KeyEvent e)
{
}

void GoalFlag::EventKeyUp(KeyEvent e)
{
}

void GoalFlag::EventMouseDown(MouseEvent e)
{
}

void GoalFlag::EventMouseUp(MouseEvent e)
{
}
