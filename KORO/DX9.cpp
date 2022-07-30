#include "DX9.hpp"
#include "Window.hpp"

DX9::DX9(HWND hwnd) : device3d(nullptr), hwnd(hwnd), direct3d(nullptr)
{
    DX9::instance.reset(this);
}

DX9::~DX9()
{
    SAFE_RELEASE(this->device3d);
    SAFE_RELEASE(this->direct3d);
}

bool DX9::Init()
{
    this->direct3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!this->direct3d) return false;

    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    d3dpp.BackBufferWidth        = Window::instance->width;
    d3dpp.BackBufferHeight       = Window::instance->height;

    d3dpp.BackBufferFormat       = D3DFMT_UNKNOWN;
    d3dpp.BackBufferCount        = 1;

    d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality     = 0;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    d3dpp.hDeviceWindow          = this->hwnd;

    d3dpp.Windowed               = true;

    d3dpp.EnableAutoDepthStencil = true;

    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    d3dpp.Flags                  = 0;

    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;

    // D3Dデバイスを作成 // 全パターン試す
    if (FAILED(this->direct3d->CreateDevice
    (
        D3DADAPTER_DEFAULT,                  // ディスプレイアダプタの指定
        D3DDEVTYPE_HAL,                      // 描画をする際に、HAL(ハードウェア)を使うかREF(ソフトウェア)を使うかの指定
        this->hwnd,                          // 描画するウィンドウのハンドルを指定
        D3DCREATE_HARDWARE_VERTEXPROCESSING, // 3Dの演算をハードウェアでするかソフトウェアでするかの指定
        &d3dpp,                              // プレゼンテーションパラメーターを指定(指定した変数のポインタ)
        &this->device3d                      // 生成したデバイスを受け取る変数のポインタを指定
    )))
    if (FAILED(this->direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &this->device3d)))
    if (FAILED(this->direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, this->hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &this->device3d)))
    if (FAILED(this->direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, this->hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &this->device3d)))
    {
        // 全パターンでもダメだった場合
        this->direct3d->Release();
        OutputDebugString("DirectXの初期化に失敗しました\n");
        return false;
    }

    // ビューポートの設定
    D3DVIEWPORT9 viewport;
    ZeroMemory(&viewport, sizeof(D3DVIEWPORT9));

    // X, y
    LONG x = 0L;
    LONG y = 0L;

    // Width, Height
    DWORD width  = 0L;
    DWORD height = 0L;

    // 16:9より縦長の場合
    if (((float)Window::instance->width / (float)Window::instance->height) < (16.0f / 9.0f))
    {
        // ウィンドウ横幅に合わせて16:9になるように縦幅設定
        width  = Window::instance->width;
        height = static_cast<DWORD>(width / 16.0f * 9.0f);

        // 縦の中央に設定
        y = (Window::instance->height / 2) - (height / 2);
    }
    // 16:9より横長の場合
    else
    {
        // ウィンドウ縦幅に合わせて16:9になるように横幅設定
        height = Window::instance->height;
        width  = static_cast<DWORD>(height / 9.0f * 16.0f);

        // 横の中央に設定
        x = (Window::instance->width / 2) - (width / 2);
    }

    viewport.X      = x;
    viewport.Y      = y;
    viewport.Width  = width;
    viewport.Height = height;
    viewport.MinZ   = 0.0f;
    viewport.MaxZ   = 1.0f;

    // サイズ再設定
    Window::instance->width  = width;
    Window::instance->height = height;

    // ViewportOffset設定
    Window::instance->viewportOffset = { x, y };

    // ビューポートをセット
    this->device3d->SetViewport(&viewport);

    // Zバッファ有効化
    this->device3d->SetRenderState(D3DRS_ZENABLE, true);

    // 遠方テクスチャのジャギーをなくすミップマップを有効にする
    this->device3d->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    this->device3d->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);

    this->device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

    this->device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    this->device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    return true;
}

bool DX9::Clear(D3DCOLOR color)
{
    this->device3d->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
    return true;
}

bool DX9::Begin()
{
    if (FAILED(this->device3d->BeginScene())) return false;
    return true;
}

bool DX9::End()
{
    if (FAILED(this->device3d->EndScene())) return false;
    return true;
}

bool DX9::Present()
{
    if (FAILED(this->device3d->Present(0, 0, 0, 0))) return false;
    return true;
}
