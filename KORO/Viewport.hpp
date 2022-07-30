#pragma once
#include <functional>
#include "DX9.hpp"

namespace Viewport
{
	// 新しいビューポートで描画
	// args :
	//	1. renderProcess 描画処理の関数オブジェクト
	//	2. originX ビューポートの左上 x 座標
	//	3. originY ビューポートの左上 y 座標
	//	4. width   ビューポートの幅
	//	5. height  ビューポートの高さ
	//	6. minZ    描画深度の最小値
	//	6. maxZ    描画深度の最大値
	inline void Render
	(
		std::function<void(void)> renderProcess,
		DWORD originX,  DWORD originY,
		DWORD width,    DWORD height,
		float minZ = 0, float maxZ = 1
	)
	{
		D3DVIEWPORT9 viewport =
		{
			.X     = originX, .Y      = originY,
			.Width = width,   .Height = height,
			.MinZ  = minZ,    .MaxZ   = maxZ
		};

		// 変更前ビューポート取得
		D3DVIEWPORT9 preViewport;
		DX9::instance->device3d->GetViewport(&preViewport);

		// ビューポート変更
		DX9::instance->device3d->SetViewport(&viewport);

		// 描画
		renderProcess();

		// ビューポートを変更前に戻す
		DX9::instance->device3d->SetViewport(&preViewport);
	}
};