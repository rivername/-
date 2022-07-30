#include "const.hpp"
#include "utils.hpp"
#pragma once

class Window
{
public:
	inline static SmartPointer(Window) instance;

public:
	Window(UINT posX, UINT posY, UINT width, UINT height, bool fullscreen);
	~Window();

public:
	// 初期化
	bool Init();

	// メッセージループ開始
	bool Run();

	// メッセージループ
	bool OnMessageLoop();

	// 描画
	bool Render();

public:
	// 位置
	UINT posX,
		 posY;

	// サイズ
	UINT width,
		 height;

    // ビューポートのずれた位置値
    POINT viewportOffset;

    // フルスクリーン
    bool fullscreen;

	// 稼働状態
	bool isRunning;

	// フォーカス状態
	bool focused;

	// DeltaTime
	float deltaTime;

	// FPS
	float fps;

    D3DCOLOR clearColor;

private:
	HINSTANCE hInstance;
	HWND      hwnd;

public:
	void SetHWND(HWND hwnd);
	HWND GetHWND();

public:
    // マウス座標を取得（座標変換済み）
    const POINT GetMouseCursorPos();
};
