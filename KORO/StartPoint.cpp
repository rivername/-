#include "StartPoint.hpp"

#include "Meshes.hpp"
#include "Koro.hpp"

StartPoint::StartPoint() : StartPoint(nullptr)
{}

StartPoint::StartPoint(const D3DXMATRIX* const matrix, Transform* parent)
	: Actor(matrix, parent)
{
    // 当たり判定なし
    this->enableCollision = false;

    this->visible = false;
}

StartPoint::~StartPoint()
{
    OutputDebugFormat("\n {} called. \n", __func__);
}


//Actor* StartPoint::CreateClone()
//{
//    // 複製禁止
//    return nullptr;
//}

KoroActorType StartPoint::GetActorType() const
{
    return KoroActorType::START_POINT;
}

void StartPoint::SetParam(ID3DXEffect* effect)
{
    Actor::SetParam(effect);

    // カラーを設定
    auto mulColor = D3DXVECTOR4(1,1,1,1);
    effect->SetVector("mulColor", &mulColor);

    // テクスチャ
    effect->SetTexture("tex_Diffuse", Texture::instance->GetTexture("resources/textures/koro-texture.png"));
}

bool StartPoint::Render(ID3DXEffect* effect)
{
    if (!this->isIgnore && this->visible)
    {
        Meshes::instance->RenderMesh("resources/x/koro_kari.x");
    }

    return true;
}