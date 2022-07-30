#pragma once

// 参考
// http://marupeke296.com/DXG_No54_StencilClipping.html
// https://yttm-work.jp/gmpg/gmpg_0021.html

/*==================================
          ステンシルハイライト
対象のオブジェクトが他のオブジェクトによって隠れた時にその隠れた部分をハイライトする
==================================*/
namespace StencilHighlight
{
    inline IDirect3DDevice9* __device3d;

    // ~ 1. 初期化 ~
    // ステンシルバッファをクリアしZテスト値を設定
    inline void Fun_1_Init(IDirect3DDevice9* device3d)
    {
        StencilHighlight::__device3d = device3d;

        StencilHighlight::__device3d->Clear(0, 0, D3DCLEAR_STENCIL, 0x00, 1.0f, 0x00);

        StencilHighlight::__device3d->SetRenderState(D3DRS_ZENABLE, true);
        StencilHighlight::__device3d->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILENABLE, true);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILMASK, 0xFF);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    }

    // ~ 2. ハイライトするオブジェクトよりも後ろにあるオブジェクトの描画開始 ~
    // 以降に描画処理を記述
    inline void Fun_2_StartRenderingBackgroundObject()
    {
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILREF, 1L);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);
    }

    // ~ 3. ハイライトするオブジェクトの描画を開始 ~
    // 以降に描画処理を記述
    inline void Fun_3_StartRenderingHighlightTarget()
    {
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILREF, 2L);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);
    }

    // ~ 4. ハイライト部分の描画を開始 ~
    // 以降にハイライトするオブジェクトよりも手前にあるオブジェクトの描画処理を記述
    // （描画時に色を変えるなどしてハイライトさせる）
    inline void Fun_4_StartRenderingHighlight()
    {
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILREF, 2L);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    }

    // ~ 5. ハイライトするオブジェクトよりも手前にあるオブジェクトの描画を開始 ~
    // 以降にハイライトするオブジェクトよりも手前にあるオブジェクトの描画処理を記述
    inline void Fun_5_StartRenderingFrontObject()
    {
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILREF, 1L);
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    }

    // ~ 6. 終了処理 ~
    inline void Fun_6_EndRendering()
    {
        StencilHighlight::__device3d->SetRenderState(D3DRS_STENCILENABLE, false);
    }
}
