#include "Actor.hpp"
#include "GoalFlag.hpp"
#include "Window.hpp"
#include "DayNightCycleSkyBoxParam.hpp"

Actor::Actor() : Actor(nullptr, nullptr)
{}

Actor::Actor(const D3DXMATRIX* const matrix, Transform* parent, const char* id)
    : Transform(parent, matrix), isIgnore(false), enableCollision(true), visible(true), judgeColor(), friction(0.02f), resistance(0.9f), id()
{
    this->SetHLSL(HLSL::GetHLSLByTag("ACTOR"));

    this->judgeColor = Actor::CreateUniqueNum();
    //this->judgeColor = (unsigned)(this) & 0xffffff;
    this->judgeColorFloat4[0] = ((this->judgeColor >> 16) & 0xff) / 255.0f;	//r
    this->judgeColorFloat4[1] = ((this->judgeColor >>  8) & 0xff) / 255.0f;	//g
    this->judgeColorFloat4[2] = ((this->judgeColor      ) & 0xff) / 255.0f;	//b
    this->judgeColorFloat4[3] = 1.0f;										//a

    if (id)
    {
        strcpy_s(this->id, 32, id);
    }
}

Actor::~Actor()
{
    //OutputDebugFormat("\n\n {}\n\n", __func__);
}

const DWORD& Actor::GetJudgeColor() const
{
	return this->judgeColor;
}

const float* Actor::GetJudgeColorFloat4() const
{
    return this->judgeColorFloat4;
}

std::tuple<D3DXVECTOR3, float> Actor::CollisionWithKoro(D3DXVECTOR3 center, float radius)
{
    // 無視
    if (this->isIgnore || !this->enableCollision) return { { 0.0f,0.0f,0.0f }, 0.0f };

    D3DXVECTOR3 tmpScale = this->GetWorldScale();
    D3DXVECTOR3 vec      = center - this->GetWorldLocation();   // KOROへのベクトル

                                                        // 近くにいないとき
    if ((D3DXVec3Length(&tmpScale) + radius) < D3DXVec3Length(&vec))
        return  { { 0.0f,0.0f,0.0f } ,0.0f };

    // 出力用
    D3DXVECTOR3 result      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);   // koroとの最短距離
    D3DXVECTOR3 innerResult = D3DXVECTOR3(0.0f, 10.0f, 0.0f);  // 内側に入った場合の最短距離
    float umariLength = 0;  // 埋まった長さ

                            // ループ用配列
    D3DXVECTOR3 directions[3] =
    {
        this->GetRightVector(),
        this->GetUpVector(),
        this->GetForwardVector()
    };

    float length[3] =
    {
        tmpScale.x / 2.0f,
        tmpScale.y / 2.0f,
        tmpScale.z / 2.0f
    };

    // 各軸で中心へのベクトルを算出
    bool umatteru = true;
    for (int i = 0; i < 3; i++)
    {
        // その方向への距離を計算
        float s = D3DXVec3Dot(&directions[i], &vec) / length[i];

        // はみ出している場合
        if (s > 1) {
            result += directions[i] * (s - 1) * length[i];
            umatteru = false;
        }
        else if (s < -1) {
            result += directions[i] * (s + 1) * length[i];
            umatteru = false;
        }
        // はみ出していない場合
        else
        {
            // その方向の埋まっている距離を取得
            D3DXVECTOR3 abcde = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            if (s > 0) abcde += directions[i] * (s - 1) * length[i];
            else if (s < 0) abcde += directions[i] * (s + 1) * length[i];
            else abcde = D3DXVECTOR3(1000.0f, 1000.0f, 1000.0f);    // 仮

                                                                    // 長さを比較
            float oldLength = D3DXVec3Length(&innerResult);
            float newLength = D3DXVec3Length(&abcde);

            // 既存のものより短い場合、置き換える
            if (oldLength > newLength)
            {
                innerResult = abcde;
                umariLength = abs(newLength - radius);
            }
        }
    }

    // 埋まってる場合
    if (umatteru)
    {
        D3DXVec3Normalize(&innerResult, &innerResult);
        return { innerResult, umariLength };
    }

    // 球体までの距離
    umariLength = 0;
    float len = D3DXVec3Length(&result);
    if (len < radius)
    {
        umariLength = abs(len - radius);
    }

    D3DXVec3Normalize(&result, &result);

    return { result, umariLength };
}

std::pair<D3DXVECTOR3, float> Actor::GetWorldRoughSphere()
{
    auto scale = GetWorldScale();
    auto s = std::max_element((FLOAT*)scale, (FLOAT*)scale + 3);
    return { this->GetWorldLocation(), 0.85f * *s };
}

void Actor::SetParam(ID3DXEffect* effect)
{
    // 行列設定
    this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");
    effect->SetMatrix("mat_LocalWorld", &this->worldMatrix);

    // ライト色
    this->hlslParam.lightColor = SkyBoxParam::ambientLightColor;
    auto lightColor = this->hlslParam.GetLightColor();
    effect->SetVector("lightColor", &lightColor);


}