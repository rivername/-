#include "FontGui.hpp"
#pragma once

/*==================================
             フォント管理
==================================*/
namespace FontManager
{
    // リソース管理枠
    namespace ResourceManager
    {
        // 読み込んだフォントリソース
        inline std::vector<HANDLE> fonts;

        // リソースIDからフォントファイルを読み込み
        void LoadFontFromResource(int resourceID);

        // 読み込んだすべてのフォントリソースを解放
        void ReleaseAllFontResource();
    }

    // 描画管理枠
    // 用開放のオブジェクトを所持:
    // [ 頂点バッファ, テクスチャ ]
    namespace RenderManager
    {
        // 読み込んだ < 文字コード, フォントサイズ + FontRenderItem >
        inline std::vector<std::tuple<UINT, LONG, FontRenderItem>> loadedRenderItemsEx;

        // FontGuiオブジェクトに文字列を描画するRenderItemを渡す
        void OutputRenderItemsOfString(LPCSTR text, FontGui* pOut);

        // 既に読み込まれた (文字コード && フォントサイズ) かどうか
        // 読み込まれていたらFontRenderItemを出力
        bool IsTheCodeAlreadyLoaded(UINT code, LONG font_size, FontRenderItem* itemIfCodeLoadedIn);

        // 読み込んだすべての描画アイテムを解放
        void ReleaseAllRenderItems();
    }
}
