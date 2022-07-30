#include "FontGui.hpp"
#pragma once

#ifdef _DEBUG
    #define IS_DEBUG true
#else
    #define IS_DEBUG false
#endif

#define IF_DEBUG   if ( IS_DEBUG)
#define IF_RELEASE if (!IS_DEBUG)

namespace Debug
{
    inline FontGui* instance;
    inline string   text;

    // 開始する
    inline void Start()
    {
        Debug::instance = new FontGui("", { 2, 2 }, 24, { 1.0f, 1.0f, 1.0f, 1.0f }, TextAlign::LEFT, true, "Consolas");
        Debug::instance->z_index   = 0U;
        Debug::instance->multiLine = true;
    }

    // デバッグ出力の行テキストを追加
    template <class... Args>
    inline void AddLine(const string format, Args... args)
    {
        IF_DEBUG Debug::text += std::format(format + "\n", args...);
    }

    // 出力する
    static void PrintFont()
    {
        IF_DEBUG
        {
            if (Debug::instance)
            {
                Debug::instance->SetText(Debug::text);
            }

            // クリア
            Debug::text = "";
        }
    }
}
