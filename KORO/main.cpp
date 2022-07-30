#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include "Window.hpp"
#include "DX9.hpp"
#include "Game.hpp"
#include "Camera.hpp"
#include "Canvases.hpp"
#include "Texture.hpp"
#include "LevelEditor.hpp"
#include "Meshes.hpp"
#include "ValueConnector.hpp"
//#include "KoroMask.hpp"
#include "LevelEditor.hpp"

int CALLBACK WinMain
(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPSTR     lpCmdLine,
    _In_     int       nShowCmd)
{
    /*==================================
               ウィンドウの初期化
    ==================================*/
    if (!(new Window(SCREEN_POS_X, SCREEN_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT,true))->Init())
    {
        //PostQuitMessage(-1);
    }

    /*==================================
               DirectX 9の初期化
    ==================================*/
    if (!(new DX9(Window::instance->GetHWND()))->Init())
    {
        PostQuitMessage(-1);
    }

    /*==================================
                ゲームの初期化
         Stage, Koro, Cameraも初期化
    ==================================*/
    
    new Meshes();

    // インクルード


#if 1
    new LevelEditor(0);
#else
    new Game(AppMode::GAME);
#endif
    


    /*==================================
                   スタート
              メッセージループ開始
    ==================================*/
    Window::instance->Run();


    ValueConnector::SaveAll();

    // 終了
    return 0;
}