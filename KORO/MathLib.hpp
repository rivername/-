#include <algorithm>
#include <d3dx9.h>
#pragma once

// InterpTo - 構造を持たない変数用
// @param out         : 出力先アドレス
// @param current     : 現在の値
// @param to          : 目標の値
// @param interpSpeed : スピード
// @return Interp完了
template <class T>
inline bool InterpTo(__out T*    out,
                     __in  T     current,
                     __in  T     to,
                     __in  float interpSpeed)
{
    T     result    = current;
    float direction = (current < to ? 1.0f : -1.0f);
    result += static_cast<T>(direction * 0.1f * fabsf(interpSpeed));

    if (current < to)
    {
        *out = std::clamp(result, current, to);
    } else

    if (current > to)
    {
        *out = std::clamp(result, to, current);
    }
    else
    {
        // currentとtoが一致した（補間が終わった）場合true
        return true;
    }

    return false;
}

// Interp To - Vector4用
// @param out         : 出力先アドレス
// @param current     : 現在の値
// @param to          : 目標の値
// @param interpSpeed : スピード
// @return Interp完了
inline bool InterpToV4(__out D3DXVECTOR4* out,
                       __in  D3DXVECTOR4  current,
                       __in  D3DXVECTOR4  to,
                       __in  float        interpSpeed)
{
    D3DXVECTOR4 result    = current;
    D3DXVECTOR4 direction = to - current;
    D3DXVec4Normalize(&direction, &direction);

    bool x = InterpTo(&out->x, out->x, to.x, interpSpeed * -direction.x);
    bool y = InterpTo(&out->y, out->y, to.y, interpSpeed * -direction.y);
    bool z = InterpTo(&out->z, out->z, to.z, interpSpeed * -direction.z);
    bool w = InterpTo(&out->w, out->w, to.w, interpSpeed * -direction.w);

    return (x && y && z && w);
}

// Interp To - Vector3用
// @param out         : 出力先アドレス
// @param current     : 現在の値
// @param to          : 目標の値
// @param interpSpeed : スピード
// @return Interp完了
inline bool InterpToV3(__out D3DXVECTOR3* out,
                       __in  D3DXVECTOR3  current,
                       __in  D3DXVECTOR3  to,
                       __in  float        interpSpeed)
{
    D3DXVECTOR3 result    = current;
    D3DXVECTOR3 direction = to - current;
    D3DXVec3Normalize(&direction, &direction);

    bool x = InterpTo(&out->x, out->x, to.x, interpSpeed * -direction.x);
    bool y = InterpTo(&out->y, out->y, to.y, interpSpeed * -direction.y);
    bool z = InterpTo(&out->z, out->z, to.z, interpSpeed * -direction.z);

    return (x && y && z);
}

// Interp To - Vector2用
// @param out         : 出力先アドレス
// @param current     : 現在の値
// @param to          : 目標の値
// @param interpSpeed : スピード
// @return Interp完了
inline bool InterpToV2(__out D3DXVECTOR2* out,
                       __in  D3DXVECTOR2  current,
                       __in  D3DXVECTOR2  to,
                       __in  float        interpSpeed)
{
    D3DXVECTOR2 result    = current;
    D3DXVECTOR2 direction = to - current;
    D3DXVec2Normalize(&direction, &direction);

    bool x = InterpTo(&out->x, out->x, to.x, interpSpeed * -direction.x);
    bool y = InterpTo(&out->y, out->y, to.y, interpSpeed * -direction.y);

    return (x && y);
}
