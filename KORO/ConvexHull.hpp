#pragma once

#include <thread>
#include <vector>

#include "const.hpp"
#include "utils.hpp"

class Transform;

class ConvexHull
{
public:
	ConvexHull(IDirect3DVertexBuffer9** const ppInVB);
	~ConvexHull();

public:
	// 三角形の面
	struct Face
	{
		// 3頂点 時計回り
		D3DXVECTOR3 a, b, c;

		// 法線
		D3DXVECTOR3 GetNormal() const
		{
			static D3DXVECTOR3 normal(0,0,0);
			if (normal = D3DXVECTOR3(0, 0, 0))
			{
				D3DXVECTOR3 ab = b - a;
				D3DXVECTOR3 ca = c - a;
				D3DXVECTOR3 cross(0, 0, 0);
				D3DXVec3Cross(&cross, &ab, &ca);
				D3DXVec3Normalize(&normal, &cross);
			}
			return normal;
		}

		// return :
		// 1. sharing?
		// 2. sharing point 1
		// 3. sharing point 2
		std::tuple<bool, D3DXVECTOR3, D3DXVECTOR3> IsShareEdge(const Face& face, bool temp) const
		{
			if (this->a == face.a)
			{
				if (this->b == face.b) return {true, this->a, this->b };
				else if (this->b == face.c) return {true, this->a, this->b };
				else if (this->c == face.b) return {true, this->c, this->a };
				else if (this->c == face.c) return {true, this->c, this->a };
			}
			else if (this->a == face.b)
			{
				if (this->b == face.a) return {true, this->a, this->b };
				else if (this->b == face.c) return {true, this->a, this->b };
				else if (this->c == face.a) return {true, this->c, this->a };
				else if (this->c == face.c) return {true, this->c, this->a };
			}
			else if (this->a == face.c)
			{
				if (this->b == face.b) return {true, this->a, this->b };
				else if (this->b == face.a) return {true, this->a, this->b };
				else if (this->c == face.b) return {true, this->c, this->a };
				else if (this->c == face.a) return {true, this->c, this->a };
			}
			else if(this->c == face.a || this->c == face.b || this->c == face.c)
			{
				if (this->b == face.a)      return {true, this->b, this->c };
				else if (this->b == face.b) return {true, this->b, this->c };
				else if (this->b == face.c) return {true, this->b, this->c };
			}

			return { false, {0,0,0}, {0,0,0} };
		}
	};

public:

	const std::vector<ConvexHull::Face>& GetFaces() const;

	void Render(Transform* parent) const;

	const bool& IsCompleted() const
	{
		return this->isThreadCompleted;
	}

	const bool& IsValid() const
	{
		return this->isValid;
	}


	float GetRadius() const { return this->radius; }
	D3DXVECTOR3 GetCenter() const { return this->center; }

private:

	bool GetVerticesFromVertexBuffer(IDirect3DVertexBuffer9** const ppInVB);

	bool CreateConvexHull();

private:

	std::vector<D3DXVECTOR3> vertices;
	std::vector<Face> faces;

	// 簡易判定用
	float radius;
	D3DXVECTOR3 center;

	std::thread thread;
	bool isThreadCompleted;
	bool isValid;
};