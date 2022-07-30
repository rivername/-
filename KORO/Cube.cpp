#include "Cube.hpp"
#include "Koro.hpp"
#include "Meshes.hpp"
#include "Stage.hpp"
#include "Game.hpp"

Cube::Cube(const D3DXMATRIX* worldMatrix, Transform * parent, const char* id)
    : Actor(worldMatrix, parent, id), material()
{

    this->material =
    {
        .Diffuse = {1, 1, 1, 1},
        .Ambient = {0.75f,0.75f,0.75f,1},
    };

    // idに一致するパターンデータを取得
    this->actorPatternData = ActorManager::patternLoader->GetPatternDataFromId(this->id);

    auto texturePaths = this->actorPatternData.cubeInfo.texPaths;

    // 6面分のテクスチャを取得して格納
    for (byte i = 0; i < 6; i++)
    {
        // パスからテクスチャを取得
        if (!texturePaths[0].empty())
        if ( texturePaths[i] != "")
        {
            this->textures[i] = Texture::instance->GetTexture(texturePaths[i]);
        }
        else
        {
            // パスが空の場合は先頭(front)のテクスチャを使用
            this->textures[i] = Texture::instance->GetTexture(texturePaths[0]);
        }
    }
}

Cube::~Cube()
{
    //OutputDebugFormat(" {} called.", __func__);
}

void Cube::SetParam(ID3DXEffect* effect)
{
    Actor::SetParam(effect);

    // カラー変数
    this->hlslParam.mulColor = this->actorPatternData.color;

    //if (this->actorPatternData.cubeInfo.type == KoroCubeInfo::Type::COLORED)
    //if (this->GetLocalLocation().y <= 0.5f)
    //{
    //    this->actorPatternData.color = 0xC25E00;
    //}

    auto mulColor = this->hlslParam.GetMulColor();

    // COLOREDの場合の色調整
    if (this->actorPatternData.cubeInfo.type == KoroCubeInfo::Type::COLORED)
    {
        mulColor *= 1.2f;
    }
    else
    {
        //OutputDebugString("");
    }

    // カラーを設定
    effect->SetVector("mulColor", &mulColor);
}

bool Cube::Render(ID3DXEffect* effect)
{
    if (!this->isIgnore && this->visible)
    {
        auto& [mesh, num] = Meshes::instance->cubeMeshInfo;

        for (DWORD i = 0; i < 6; ++i)
        {
            if (effect)
            {
                effect->SetTexture("tex_Diffuse", this->textures[i]);
                effect->CommitChanges();
            }
            mesh->DrawSubset(i);
        }
    }

    return true;
}
//
//Actor* Cube::CreateClone()
//{
//    return new Cube(&this->worldMatrix, nullptr, this->id);
//}

KoroActorType Cube::GetActorType() const
{
    return KoroActorType::CUBE;
}
