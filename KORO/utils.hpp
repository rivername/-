#include <Windows.h>
#include <memory>
#include <string>
#include <format>
#include <random>
#pragma once

using std::string;

// スマートポインター
#define SmartPointer(T) std::unique_ptr<T>

#define SAFE_DELETE(x)  { if (x) delete x;     x = nullptr; }
#define SAFE_RELEASE(x) { if (x) x->Release(); x = nullptr; }

template<class... Args>
inline void OutputDebugFormat(std::string str, Args... args)
{
    OutputDebugString(std::format(str, args...).c_str());
}

/*==================================
           Get Random Int
==================================*/
inline int GetRandomInt(int min, int max)
{
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> random(min, max);
    return random(mt);
}

/*==================================
          トースト通知を送信
==================================*/
inline void SendToastNotice(LPCSTR title, LPCSTR desc)
{
    NOTIFYICONDATA data = {};
    data.cbSize         = sizeof(NOTIFYICONDATA);
    data.uFlags         = NIF_INFO;
    data.dwInfoFlags    = NIIF_INFO;

    ::Shell_NotifyIcon(NIM_ADD, &data);

    lstrcpyA(data.szInfoTitle, title);
    lstrcpyA(data.szInfo,      desc);

    ::Shell_NotifyIcon(NIM_MODIFY, &data);
    ::Shell_NotifyIcon(NIM_DELETE, &data);
}
