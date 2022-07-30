#include "PanelGui.hpp"
#include "Window.hpp"

PanelGui::PanelGui
(
    D3DXVECTOR2 position,
    LONG        width,
    LONG        height,
    D3DXCOLOR   color,
    BorderType  borderType,
    float       borderRadius,
    D3DXVECTOR2 pivotOffset
) : Gui(position, width, height, pivotOffset),outerBorderRadius(0.0f), innerBorderRadius(0.0f), borderType(borderType), vertex_buffer(nullptr)
{
    // HLSLセット
    this->SetHLSL(HLSL::GetHLSLByTag("GUI-PANEL"));

    this->color = color;

    // ボーダータイプによるボーダー半径設定先
    borderType != BorderType::INNER_MASK ?
        this->outerBorderRadius = borderRadius :
        this->innerBorderRadius = borderRadius;

#pragma region 頂点バッファ作成

    // 頂点
    // 2D射影行列の性質により上下が逆さになるので
    // 0   1
    // 
    // 2   3 を
    // -----
    // 2   3
    // 
    // 0   1 に変更
    D3DXVECTOR3 VERTICES_BLOCK_PANEL[4] =
    {
        D3DXVECTOR3(-(width * 0.5f), -(height * 0.5f), 0.0f),
        D3DXVECTOR3( (width * 0.5f), -(height * 0.5f), 0.0f),
        D3DXVECTOR3(-(width * 0.5f),  (height * 0.5f), 0.0f),
        D3DXVECTOR3( (width * 0.5f),  (height * 0.5f), 0.0f)
    };

    // ########## 頂点バッファ作成 ##########
    const UINT verts_length     = ARRAYSIZE(VERTICES_BLOCK_PANEL);
    const UINT verts_array_size = sizeof(D3DXVECTOR3) * verts_length;

    // 作成
    if (FAILED(DX9::instance->device3d->CreateVertexBuffer
    (
        verts_array_size,
        0,
        D3DFVF_XYZ,
        D3DPOOL_MANAGED,
        &this->vertex_buffer,
        nullptr
    ))) RELEASE_RETURN(this);

    void* pVertices = nullptr;
    this->vertex_buffer->Lock(0, verts_array_size, (void**)&pVertices, D3DLOCK_NOSYSLOCK);
    memcpy(pVertices, VERTICES_BLOCK_PANEL, verts_array_size);
    this->vertex_buffer->Unlock();

#pragma endregion
}

PanelGui::~PanelGui()
{
    this->Release();
}

void PanelGui::SetParam(ID3DXEffect* effect)
{
    // ワールド行列を設定
    this->GetWorldMatrix(&this->hlslParam.mat_World);
    effect->SetMatrix("mat_World", &this->hlslParam.mat_World);

    D3DXMATRIX mat_World_OnlyPosition;
    D3DXMatrixTranslation(&mat_World_OnlyPosition, this->position.x, this->position.y, 0.0f);
    effect->SetMatrix("mat_World_OnlyPosition", &mat_World_OnlyPosition);

    // 乗算色を設定
    this->hlslParam.mulColor = this->color;
    auto mulColor = this->hlslParam.GetMulColor();
    effect->SetVector("mulColor", &mulColor);

    D3DXVECTOR4 vec_ScreenPos =
    {
        this->position.x + this->pivotOffset.x,
        this->position.y + this->pivotOffset.y,
        0.0f, 0.0f
    };

    // スクリーン座標
    effect->SetVector("vec_ScreenPos", &vec_ScreenPos);

    // 幅・高さ
    effect->SetFloat("width",  this->width);
    effect->SetFloat("height", this->height);

    // BorderType
    effect->SetInt("enum_BorderType", static_cast<int>(this->borderType));

    // ボーダー半径を設定 // スケーリング済みを渡す
    effect->SetFloat("outerBorderRadius", this->outerBorderRadius * Gui::GetScalingValue());
    effect->SetFloat("innerBorderRadius", this->innerBorderRadius * Gui::GetScalingValue());
}

bool PanelGui::Render(ID3DXEffect* effect)
{
    // Vertexを設定
    DX9::instance->device3d->SetStreamSource(0, this->vertex_buffer, 0, sizeof(D3DXVECTOR3));

    // FVFを設定
    DX9::instance->device3d->SetFVF(D3DFVF_XYZ);

    // ポリゴン描画
    DX9::instance->device3d->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    return true;
}

void PanelGui::Release()
{
    Gui::Release();
    SAFE_RELEASE(this->vertex_buffer);
}
