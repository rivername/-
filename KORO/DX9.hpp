#include "const.hpp"
#include "utils.hpp"
#pragma once

class DX9
{
public:
    static inline SmartPointer(DX9) instance;

public:
    DX9(HWND hwnd);
    ~DX9();

public:
    // 初期化
    bool Init();

    // 画面クリア
    bool Clear(D3DCOLOR color);

    // Begin
    bool Begin();

    // End
    bool End();

    // Present
    bool Present();

public:
    IDirect3DDevice9* device3d;

private:
    HWND        hwnd;
    IDirect3D9* direct3d;
};
