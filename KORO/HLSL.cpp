#include "HLSL.hpp"

void HLSLObject::SetHLSL(HLSL* hlsl)
{
    if (!hlsl) return;

    hlsl->hlslObjects.push_back(this);
    this->hlslParam.effect = hlsl->effect;
}

HLSL* HLSL::GetHLSLByTag(string tag)
{
    for (auto hlsl : HLSL::all)
    {
        if (hlsl->tag == tag) return hlsl;
    }

    return nullptr;
}

HLSL::HLSL(string filename, string tag) : effect(nullptr), tag(tag), enableAutoRenderProcess(false), setParamOnce(false)
{
    HLSL::all.push_back(this);

    this->Init(filename) ?
        OutputDebugFormat("[ HLSL ] {}: 初期化に成功しました\n", filename) :
        OutputDebugFormat("[ HLSL ] {}: 初期化に失敗しました\n", filename);
}

HLSL::HLSL(int resourceID, string tag) : effect(nullptr), tag(tag), enableAutoRenderProcess(false), setParamOnce(false)
{
    HLSL::all.push_back(this);

    this->Init(resourceID) ?
        OutputDebugFormat("[ HLSL ] {}: 初期化に成功しました\n", resourceID) :
        OutputDebugFormat("[ HLSL ] {}: 初期化に失敗しました\n", resourceID);
}

HLSL::~HLSL()
{
    SAFE_RELEASE(this->effect);
}

bool HLSL::Init(string filename)
{
    auto result =
        D3DXCreateEffectFromFileA(
        DX9::instance->device3d,
        filename.c_str(),
        nullptr,
        nullptr,
        0,
        nullptr,
        &this->effect,
        nullptr);

    return SUCCEEDED(result);
}

bool HLSL::Init(int resourceID)
{
    auto result =
        D3DXCreateEffectFromResourceA(
        DX9::instance->device3d, nullptr,
        MAKEINTRESOURCE(resourceID),
        nullptr,
        nullptr,
        0,
        nullptr,
        &this->effect,
        nullptr);

    return SUCCEEDED(result);
}

bool HLSL::BeginRender()
{
    if (!this->effect) return false;

    // シェーダーのパス数
    UINT numPass;

    // Begin
    if (FAILED(this->effect->Begin(&numPass, 0)))
    {
        return false;
    }

    // パス数分BeginPass
    for (UINT i = 0; i < numPass; i++)
    {
        if (FAILED(this->effect->BeginPass(i)))
        {
            return false;
        }
    }

    return true;
}

bool HLSL::Render(HLSLObject* target)
{
    if (target == nullptr) return false;

    // 一度のみパラメーターを設定
    if (!this->setParamOnce)
    {
        target->SetParamOnce(this->effect);
        this->setParamOnce = true;
    }

    // パラメーターを設定
    target->SetParam(this->effect);

    // グローバル値を更新
    if (FAILED(this->effect->CommitChanges()))
    {
        return false;
    }

    // 描画
    if (!target->Render(this->effect))
    {
        return false;
    }

    return true;
}

bool HLSL::Render(std::vector<HLSLObject*>& targetList)
{
    if (targetList.empty()) return false;

    // 一度のみパラメーターを設定
    if (!this->setParamOnce)
    {
        targetList[0]->SetParamOnce(this->effect);
        this->setParamOnce = true;
    }

    for (auto e : targetList)
    {
        // パラメーターを設定
        e->SetParam(this->effect);

        // グローバル値を更新
        if (FAILED(this->effect->CommitChanges()))
        {
            return false;
        }

        // 描画
        if (!e->Render(this->effect))
        {
            return false;
        }
    }

    return true;
}

bool HLSL::EndRender()
{
    if (!this->effect) return false;

    // EndPass
    if (FAILED(this->effect->EndPass()))
    {
        return false;
    }

    // End
    if (FAILED(this->effect->End()))
    {
        return false;
    }

    return true;
}

bool HLSL::RenderProcess(HLSLObject* target)
{
    // 引数がない場合は格納されたオブジェクトを描画
    if (target == nullptr)
    {
        return this->RenderProcess(this->hlslObjects);
    }

    // Begin
    if (this->BeginRender())
    {
        // Render
        this->Render(target);
    }

    // End
    this->EndRender();

    return true;
}

bool HLSL::RenderProcess(std::vector<HLSLObject*>& targetList)
{
    // Begin
    if (this->BeginRender())
    {
        // Render
        this->Render(targetList);
    }

    // End
    this->EndRender();

    return true;
}

void HLSL::RemoveHLSLObjectFromList(HLSLObject* hlslObject)
{
    this->hlslObjects.erase(std::remove(this->hlslObjects.begin(), this->hlslObjects.end(), hlslObject), this->hlslObjects.end());
}

void HLSL::EventBeginPlay()
{
}

void HLSL::EventRender()
{
}

void HLSL::EventTick(float deltaTime)
{
    if (this->enableAutoRenderProcess)
    {
        this->RenderProcess();
    }
}
