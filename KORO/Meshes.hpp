#pragma once

#include <map>
#include <d3dx9.h>
#include "utils.hpp"

/*********************************************
���b�V���V�K�ǉ�����Ƃ��́A
1. XMeshType �ɒ萔��ǉ��A
2. meshMap �ɒǉ������萔�ɑΉ�����t�H���_�p�X��ǉ�
*********************************************/

enum class XMeshType
{
	KORO,
	TREE,
	ARROW,
	PLATE,
	YOKOHAMA_ARENA,
	YOKOARIKUN_LOCOMOTION,

	NONE
};

// type �ɑΉ����� x�t�@�C���̃p�X
inline const std::map<XMeshType, const char*> meshMap
{
	{XMeshType::KORO, "./resources/x/koro_kari.x"},
	{XMeshType::TREE, "./resources/x/tree.x"},
	{XMeshType::ARROW, "./resources/x/arrow.x"},
	{XMeshType::PLATE, "./resources/x/plate.x"},
	{XMeshType::YOKOHAMA_ARENA, "./resources/x/YokohamaArena.x"},
	{XMeshType::YOKOARIKUN_LOCOMOTION, "./resources/x/Yokoari_Locomotion_Pose.x"}
};

class ConvexHull;

class Meshes
{
public:
	static inline std::unique_ptr<Meshes> instance;

public:
	Meshes();
	~Meshes();

private:
	// x�t�@�C�����烁�b�V�����쐬
	void CreateXFileMeshes();

	// cube�̃��b�V�����v���O�����ō쐬
	void CreateCubeMesh();

	// x���b�V���̓ʕ�𐶐�
	void CreateConvexHulls();

public:
	// �w�肵��XMeshType�̃��b�V����`��
	void RenderMeshByType(XMeshType type);

	bool IsCompleted() const;

private:
	// key : XMeshType,   value : {�}�e���A��,�}�e���A����,���b�V��}
	std::map< XMeshType, std::tuple<ID3DXBuffer*, DWORD, ID3DXMesh*> > xfileMeshes;

public:
	// key : XMeshType,   value : unique_ptr<ConvexHull>
	std::map< XMeshType, std::unique_ptr<ConvexHull> > xmeshesConvexHull;

	// [mesh, num_subset]
	std::pair<ID3DXMesh*, DWORD> cubeMeshInfo;

private:

	bool isThreadCompleted;

	/////////////////////

public:
	// �t�@�C���p�X���烁�b�V���𐶐����A�`�悷��
	// ���ɐ����ς݂̏ꍇ�́A�`��̂�
	bool RenderMesh(const char* path);

	const ConvexHull* GetConvexHull(const char* path) const;

private:
	// �����Ɏ��s���Ă��邩
	bool IsFailed(const char* path) const;

	bool CreateMesh(const char* path);

	struct XMesh
	{
		ID3DXMesh*   mesh;
		ID3DXBuffer* materials;
		DWORD        numMaterials;
	};

	std::vector<const char*> failedPaths;

	std::map<const char*, XMesh> createdMeshes;

	std::map<const char*, std::unique_ptr<ConvexHull>> createdConvexhulls;


};

struct CustomVertex_xyz_normal_tex1
{
	D3DXVECTOR3 location;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;

	static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_TEX1);
};