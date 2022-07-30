#include "HLSL.hpp"
#pragma once

/*==================================
         頂点（位置、法線、UV）
==================================*/
struct VertexPositionNormalTexture
{
    float  x,  y,  z; // Position
    float nx, ny, nz; // Normals
    float  u,  v;     // Texture Coords

    enum
    {
        // 頂点の情報 // 位置 / 法線 / UV
        FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
    };
};

/*==================================
             SkyBoxサイズ
==================================*/
const float SKYBOX_SIZE = 512.0f;

/*==================================
             UV SkyBox
==================================*/
#define UV_SKYBOX(x, y) 0.5f * (float)x, 0.5f * (float)y

/*==================================
    Vertexテンプレート（SkyBox用）
==================================*/
const VertexPositionNormalTexture VERTICES_BLOCK_SKYBOX[24] =
{
    //                         |        頂点座標       |         法線座標        |     テクスチャ座標(UV)  |
    // Front                        X      Y      Z        NX     NY     NZ                   U  V
    VertexPositionNormalTexture( 0.5f,  0.5f, -0.5f,     0.0f,  0.0f,  1.0f,        UV_SKYBOX(0, 1)),
    VertexPositionNormalTexture(-0.5f,  0.5f, -0.5f,     0.0f,  0.0f,  1.0f,        UV_SKYBOX(1, 1)),
    VertexPositionNormalTexture( 0.5f, -0.5f, -0.5f,     0.0f,  0.0f,  1.0f,        UV_SKYBOX(0, 2)),
    VertexPositionNormalTexture(-0.5f, -0.5f, -0.5f,     0.0f,  0.0f,  1.0f,        UV_SKYBOX(1, 2)),
    
    // Back
    VertexPositionNormalTexture(-0.5f,  0.5f,  0.5f,     0.0f,  0.0f, -1.0f,        UV_SKYBOX(0, 1)),
    VertexPositionNormalTexture( 0.5f,  0.5f,  0.5f,     0.0f,  0.0f, -1.0f,        UV_SKYBOX(1, 1)),
    VertexPositionNormalTexture(-0.5f, -0.5f,  0.5f,     0.0f,  0.0f, -1.0f,        UV_SKYBOX(0, 2)),
    VertexPositionNormalTexture( 0.5f, -0.5f,  0.5f,     0.0f,  0.0f, -1.0f,        UV_SKYBOX(1, 2)),
    
    // Left
    VertexPositionNormalTexture( 0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,        UV_SKYBOX(0, 1)),
    VertexPositionNormalTexture( 0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,        UV_SKYBOX(1, 1)),
    VertexPositionNormalTexture( 0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,        UV_SKYBOX(0, 2)),
    VertexPositionNormalTexture( 0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,        UV_SKYBOX(1, 2)),
    
    // Right
    VertexPositionNormalTexture(-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,        UV_SKYBOX(0, 1)),
    VertexPositionNormalTexture(-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,        UV_SKYBOX(1, 1)),
    VertexPositionNormalTexture(-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,        UV_SKYBOX(0, 2)),
    VertexPositionNormalTexture(-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,        UV_SKYBOX(1, 2)),
    
    
    // Top
    VertexPositionNormalTexture( 0.5f,  0.5f,  0.5f,     0.0f, -1.0f,  0.0f,        UV_SKYBOX(1, 0)),
    VertexPositionNormalTexture(-0.5f,  0.5f,  0.5f,     0.0f, -1.0f,  0.0f,        UV_SKYBOX(2, 0)),
    VertexPositionNormalTexture( 0.5f,  0.5f, -0.5f,     0.0f, -1.0f,  0.0f,        UV_SKYBOX(1, 1)),
    VertexPositionNormalTexture(-0.5f,  0.5f, -0.5f,     0.0f, -1.0f,  0.0f,        UV_SKYBOX(2, 1)),
    
    // Bottom
    VertexPositionNormalTexture(-0.5f, -0.5f,  0.5f,     0.0f,  1.0f,  0.0f,        UV_SKYBOX(0, 0)),
    VertexPositionNormalTexture( 0.5f, -0.5f,  0.5f,     0.0f,  1.0f,  0.0f,        UV_SKYBOX(1, 0)),
    VertexPositionNormalTexture(-0.5f, -0.5f, -0.5f,     0.0f,  1.0f,  0.0f,        UV_SKYBOX(0, 1)),
    VertexPositionNormalTexture( 0.5f, -0.5f, -0.5f,     0.0f,  1.0f,  0.0f,        UV_SKYBOX(1, 1))
};

/*==================================
       Indexテンプレート（SkyBox用）
                            (16bit)
==================================*/
const WORD INDICES_BLOCK_SKYBOX[6] =
{
    0, 1, 2,
    2, 1, 3
};

/*==================================
               SkyBox
==================================*/
class SkyBox : public HLSLObject, public GameObject
{
public:
    SkyBox(int resourceID);
    ~SkyBox();

public:
    bool Init();

public:
    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect)   override;

public:
    // オフセット
    D3DXVECTOR3 offset;

    // 回転
    Rotation rotation;

    // サイズに足す値
    float sizeAddValue;

private:
    // リソースID
    int resourceID;

    // バッファ
    IDirect3DVertexBuffer9* vertex_buffer;
    IDirect3DIndexBuffer9*  index_buffer;

public:
    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;
};
