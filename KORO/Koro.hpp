#include "const.hpp"
#include "utils.hpp"

#include "DX9.hpp"
#include "GameObject.hpp"
#include "Gravity.hpp"

#include "HLSL.hpp"

#pragma once

enum class TextureResource;

class Koro : public HLSLObject, public GameObject, public Transform
{
public:
    inline static Koro* instance;

public:
    Koro();
    ~Koro();

    bool Init();

public:
    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

public:
    // サイズ関係1a
    float GetRadius() const { return this->radius; }
    void SetRadius(float radius) { this->radius = radius; }

    // 速度関係
    D3DXVECTOR3 GetSpeed() { return this->speed; }
    void SetSpeed(D3DXVECTOR3 speed) { this->speed = speed; }
    void SetSpeedX(float speedx) { this->speed.x = speedx; }
    void SetSpeedY(float speedy) { this->speed.y = speedy; }
    void SetSpeedZ(float speedz) { this->speed.z = speedz; }
    void AddSpeed(D3DXVECTOR3 speed) { this->speed += speed; }

private:

    // 箱当たり判定
    void CollisionCube();

    // ゴール判定
    void HitGoal();

public:
    bool Render();

    bool enableMove;				// 動かすフラグ
private:


    float radius;		// 半径
    D3DXVECTOR3 speed;	// 速度

    float decay;		// 減衰
    float weight;		// 重量
    float resistance;	// 反発

    ID3DXBuffer* materials;
    DWORD numMaterials;
    ID3DXMesh* mesh;

public:
    // 位置リセット
    void ResetLocation();

private:
    virtual void EventRender() override;

    virtual void EventBeginPlay() override;

    virtual void EventTick(float deltaTime) override;

    virtual void EventKeyDown(KeyEvent e) override;

    virtual void EventKeyUp(KeyEvent e) override;

    virtual void EventMouseDown(MouseEvent e) override;

    virtual void EventMouseUp(MouseEvent e) override;

private:
    friend class KoroMask;
};
