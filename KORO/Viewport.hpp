#pragma once
#include <functional>
#include "DX9.hpp"

namespace Viewport
{
	// �V�����r���[�|�[�g�ŕ`��
	// args :
	//	1. renderProcess �`�揈���̊֐��I�u�W�F�N�g
	//	2. originX �r���[�|�[�g�̍��� x ���W
	//	3. originY �r���[�|�[�g�̍��� y ���W
	//	4. width   �r���[�|�[�g�̕�
	//	5. height  �r���[�|�[�g�̍���
	//	6. minZ    �`��[�x�̍ŏ��l
	//	6. maxZ    �`��[�x�̍ő�l
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

		// �ύX�O�r���[�|�[�g�擾
		D3DVIEWPORT9 preViewport;
		DX9::instance->device3d->GetViewport(&preViewport);

		// �r���[�|�[�g�ύX
		DX9::instance->device3d->SetViewport(&viewport);

		// �`��
		renderProcess();

		// �r���[�|�[�g��ύX�O�ɖ߂�
		DX9::instance->device3d->SetViewport(&preViewport);
	}
};