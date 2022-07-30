#include "XModel.hpp"
#include "Texture.hpp"
#include "Stage.hpp"
#include "Koro.hpp"
#include "ConvexHull.hpp"
#include "DayNightCycleSkyBoxParam.hpp"
#include "Game.hpp"

//XModel::XModel() : XModel(nullptr, nullptr)
//{}

XModel::XModel(const D3DXMATRIX* matrix, Transform* parent, const char* id)
    : Actor(matrix, parent, id)
{
    // idに一致するパターンデータを取得
    this->actorPatternData = ActorManager::patternLoader->GetPatternDataFromId(this->id);

    // テクスチャを取得
    this->texture = Texture::instance->GetTexture(this->actorPatternData.xModelInfo.tex_path);
}

XModel::~XModel()
{
    // OutputDebugFormat("\n{}() called.", __func__);
}

void XModel::SetParam(ID3DXEffect* effect)
{
    Actor::SetParam(effect);

    // カラーを設定
    this->hlslParam.mulColor = this->actorPatternData.color;
    auto mulColor = this->hlslParam.GetMulColor();
    effect->SetVector("mulColor", &mulColor);

    // テクスチャ
    effect->SetTexture("tex_Diffuse", this->texture);
}

bool XModel::Render(ID3DXEffect* effect)
{
    if (!this->isIgnore)
    {
        //DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);

        //DX9::instance->device3d->SetTexture(0, this->texture);

        Meshes::instance->RenderMesh(this->actorPatternData.xModelInfo.x_path.c_str());

#if 0
        // 凸包表示
        //auto ch = Meshes::instance->GetConvexHull(this->actorPatternData.xModelInfo.x_path.c_str());
        //if(ch)ch->Render(this);
#endif
    }

    return true;
}

void XModel::RenderInEditor()
{
    DX9::instance->device3d->SetTransform(D3DTS_WORLD, &this->worldMatrix);
    DX9::instance->device3d->SetTexture(0, this->texture);
    Meshes::instance->RenderMesh(this->actorPatternData.xModelInfo.x_path.c_str());
}

std::tuple<D3DXVECTOR3, float> XModel::CollisionWithKoro(D3DXVECTOR3 center, float radius)
{
    if (this->isIgnore/* || !this->enableCollision*/) return { { 0.0f,0.0f,0.0f }, 0.0f };

    auto convexHull = Meshes::instance->GetConvexHull(this->actorPatternData.xModelInfo.x_path.c_str());

    if (!convexHull) return { {0,0,0}, 0 };

    //////////////////////////////////////////////////
    // 簡易判定
    D3DXVECTOR3 convexHullCenter = convexHull->GetCenter();
    D3DXMATRIX centerMatrix;
    D3DXMatrixIdentity(&centerMatrix);
    centerMatrix._41 = convexHullCenter.x;
    centerMatrix._42 = convexHullCenter.y;
    centerMatrix._43 = convexHullCenter.z;
    centerMatrix *= this->worldMatrix;
    D3DXVECTOR3 scale = this->GetWorldScale();
    std::array<float, 3> tempScale = { scale.x, scale.y, scale.z };
    float coodedRadius = convexHull->GetRadius() * *std::max_element(tempScale.begin(), tempScale.end());
    auto dierction = D3DXVECTOR3(centerMatrix._41, centerMatrix._42, centerMatrix._43) - center;
    if ((radius + coodedRadius) < D3DXVec3Length(&dierction))
    {
        return { {0,0,0}, 0 };
    }

    //////////////////////////////////////////////////
    // 詳細判定

    // 点から三角形への最短ベクトルを求める
    // return pair<D3DXVECTOR3, float>
    // 1. 最短ベクトル
    // 2. 点が面の表裏どっちか(正:表 / 負:裏)
    auto CalcDist_PointToTriangle = [](const D3DXVECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c)
    {
        // 戻り値 : 点から三角形への最短のベクトル
        D3DXVECTOR3 result(FLT_MAX, FLT_MAX, FLT_MAX);

        // 判定用平面の生成
        D3DXVECTOR3 ab = b - a;
        D3DXVECTOR3 ac = c - a;
        D3DXVECTOR3 normal(0,0,0);
        D3DXVec3Cross(&normal, &ab, &ac);
        D3DXVec3Normalize(&normal, &normal);
        D3DXPLANE plane(0, 0, 0, 0);
        D3DXPlaneFromPointNormal(&plane, &a, &normal);
        D3DXPlaneNormalize(&plane, &plane);

        // 点と平面の最短点が三角形内にあるか判別
        float dot = D3DXPlaneDotCoord(&plane, &p);
        D3DXVECTOR3 h  = p + dot * -normal;  // 点と平面の最短点
        D3DXVECTOR3 ah = h - a;
        D3DXVECTOR3 bh = h - b;
        D3DXVECTOR3 ch = h - c;
        D3DXVECTOR3 bc = c - b;
        D3DXVECTOR3 ca = a - c;
        D3DXVECTOR3 cross1(0, 0, 0);
        D3DXVECTOR3 cross2(0, 0, 0);
        D3DXVECTOR3 cross3(0, 0, 0);
        D3DXVec3Cross(&cross1, &ab, &bh);
        D3DXVec3Cross(&cross2, &bc, &ch);
        D3DXVec3Cross(&cross3, &ca, &ah);
        if (D3DXVec3Dot(&cross1, &cross2) > -FLT_EPSILON && D3DXVec3Dot(&cross1, &cross3) > -FLT_EPSILON)
        {
            result = h - p;
        }
        // 点と三角形の辺の最短ベクトルを求める
        else
        {
            // ループ用
            const D3DXVECTOR3* const verts[3] =
            {
                &a, &b, &c
            };

            // 各頂点からpへのベクトル
            D3DXVECTOR3 p_verts[3] =
            {
                p - a,
                p - b,
                p - c
            };

            // 各辺のベクトル
            D3DXVECTOR3 edges[3] =
            {
                ab,
                bc,
                ca
            };

            for (int i = 0; i < 3; ++i)
            {
                float len = D3DXVec3Length(&edges[i]);
                D3DXVECTOR3 normal(0, 0, 0);
                D3DXVec3Normalize(&normal, &edges[i]);
                float lenH = D3DXVec3Dot(&normal, &p_verts[i]);

                D3DXVECTOR3 vec(0, 0, 0);
                if (lenH < 0) vec = *verts[i] - p;
                else if (lenH > len) vec = *verts[(i + 1) % 3] - p;
                else vec = normal * lenH - p_verts[i];

                if (D3DXVec3LengthSq(&result) > D3DXVec3LengthSq(&vec))
                {
                    result = vec;
                }
            }
        }
        return std::make_pair(result, dot);
    };

    auto& faces = convexHull->GetFaces();

    // 中心との距離(符号付)
    float signedMinLen = FLT_MAX;
    D3DXVECTOR3 resultNormal(0,0,0);
    for (size_t i = 0; i < faces.size(); ++i)
    {
        ConvexHull::Face face = faces[i];

        // ワールド変換
        D3DXMATRIX world;
        D3DXMatrixIdentity(&world);
        world._41 = face.a.x;
        world._42 = face.a.y;
        world._43 = face.a.z;
        world *= this->worldMatrix;
        face.a = { world._41, world._42, world._43 };

        D3DXMatrixIdentity(&world);
        world._41 = face.b.x;
        world._42 = face.b.y;
        world._43 = face.b.z;
        world *= this->worldMatrix;
        face.b = { world._41, world._42, world._43 };

        D3DXMatrixIdentity(&world);
        world._41 = face.c.x;
        world._42 = face.c.y;
        world._43 = face.c.z;
        world *= this->worldMatrix;
        face.c = { world._41, world._42, world._43 };

        auto [nearVec, dot] = CalcDist_PointToTriangle(center, face.a, face.b, face.c);
        float len = D3DXVec3Length(&nearVec);
        if (len < abs(signedMinLen))
        {
            if (dot < 0) len *= -1;
            signedMinLen = len;
            resultNormal = face.GetNormal();
        }
    }
    signedMinLen -= radius;
    if (signedMinLen > 0) signedMinLen = 0;

    return { resultNormal, -signedMinLen };
}

std::pair<D3DXVECTOR3, float> XModel::GetWorldRoughSphere()
{
    D3DXVECTOR3 center(0, 0, 0);
    auto ch = Meshes::instance->GetConvexHull(this->actorPatternData.xModelInfo.x_path.c_str());
    if(ch) center = ch->GetCenter();
    D3DXVec3TransformCoord(&center, &center, &this->worldMatrix);

    float radius = 0;
    auto scale = GetWorldScale();
    auto s = std::max_element((FLOAT*)scale, (FLOAT*)scale + 3);
    if (ch) radius = ch->GetRadius();

    return { center, radius * *s };
}

//Actor* XModel::CreateClone()
//{
//    return new XModel(&this->worldMatrix, nullptr, this->id);
//}

KoroActorType XModel::GetActorType() const
{
    return KoroActorType::X_MODEL;
}
