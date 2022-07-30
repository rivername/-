#include "Meshes.hpp"

#include "DX9.hpp"
#include "ConvexHull.hpp"

Meshes::Meshes() : isThreadCompleted(false)
{
    Meshes::instance.reset(this);

    //this->CreateXFileMeshes();
    this->CreateCubeMesh();

    //this->CreateConvexHulls();
}

Meshes::~Meshes()
{
	SAFE_RELEASE(cubeMeshInfo.first);

	//for (auto& xMesh : this->xfileMeshes)
	//{
	//	auto [material, num, mesh] = xMesh.second;
	//	SAFE_RELEASE(material);
	//	SAFE_RELEASE(mesh);
	//}

    for (auto& xmesh : this->createdMeshes)
    {
        SAFE_RELEASE(xmesh.second.mesh);
        SAFE_RELEASE(xmesh.second.materials);
    }

    OutputDebugFormat("\n {} called. \n", __func__);
}

void Meshes::CreateXFileMeshes()
{
	for (auto& meshInfo : meshMap)
	{
		auto& [type, path] = meshInfo;
        ID3DXBuffer* tempMaterials;
        DWORD tempNum;
        ID3DXMesh* tempMesh;
		auto hr = D3DXLoadMeshFromX
		(
			path,
			D3DXMESH_MANAGED,
			DX9::instance->device3d,
			NULL,
			&tempMaterials,
			NULL,
			&tempNum,
			&tempMesh
		);
        if (hr == S_OK)
        {
            auto& [materials, num, mesh] = this->xfileMeshes[type];
            materials = tempMaterials;
            num = tempNum;
            mesh = tempMesh;
        }

	}
}

void Meshes::CreateCubeMesh()
{
    // ���_
    CustomVertex_xyz_normal_tex1 vertices[] =
    {
        // front
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },

        // back
        { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f} },
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f} },

        // right
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },

        // left
        { {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },

        // top
        { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },

        // bottom
        { {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} }
    };

    // �C���f�b�N�X
    WORD indices[] =
    {
        // front
        0,  1,  2,
        2,  1,  3,

        // back
        4,  5,  6,
        6,  5,  7,

        // right
        8,  9, 10,
        10, 9, 11,

        // left
        12, 13, 14,
        14, 13, 15,

        // top
        16, 17, 18,
        18, 17, 19,

        // bottom
        20, 21, 22,
        22, 21, 23
    };

    // ����
    DWORD attributes[] =
    {
        0, 0, 1, 1, 2, 2,
        3, 3, 4, 4, 5, 5
    };

    auto& [mesh, num] = cubeMeshInfo;

    // ���b�V������
    D3DXCreateMeshFVF(ARRAYSIZE(indices) / 3, ARRAYSIZE(vertices), D3DXMESH_MANAGED, CustomVertex_xyz_normal_tex1::FVF, DX9::instance->device3d, &mesh);

    // ���_�o�b�t�@��ݒ�
    void* tempVB;
    if (SUCCEEDED(mesh->LockVertexBuffer(0, &tempVB)))
    {
        memcpy(tempVB, vertices, sizeof(vertices));
        mesh->UnlockVertexBuffer();
    }

    // �C���f�b�N�X�o�b�t�@��ݒ�
    void* tempIB;
    if (SUCCEEDED(mesh->LockIndexBuffer(0, &tempIB)))
    {
        memcpy(tempIB, indices, sizeof(indices));
        mesh->UnlockIndexBuffer();
    }

    // �����o�b�t�@��ݒ�
    DWORD* tempAB;
    if (SUCCEEDED(mesh->LockAttributeBuffer(0, &tempAB)))
    {
        memcpy(tempAB, attributes, sizeof(attributes));
        mesh->UnlockAttributeBuffer();
    }

    // �אڃ��X�g
    std::vector<DWORD> adjacentList;
    adjacentList.resize(mesh->GetNumFaces() * 3);
    mesh->GenerateAdjacency(FLT_EPSILON, &adjacentList[0]);
    // �œK�� DrawSubset�̃p�t�H�[�}���X����
    mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, &adjacentList[0], nullptr, nullptr, nullptr);

    // subset�̐��擾
    mesh->GetAttributeTable(NULL, &num);
}

void Meshes::CreateConvexHulls()
{
    //for (auto& xmeshes : this->xfileMeshes)
    //{
    //    auto& [mat, num, mesh] = xmeshes.second;

    //    if (!mesh) continue;

    //    IDirect3DVertexBuffer9* vb;
    //    mesh->GetVertexBuffer(&vb);
    //    this->xmeshesConvexHull[xmeshes.first] = std::make_unique<ConvexHull>(&vb);
    //}
}

void Meshes::RenderMeshByType(XMeshType type)
{
    if (this->xfileMeshes.contains(type))
    {
        auto& [materials, num, mesh] = this->xfileMeshes[type];

        for (DWORD i = 0; i < num; ++i)
        {
            D3DXMATERIAL mat = ((D3DXMATERIAL*)(materials->GetBufferPointer()))[i];
            DX9::instance->device3d->SetMaterial(&(mat.MatD3D));
            mesh->DrawSubset(i);
        }
    }
}

bool Meshes::IsCompleted() const
{
    bool result = true;
    for (auto& convexHull : this->xmeshesConvexHull)
    {
        result &= convexHull.second->IsCompleted();
    }

    return result;
}

bool Meshes::RenderMesh(const char* path)
{
    // ���ɐ����Ɏ��s���Ă����疳��
    if (this->IsFailed(path)) return false;

    // ��������Ă��Ȃ��ꍇ�͐�������
    if (!this->createdMeshes.contains(path))
    {
        if (!this->CreateMesh(path)) return false;
    }

    // �`��
    auto& xmesh = this->createdMeshes.at(path);
    for (DWORD i = 0; i < xmesh.numMaterials; ++i)
    {
        D3DXMATERIAL mat = ((D3DXMATERIAL*)(xmesh.materials->GetBufferPointer()))[i];
        DX9::instance->device3d->SetMaterial(&(mat.MatD3D));
        xmesh.mesh->DrawSubset(i);
    }

    return true;
}

const ConvexHull* Meshes::GetConvexHull(const char* path) const
{
    // ���ɐ����Ɏ��s���Ă����疳��
    if (this->IsFailed(path)) return nullptr;

    // ��������Ă��Ȃ��ꍇ��nullptr
    if (!this->createdMeshes.contains(path))
    {
        return nullptr;
    }

    ConvexHull* convexHull = this->createdConvexhulls.at(path).get();

    // �ʕ�̐����^�X�N���I�����Ă�����Ԃ�
    if (convexHull->IsCompleted() && convexHull->IsValid())
    {
        return convexHull;
    }

    return nullptr;
}

bool Meshes::IsFailed(const char* path) const
{
    for (auto& failedPath : this->failedPaths)
    {
        if (failedPath == path) return true;
    }
    return false;
}

bool Meshes::CreateMesh(const char* path)
{
    XMesh tempXMesh = {};
    HRESULT hr = D3DXLoadMeshFromX
    (
        path,
        D3DXMESH_MANAGED,
        DX9::instance->device3d,
        NULL,
        &tempXMesh.materials,
        NULL,
        &tempXMesh.numMaterials,
        &tempXMesh.mesh
    );

    if (SUCCEEDED(hr))
    {
        // �����ς݃}�b�v�ɓo�^
        this->createdMeshes.emplace(path, tempXMesh);

        // �ʕ�𐶐�
        IDirect3DVertexBuffer9* vb;
        tempXMesh.mesh->GetVertexBuffer(&vb);
        this->createdConvexhulls.emplace(path, std::make_unique<ConvexHull>(&vb));
    }
    else
    {
        // �������s�z��ɓo�^
        this->failedPaths.push_back(path);
        return false;
    }

    return true;
}