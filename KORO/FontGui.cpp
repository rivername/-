#include "FontGui.hpp"
#include "DX9.hpp"
#include "FontManager.hpp"

FontGui::FontGui
(
    LPCSTR      text,
    D3DXVECTOR2 position,
    LONG        font_size,
    D3DXCOLOR   color,
    TextAlign   text_align,
    bool        monospace,
    LPCSTR      font_family
) : Gui(position, font_size, font_size), text(text), font_size(font_size), text_align(text_align), monospace(monospace), font_family(font_family), multiLine(false),
    isTextButton(false), isValidTextButton(true), enableTextButtonArrow(false)
{
    // シェーダーを設定
    this->SetHLSL(HLSL::GetHLSLByTag("GUI-DEFAULT"));

    // テキストを設定
    this->SetText(text);
}

FontGui::~FontGui()
{
    this->renderItems.clear();
}

void FontGui::SetParam(ID3DXEffect* effect)
{
    // FVFを設定
    DX9::instance->device3d->SetFVF(VertexPositionTexture::FVF);

    // テキストボタンの色設定
    if (this->isTextButton)
    {
        if (this->isValidTextButton)
        {
            if (this->isEnableInput && this->inputState.mouseOver)
            {
                this->color = TextButtonColor::HOVERED;
            }
            else
            {
                this->color = TextButtonColor::DEFAULT;
            }
        }
        else
        {
            this->color = TextButtonColor::INVALID;
        }
    }

    // 乗算色を設定
    this->hlslParam.mulColor = this->color;
    auto mulColor = this->hlslParam.GetMulColor();
    effect->SetVector("mulColor", &mulColor);
}

bool FontGui::Render(ID3DXEffect* effect)
{
    this->width = 0.0f;

    // 複数行の場合はオフセット変数を使って改行
    if (this->multiLine)
    {
        this->pivotOffset.x = 0.0f;
        this->pivotOffset.y = 0.0f;
    }

    // 文字列の幅を計算
    for (auto& item : this->renderItems)
    {
        auto& [v, t, size] = item;
        this->width += size;
    }

    // スケーリング値
    float scaling = Gui::GetScalingValue();

    // スケーリング
    this->width *= scaling;

    for (size_t i = 0; i < this->renderItems.size(); i++)
    {
        auto& [vertex_buffer, texture, size] = this->renderItems[i];

        if (!vertex_buffer && !texture) continue;

        // 改行判定
        if (IS_NEWLINE_TEX(texture))
        {
            this->pivotOffset.x = 0;
            this->pivotOffset.y += (this->height * scaling);
            continue;
        } else

        if (i > 0)
        {
            auto& [v, t, i_1_width] = this->renderItems[i - 1];
            this->pivotOffset.x += (i_1_width * scaling);
        }

        // テクスチャを設定
        effect->SetTexture("tex_Diffuse", texture);

        float worldOffsetX = 0.0f;

        if (this->text_align == TextAlign::LEFT)
        {
            worldOffsetX = -(this->font_size * 0.5f);
        }
        if (this->text_align == TextAlign::CENTER)
        {
            worldOffsetX = -(this->width * 0.5f) + (this->font_size * 0.5f);
        }
        if (this->text_align == TextAlign::RIGHT)
        {
            worldOffsetX = -this->width;
        }

        // ワールド行列
        this->GetWorldMatrix(&this->hlslParam.mat_World);

        // ワールドオフセット行列
        D3DXMATRIX mat_WorldOffset;
        D3DXMatrixTranslation(&mat_WorldOffset, worldOffsetX, 0.0f, 0.0f);
        this->hlslParam.mat_World *= mat_WorldOffset;

        // ワールド行列を設定
        effect->SetMatrix("mat_World", &this->hlslParam.mat_World);
        effect->CommitChanges();

        // Vertexを設定
        DX9::instance->device3d->SetStreamSource(0, vertex_buffer, 0, sizeof(VertexPositionTexture));

        // ポリゴン描画
        DX9::instance->device3d->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }

    this->pivotOffset.x = 0.0f;

    return true;
}

void FontGui::SetText(string text)
{
    this->text = text;

    FontManager::RenderManager::OutputRenderItemsOfString(text.c_str(), this);
}

string FontGui::GetText()
{
    return this->text;
}

LONG FontGui::GetFontSize()
{
    return this->font_size;
}

void FontGui::OnMouseUp()
{
    Gui::OnMouseUp();
}
