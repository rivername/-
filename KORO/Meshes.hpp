#pragma once

#include <map>
#include <d3dx9.h>
#include "utils.hpp"

/*********************************************
メッシュ新規追加するときは、
1. XMeshType に定数を追加、
2. meshMap に追加した定数に対応するフォルダパスを追加
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

// type に対応する xファイルのパス
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
	// xファイルからメッシュを作成
	void CreateXFileMeshes();

	// cubeのメッシュをプログラムで作成
	void CreateCubeMesh();

	// xメッシュの凸包を生成
	void CreateConvexHulls();

public:
	// 指定したXMeshTypeのメッシュを描画
	void RenderMeshByType(XMeshType type);

	bool IsCompleted() const;

private:
	// key : XMeshType,   value : {マテリアル,マテリアル数,メッシュ}
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
	// ファイルパスからメッシュを生成し、描画する
	// 既に生成済みの場合は、描画のみ
	bool RenderMesh(const char* path);

	const ConvexHull* GetConvexHull(const char* path) const;

private:
	// 生成に失敗しているか
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