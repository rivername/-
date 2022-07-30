#include <Windows.h>
#include <memory>
#include <d3d9.h>
#include <d3dx9.h>
#include <format>
#include <vector>
#pragma once

const unsigned int SCREEN_WIDTH  = 1280;                         // ��ʕ�
const unsigned int SCREEN_HEIGHT = 720;                          // ��ʍ���
const unsigned int SCREEN_POS_X  = 1920 / 2 - SCREEN_WIDTH / 2;  // ��ʈʒuX
const unsigned int SCREEN_POS_Y  = 1080 / 2 - SCREEN_HEIGHT / 2; // ��ʈʒuY

// Window�N���X��
constexpr const char* WINDOW_CLASS_NAME = "DX9_WINDOW_CLASS";

// Window�^�C�g��
constexpr const char* WINDOW_TITLE      = "KORO";

const bool FULL_SCREEN = true;
