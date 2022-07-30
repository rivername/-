#include "SkyBox.hpp"
#include "HLSL.hpp"
#include <map>
#pragma once

/*==================================
            パーティクル名
==================================*/
enum class ParticleName
{
    PAPER_SHOWER
};

/*==================================
           パーティクルタイプ
==================================*/
enum class ParticleType
{
    DEFAULT
};

/*==================================
      Vertexテンプレート：Particle
==================================*/
const VertexPositionNormalTexture VERTICES_BLOCK_PARTICLE[4] =
{
    VERTICES_BLOCK_SKYBOX[0],
    VERTICES_BLOCK_SKYBOX[1],
    VERTICES_BLOCK_SKYBOX[2],
    VERTICES_BLOCK_SKYBOX[3],
};

/*==================================
     ParticleTypeとファイル名マップ
==================================*/
inline std::map<ParticleName, const char*> PE_NAME_WITH_FILENAME =
{
    { ParticleName::PAPER_SHOWER, "resources/textures/particle/paper-shower.png" }
};

/*==================================
      ParticleTypeとサイズマップ
==================================*/
inline std::map<ParticleName, POINT> PE_NAME_WITH_TEX_SIZE =
{
    { ParticleName::PAPER_SHOWER, POINT(10, 1) }
};

/*==================================
     ParticleTypeとKillY座標マップ
==================================*/
inline std::map<ParticleName, float> PE_NAME_WITH_KILL_Y =
{
    { ParticleName::PAPER_SHOWER, -20.0f }
};

/*==================================
              Particle
==================================*/
class Particle : public HLSLObject, public GameObject
{
public:
    // すべてのパーティクル
    inline static std::vector<Particle*> all_particles;

    // すべてのパーティクルを解放
    static void ReleaseAll();

public:
    // パーティクル
    // @param count : パーティクルの枚数
    // @param type  : パーティクルタイプ
    Particle(size_t count, ParticleName type);

    // パーティクル
    // @param filename     : ファイル名
    // @param particleType : パーティクルタイプ
    Particle(const char* filename, ParticleType particleType);

    ~Particle();

public:
    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

    // 解放
    bool Release();

public:
    // 位置と回転の設定
    void SetLocationAndRotation();

    // 重力の適用
    void ApplyGravity(float deltaTime);

    // 召喚する
    void Summon(D3DXVECTOR3 spawnLocation);

    // 値をリセット
    void ResetValue();

private:
    std::vector<IDirect3DVertexBuffer9*> vertex_buffers; // Vertexバッファリスト
    std::vector<IDirect3DIndexBuffer9*>  index_buffers;  // Indexバッファリスト

    std::vector<D3DXVECTOR3> locations; // 位置リスト
    std::vector<D3DXVECTOR3> rotations; // 回転リスト

    std::vector<float>       accelerationsXZ; // XZ方向の加速度リスト
    std::vector<float>       accelerationsY;  // Y方向の加速度リスト

    IDirect3DTexture9*       texture; // テクスチャ

private:
    // パーティクルの枚数
    size_t count;

    // パーティクルタイプ
    ParticleName type;
    
    // 召喚位置
    D3DXVECTOR3 spawnLocation;

    // 経過Tick
    UINT elapsedTick;

    // 解放された
    bool released;

public:
    virtual void EventBeginPlay() override;
    virtual void EventRender() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;
};
