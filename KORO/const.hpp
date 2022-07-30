#include <Windows.h>
#include <memory>
#include <d3d9.h>
#include <d3dx9.h>
#include <format>
#include <vector>
#pragma once

const unsigned int SCREEN_WIDTH  = 1280;                         // 画面幅
const unsigned int SCREEN_HEIGHT = 720;                          // 画面高さ
const unsigned int SCREEN_POS_X  = 1920 / 2 - SCREEN_WIDTH / 2;  // 画面位置X
const unsigned int SCREEN_POS_Y  = 1080 / 2 - SCREEN_HEIGHT / 2; // 画面位置Y

// Windowクラス名
constexpr const char* WINDOW_CLASS_NAME = "DX9_WINDOW_CLASS";

// Windowタイトル
constexpr const char* WINDOW_TITLE      = "KORO";

const bool FULL_SCREEN = true;
