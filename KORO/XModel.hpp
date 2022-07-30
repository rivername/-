#include "Actor.hpp"
#include "Meshes.hpp"
#include "HLSL.hpp"
#include <map>
#pragma once

class ConvexHull;

class XModel : public virtual Actor
{
public:
    XModel() = delete;

    XModel(const D3DXMATRIX* matrix, Transform* parent = nullptr, const char* id = "");

    ~XModel();

public:
    // パラメーターを設定
    virtual void SetParam(ID3DXEffect* effect) override;

    // 描画
    virtual bool Render(ID3DXEffect* effect) override;

    virtual void RenderInEditor() override;


public:
    std::tuple<D3DXVECTOR3, float> CollisionWithKoro(D3DXVECTOR3 center, float radius) override;

    virtual std::pair<D3DXVECTOR3, float> GetWorldRoughSphere() override;

private:
    // パターンデータ
    KoroActorPatternData actorPatternData;

    // テクスチャ
    IDirect3DTexture9* texture;

private:
    //////////////////////////////////////////////////
    // 継承

    //virtual Actor* CreateClone() override;

    virtual KoroActorType GetActorType() const override;

};
