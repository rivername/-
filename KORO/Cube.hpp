#include <tuple>
#include <map>

#include "GoalFlag.hpp"
#include "Actor.hpp"
#include "HLSL.hpp"

#pragma once

namespace cube
{
    constexpr unsigned int NUM_VERTICES = 24;
    constexpr unsigned int NUM_INDICES  = 6;
}

struct CubeVertex
{
    D3DXVECTOR3 location;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 uv;

    static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_TEX1);
};

class Cube : public virtual Actor
{
public:
    Cube() = delete;
    Cube(const D3DXMATRIX* worldMatrix, Transform* parent = nullptr, const char* id = "");

    ~Cube();

public:
    // パラメーターを設定
    virtual void SetParam(ID3DXEffect* effect) override;

    // 描画 : HLSL
    virtual bool Render(ID3DXEffect* effect) override;

private:
    // マテリアル
    D3DMATERIAL9 material;

    // パターンデータ
    KoroActorPatternData actorPatternData;

    // 6面用テクスチャ配列
    IDirect3DTexture9* textures[6];

private:
    //virtual Actor* CreateClone() override;
    virtual KoroActorType GetActorType() const override;
};
