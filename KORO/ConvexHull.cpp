#include "ConvexHull.hpp"

#include <chrono>
#include <queue>

ConvexHull::ConvexHull(IDirect3DVertexBuffer9** const ppInVB)
    : vertices(), faces(), radius(0), center({0,0,0}), thread(), isThreadCompleted(false), isValid(false)
{
	this->GetVerticesFromVertexBuffer(ppInVB);

    std::thread tempThread(&ConvexHull::CreateConvexHull, this);
    this->thread.swap(tempThread);
}

ConvexHull::~ConvexHull()
{
    this->thread.join();
    //OutputDebugFormat("{} called.\n", __func__);
}

const std::vector<ConvexHull::Face>& ConvexHull::GetFaces() const
{
	return this->faces;
}

void ConvexHull::Render(Transform* parent) const
{
    //if (!this->isValid || !this->isThreadCompleted) return;

    //this->debugLine->BecomeParents(parent);
    //int i = 0;
    //for (auto& face : this->faces)
    //{
    //    this->debugLine->SetStartEnd(face.a, face.b);
    //    this->debugLine->Render();
    //    this->debugLine->SetStartEnd(face.b, face.c);
    //    this->debugLine->Render();
    //    this->debugLine->SetStartEnd(face.c, face.a);
    //    this->debugLine->Render();
    //}

    //this->debugLine->BreakParents();
}

bool ConvexHull::GetVerticesFromVertexBuffer(IDirect3DVertexBuffer9** const ppInVB)
{
    if (!ppInVB) return false;

    // 頂点バッファーの情報を取得
    D3DVERTEXBUFFER_DESC desc = {};
    (*ppInVB)->GetDesc(&desc);

    // 頂点データのレイアウト
    D3DVERTEXELEMENT9 elm[MAX_FVF_DECL_SIZE];
    D3DXDeclaratorFromFVF(desc.FVF, elm);

    // 座標情報(D3DFVF_XYZ)までのオフセット値 取得
    WORD offset = 0;
    for (unsigned i = 0; i < D3DXGetDeclLength(elm); ++i)
    {
        if (elm[i].Type == D3DDECLTYPE_FLOAT3 && elm[i].Usage == D3DDECLUSAGE_POSITION)
        {
            offset = elm[i].Offset;
            break;
        }
    }
    // FVFからCustomVertexのサイズを取得
    unsigned vertexSize = D3DXGetFVFVertexSize(desc.FVF);
    unsigned verticesSize = desc.Size;
    unsigned vertexNum = verticesSize / vertexSize;

    void* pbData;
    if (SUCCEEDED((*ppInVB)->Lock(0, verticesSize, &pbData, 0)))
    {
        // 領域確保
        this->vertices.reserve(vertexNum);

        D3DXVECTOR3 tempVertex;

        // ポインタ移動用
        BYTE* pb = reinterpret_cast<BYTE*>(pbData);

        for (unsigned i = 0; i < vertexNum; ++i)
        {
            // 頂点取得
            memcpy(&tempVertex, ((pb)+offset), sizeof(tempVertex));
            this->vertices.push_back(tempVertex);

            // ポインタを移動
            pb += vertexSize;
        }

        (*ppInVB)->Unlock();
    }
    else return false;

    return true;
}

bool ConvexHull::CreateConvexHull()
{
    if (this->vertices.size() < 4)
    {
        OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
        this->isThreadCompleted = true;
        return false;
    }

    auto start = std::chrono::system_clock::now();

    std::vector<D3DXVECTOR3> vertices(this->vertices);

    ///////////////////////////////////////////////////////////
    //// function objects

    // signed volume of tetraahedron
    auto CalcSignedTetrahedronVolume = [](const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c, const D3DXVECTOR3& d)
    {
        D3DXVECTOR3 ab = b - a;
        D3DXVECTOR3 ca = c - a;
        D3DXVECTOR3 ad = d - a;
        D3DXVECTOR3 cross(0, 0, 0);
        D3DXVec3Cross(&cross, &ab, &ca);

        return (/*abs*/(D3DXVec3Dot(&cross, &ad)) / 6.0f);
    };

    // remove points in tetrahedron
    auto RemovePointInsideTetrahedron = [&CalcSignedTetrahedronVolume](const std::vector<D3DXVECTOR3>& points, const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c, const D3DXVECTOR3& d)
    {
        std::vector<D3DXVECTOR3> newPoints;
        for (auto& point : points)
        {
            if (point == a || point == b || point == c || point == d) continue;
            float v  = abs( CalcSignedTetrahedronVolume(a, b, c, d) );
            float v1 = abs( CalcSignedTetrahedronVolume(a, b, c, point) );
            float v2 = abs( CalcSignedTetrahedronVolume(d, b, a, point) );
            float v3 = abs( CalcSignedTetrahedronVolume(d, c, b, point) );
            float v4 = abs( CalcSignedTetrahedronVolume(d, a, c, point) );

            if ((v1 + v2 + v3 + v4) - v > 0)
            {
                newPoints.push_back(point);
            }
        }
        return newPoints;
    };

    // find furthest point above triangler face
    // return : furthest point (if not found return { FLT_MAX, FLT_MAX, FLT_MAX } )
    auto CalcFurthestPoint = [&CalcSignedTetrahedronVolume](const std::vector<D3DXVECTOR3>& points, const Face& face)
    {
        //OutputDebugFormat("\n pointnum : {}", points.size());
        float maxSignedVolume = -FLT_EPSILON;
        D3DXVECTOR3 farPoint(0, 0, 0);
        for (auto& point : points)
        {
            if (point == face.a || point == face.b || point == face.c) continue;
            float signedVolume = CalcSignedTetrahedronVolume(face.a, face.b, face.c, point);

            if (signedVolume > maxSignedVolume)
            {
                maxSignedVolume = signedVolume;
                farPoint = point;
            }
        }

        if (maxSignedVolume < 0)
        {
            farPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
        }

        return farPoint;
    };

    // remove points under face
    auto RemovePointsUnderFace = [&CalcSignedTetrahedronVolume](const std::vector<D3DXVECTOR3>& points, const Face& face)
    {
        std::vector<D3DXVECTOR3> newPoints;
        for (auto& point : points)
        {
            if (point == face.a || point == face.b || point == face.c) continue;

            if (0 <= CalcSignedTetrahedronVolume(face.a, face.b, face.c, point))
                newPoints.push_back(point);
        }

        return newPoints;
    };


    ///////////////////////////////////////////////////////////
    //// first tetrahedron

    //// Find min and max point
    D3DXVECTOR3 min = vertices.front();
    D3DXVECTOR3 max = min;

    for (size_t i = 1; i < vertices.size(); ++i)
    {
        bool minConditions =
            vertices[i].x < min.x
            || ((vertices[i].x == min.x)
                && ((vertices[i].y < min.y)
                    || (vertices[i].y == min.y && vertices[i].z < min.z)));
        if (minConditions)
        {
            min = vertices[i];
        }

        bool maxConditions =
            vertices[i].x > max.x
            || ((vertices[i].x == max.x)
                && ((vertices[i].y > max.y)
                    || (vertices[i].y == max.y && vertices[i].z > max.z)));

        if (maxConditions)
        {
            max = vertices[i];
        }
    }

    //// Find furthest point from segment(min, max)
    float maxLenSq = FLT_MIN;
    D3DXVECTOR3 far1(0, 0, 0);
    for (auto& vertex : vertices)
    {
        D3DXVECTOR3 vec1 = min - max;
        D3DXVec3Normalize(&vec1, &vec1);
        D3DXVECTOR3 vec2 = vertex - max;
        float vec1Len = D3DXVec3Dot(&vec1, &vec2);
        float vec2LenSq = D3DXVec3LengthSq(&vec2);
        float lenSq = vec2LenSq - vec1Len * vec1Len;
        if (lenSq > maxLenSq)
        {
            maxLenSq = lenSq;
            far1 = vertex;
        }
    }

    //// Find furthest point from Triangle(min, max, far1)
    float maxSignedVolume = 0;
    D3DXVECTOR3 far2(1,0,0);
    for (auto& vertex : vertices)
    {
        float signedVolume = CalcSignedTetrahedronVolume(min, max, far1, vertex);
        if (abs(signedVolume) >= abs(maxSignedVolume))
        {
            maxSignedVolume = signedVolume;
            far2 = vertex;
        }
    }
    if (maxSignedVolume > 0)
    {
        std::swap(min, max);
    }

    // pending
    D3DXVECTOR3 overlapCheck[4] = { min, max, far1, far2 };
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == j)continue;
            if (overlapCheck[i] == overlapCheck[j])
            {
                OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
                this->isThreadCompleted = true;
                return false;
            }
        }
    }

    // Remove points in tetrahedron
    vertices = RemovePointInsideTetrahedron(vertices, min, max, far1, far2);

    this->faces.push_back({ min, max, far1 } );
    this->faces.push_back({ max, min, far2 } );
    this->faces.push_back({ far1, max, far2 });
    this->faces.push_back({ far1, far2, min });

    std::queue<Face> face_queue;
    face_queue.push({ min, max, far1 } );
    face_queue.push({ max, min, far2 } );
    face_queue.push({ far1, max, far2 });
    face_queue.push({ far1, far2, min });

    ///////////////////////////////////////////////////////////
    //// loop

    while (!vertices.empty() && !face_queue.empty())
    {
        // 10s elapsed
        if (10000 < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count())
        {
            OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
            this->isThreadCompleted = true;
            return false;
        }

        Face face = face_queue.front();
        face_queue.pop();

        std::vector<D3DXVECTOR3> upperSidePoints = RemovePointsUnderFace(vertices, face);
        auto furthest = CalcFurthestPoint(upperSidePoints, face);
        if (furthest == D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX)) continue;

        std::vector<Face> invisibleFaces;
        std::vector<Face> visibleFaces;

        for (auto& face : this->faces)
        {
            // 10s elapsed
            if (10000 < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count())
            {
                OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
                this->isThreadCompleted = true;
                return false;
            }

            float signedVolume = CalcSignedTetrahedronVolume(face.a, face.b, face.c, furthest);
            if (signedVolume <= 0)
            {
                invisibleFaces.push_back(face);
            }
            else
            {
                visibleFaces.push_back(face);
            }
        }

        this->faces = invisibleFaces;

        for (auto& visibleFace : visibleFaces)
        {
            // 10s elapsed
            if (10000 < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count())
            {
                OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
                this->isThreadCompleted = true;
                return false;
            }

            // remove inner points
            vertices = RemovePointInsideTetrahedron(vertices, visibleFace.a, visibleFace.b, visibleFace.c, furthest);

            // create new faces
            for (auto& invisibleFace : invisibleFaces)
            {
                // 10s elapsed
                if (10000 < std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count())
                {
                    OutputDebugFormat("\n\n ************* CreateConvexHull ERROR *************\n\n");
                    this->isThreadCompleted = true;
                    return false;
                }

                auto [isSharing, shareP1, shareP2] = visibleFace.IsShareEdge(invisibleFace, true);
                if (isSharing)
                {
                    D3DXVECTOR3 v1 = furthest - shareP1;
                    D3DXVECTOR3 v2 = furthest - shareP2;

                    D3DXVec3Normalize(&v1, &v1);
                    D3DXVec3Normalize(&v2, &v2);

                    if (1 - abs(D3DXVec3Dot(&v1, &v2)) <= FLT_EPSILON) continue;

                    Face newFace = { shareP1, shareP2, furthest };
                    this->faces.push_back(newFace);
                    face_queue.push(newFace);
                }
            }
        }
    }

    OutputDebugFormat("\n elapsed : {} ms.", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count());
    OutputDebugString("\n CreateConvexHull is Complete.\n\n");

    //////////////////////////////////////////////////
    // 簡易半径を求める

    std::vector<D3DXVECTOR3> tempVerts;
    maxLenSq = FLT_MIN;
    D3DXVECTOR3 p1, p2;

    auto MostDistPair = [&tempVerts, &maxLenSq, &p1, &p2](const D3DXVECTOR3& vertex)
    {
        if(!std::any_of(tempVerts.begin(), tempVerts.end(), [&vertex](const auto& v) { return v == vertex; }))
        {
            for (auto& v : tempVerts)
            {
                D3DXVECTOR3 d = v - vertex;
                float lenSq = D3DXVec3LengthSq(&d);
                if (lenSq > maxLenSq)
                {
                    maxLenSq = lenSq;
                    p1 = vertex;
                    p2 = v;
                }
            }
            tempVerts.push_back(vertex);
        }
    };

    for (auto& face : this->faces)
    {
        MostDistPair(face.a);
        MostDistPair(face.b);
        MostDistPair(face.c);
    }

    D3DXVECTOR3 radiusDirection = (p2 - p1) * 0.5f;
    this->radius = D3DXVec3Length(&radiusDirection);
    this->center = p1 + radiusDirection;

    ////////////////////////////////////////////////////
    //// 各面法線
    //for (auto& face : this->faces)
    //{

    //}

    // 完了
    this->isThreadCompleted = true;
    this->isValid = true;

    return true;
}