#include "DX9.hpp"
#pragma once

/*==================================
           HLSL基本パラメーター
==================================*/
struct HLSL_BasicParam
{
private:
    ID3DXEffect* effect;

private:
    friend class HLSLObject;

public:
    ~HLSL_BasicParam()
    {
        SAFE_RELEASE(this->tex_Diffuse);
        SAFE_RELEASE(this->tex_Normal);
    }

    D3DXMATRIX mat_World; // float4x4 : ワールド
    D3DXMATRIX mat_View;  // float4x4 : ビュー
    D3DXMATRIX mat_Proj;  // float4x4 : 射影

    IDirect3DTexture9* tex_Diffuse; // texture : Diffuse
    IDirect3DTexture9* tex_Normal;  // texture : Normal

    D3DXCOLOR mulColor;   // float4 : 乗算色
    D3DXCOLOR lightColor; // float4 : ライト色

    D3DXVECTOR3 vec_LightDirection; // vector : ライト方向

    // 基本行列の値を設定
    void SetBasicMatrices(D3DXHANDLE mat_World_Param, D3DXHANDLE mat_View_Param, D3DXHANDLE mat_Proj_Param)
    {
        D3DXMatrixIdentity(&this->mat_World);
        DX9::instance->device3d->GetTransform(D3DTS_VIEW, &this->mat_View);
        DX9::instance->device3d->GetTransform(D3DTS_PROJECTION, &this->mat_Proj);

        this->effect->SetMatrix(mat_World_Param, &this->mat_World);
        this->effect->SetMatrix(mat_View_Param,  &this->mat_View);
        this->effect->SetMatrix(mat_Proj_Param,  &this->mat_Proj);
    }

    bool CreateTexture_Diffuse(LPCSTR filename, UINT width, UINT height)
    {
        return this->CreateTexture(&this->tex_Diffuse, filename, -1, width, height);
    }

    bool CreateTexture_Diffuse(int resourceID, UINT width, UINT height)
    {
        return this->CreateTexture(&this->tex_Diffuse, nullptr, resourceID, width, height);
    }

    bool CreateTexture_Normal(LPCSTR filename, UINT width, UINT height)
    {
        return this->CreateTexture(&this->tex_Normal, filename, -1, width, height);
    }

    bool CreateTexture_Normal(int resourceID, UINT width, UINT height)
    {
        return this->CreateTexture(&this->tex_Normal, nullptr, resourceID, width, height);
    }

    bool CreateTexture(IDirect3DTexture9** texture, LPCSTR filename, int resourceID, UINT width, UINT height)
    {
        HRESULT result;

        // ミップマップ最大 / 高フィルターで作成

        if (resourceID == -1)
        {
            result =
                D3DXCreateTextureFromFileExA(
                DX9::instance->device3d,
                filename,
                width, height,
                0, 0,
                D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                D3DX_DEFAULT, D3DX_DEFAULT,
                0x00,
                nullptr,
                nullptr,
                texture);
        }
        else
        {
            result =
                D3DXCreateTextureFromResourceExA(
                DX9::instance->device3d, nullptr,
                MAKEINTRESOURCE(resourceID),
                width, height,
                0, 0,
                D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                D3DX_DEFAULT, D3DX_DEFAULT,
                0x00,
                nullptr,
                nullptr,
                texture);
        }

        return SUCCEEDED(result);
    }

    D3DXVECTOR4 GetMulColor()
    {
        return
        {
            this->mulColor.r,
            this->mulColor.g,
            this->mulColor.b,
            this->mulColor.a
        };
    }

    D3DXVECTOR4 GetLightColor()
    {
        return
        {
            this->lightColor.r,
            this->lightColor.g,
            this->lightColor.b,
            this->lightColor.a
        };
    }

    D3DXVECTOR4 GetLightDirection()
    {
        return
        {
            this->vec_LightDirection.x,
            this->vec_LightDirection.y,
            this->vec_LightDirection.z,
            1.0f
        };
    }
};
