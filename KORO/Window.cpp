#include "Window.hpp"
#include "DX9.hpp"
#include "GameObject.hpp"
#include "Debug.hpp"
#include "resource.h"
#include "Camera.hpp"

// コールバック関数
LRESULT CALLBACK WndProc
(
    HWND   hwnd,    // ウィンドウハンドル
    UINT   message, // ウィンドウメッセージ
    WPARAM wp,      // パラメータ１
    LPARAM lp       // パラメータ２
)
{
    //必要なものを処理
    switch (message)
    {
    case WM_DESTROY:

        PostQuitMessage(0);
        break;

    case WM_SETFOCUS:

        Window::instance->focused = true;
        break;

    case WM_KILLFOCUS:

        Window::instance->focused = false;
        break;

    case WM_SYSKEYDOWN:

        if (wp == VK_MENU)
        {
            return 0;
        }
        break;

    case WM_MOUSEWHEEL:

        for (size_t i = 0; i < GameObject::all.size(); i++)
        {
            if (GameObject::all[i]->IsValid())
            {
                GameObject::all[i]->EventMouseWheelMove(GET_WHEEL_DELTA_WPARAM(wp) / 120);
            }
        }

        break;
    }

    // 不要なものを渡して処理
    return DefWindowProc(hwnd, message, wp, lp);
}

Window::Window(UINT posX, UINT posY, UINT width, UINT height, bool fullscreen)
    : posX(posX), posY(posY), width(width), height(height), fullscreen(fullscreen), hwnd(nullptr), deltaTime(0.0f)
{
    Window::instance.reset(this);
    clearColor = 0xFFCC99;
    //clearColor = 0x13a3ff;
    //clearColor = 0xFFFFFF;
    this->clearColor = 0x00;

}

Window::~Window()
{
}

bool Window::Init()
{
    WNDCLASSEX wcex;

    // ウィンドウクラスを登録
    wcex.cbSize = sizeof(WNDCLASSEX);                            // 必ず、WNDCLASSEXの構造体のサイズ
    wcex.style = 0;                                              // クラススタイルを指定(不要な場合は０)
    wcex.lpfnWndProc = WndProc;                                  // メッセージ処理のコールバック関数
    wcex.cbClsExtra = 0;                                         // WNDCLASSEXに追加したい補助メモリのサイズ
    wcex.cbWndExtra = 0;                                         // ウィンドウ内部データに追加したい補助メモリのサイズ
    wcex.hInstance = this->hInstance;                            // WinMainの第一引数を指定
    wcex.hIcon   = LoadIcon(nullptr, MAKEINTRESOURCE(APP_ICON)); // ウィンドウのアイコン
    wcex.hIconSm = LoadIcon(nullptr, MAKEINTRESOURCE(APP_ICON)); // タスクバー上でのアイコン
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);               // ウィンドウ内でのカーソル
    wcex.hbrBackground = nullptr;                                // ウィンドウの背景色
    wcex.lpszMenuName = nullptr;                                 // メニューリソースを識別するための文字列を指定
    wcex.lpszClassName = WINDOW_CLASS_NAME;                      // ウィンドウクラスを識別するための文字列を指定

    if (!RegisterClassEx(&wcex)) return false;

    DWORD style;

    // ボーダーレスフルスクリーン
    if (this->fullscreen)
    {
        style  = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

        this->width  = GetSystemMetrics(SM_CXSCREEN);
        this->height = GetSystemMetrics(SM_CYSCREEN);
        this->posX   = 0;
        this->posY   = 0;
    }
    // ウィンドウ
    else
    {
        style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
    }

    // ウィンドウ作成
    this->hwnd = CreateWindow
    (
        WINDOW_CLASS_NAME, // ウィンドウのクラス名
        WINDOW_TITLE,      // ウィンドウの名前
        style,             // ウィンドウのスタイル
        this->posX,        // ウィンドウの横位置
        this->posY,        // ウィンドウの縦位置
        this->width,       // ウィンドウの幅
        this->height,      // ウィンドウの高さ
        nullptr,           // 親ウィンドウのハンドル
        nullptr,           // メニューハンドル
        this->hInstance,   // インスタンスハンドル
        nullptr            // ウィンドウの作成データ
    );

    if (this->hwnd == nullptr) return false;

    // ウィンドウをタイトルバーの高さを考慮した高さに変更
    RECT rw, rc;
    GetWindowRect(this->hwnd, &rw);
    GetClientRect(this->hwnd, &rc);

    int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
    int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

    SetWindowPos(this->hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);

    ShowWindow(this->hwnd, SW_SHOW);

    return true;
}

bool Window::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg); // メッセージ変換
            DispatchMessage(&msg);  // プロシージャにコールバック
        }
        else
        {
            this->OnMessageLoop();
        }
    }

    return true;
}

bool Window::OnMessageLoop()
{
    this->Render();

    return true;
}

bool Window::Render()
{
    DX9::instance->Clear(this->clearColor);

    if (!DX9::instance->Begin())
    {
        DX9::instance->Present();
        return false;
    }
    
    Debug::AddLine("[@を押して表示切替]");
    Debug::AddLine("------------ debug ------------");
    
    Debug::PrintFont();
    

    GameObject::BaseUpdate();

    DX9::instance->End();
    DX9::instance->Present();

    return true;
}

void Window::SetHWND(HWND hwnd)
{
    this->hwnd = hwnd;
}

HWND Window::GetHWND()
{
    return this->hwnd;
}

const POINT Window::GetMouseCursorPos()
{
    // カーソール位置
    POINT cursor;
    GetCursorPos(&cursor);

    // クライアント座標に変換
    ScreenToClient(this->hwnd, &cursor);

    LONG mouseX = cursor.x - this->viewportOffset.x;
    LONG mouseY = cursor.y - this->viewportOffset.y;

    return { mouseX, mouseY };
}
