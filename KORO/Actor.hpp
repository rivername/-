#include "GameObject.hpp"
#include "Debug.hpp"
#include "Texture.hpp"
#include "ActorManager.hpp"
#pragma once

class GoalFlag;

// 保存用構造体
struct ActorSaveData
{
	char          id[32];      // ID
	KoroActorType actorType;   // Actorタイプ
	D3DXMATRIX    worldMatrix; // ワールド行列
};

class Actor : public HLSLObject, public GameObject, public Transform
{
protected:
	//////////////////////////////////////////////////
	// ctor

	Actor();
	Actor(const D3DXMATRIX* const matrix, Transform* parent, const char* id = "");

public:
	//////////////////////////////////////////////////
	// dtor
	virtual ~Actor();

public:
	//////////////////////////////////////////////////
	// 描画系

	// 当たり判定の描画
	virtual bool RenderCollision() { return true; }

	//virtual bool RenderObjectJudge();


public:
	//////////////////////////////////////////////////
	// クリック判定用

	static DWORD CreateUniqueNum()
	{
		static DWORD uid = 0;
		return ++uid;
	}

	// クリック判定色を取得
	const DWORD& GetJudgeColor() const;

	const float* GetJudgeColorFloat4() const;

public:
	//////////////////////////////////////////////////
	// その他

	// 複製関数
	//virtual Actor* CreateClone() = 0;

	// Koro(球)との衝突判定
	// デフォルトはOBB判定、その他は派生クラスで再定義させる
	// args :
	//     1 : Koro中心
	//     2 : Koro半径
	// returns :
	//     1 : 法線
	//     2 : 埋まった距離
	virtual std::tuple<D3DXVECTOR3, float> CollisionWithKoro(D3DXVECTOR3 center, float radius);

	// 大雑把な包含球の中心と半径を取得 (world)
	// デフォルト半径は0.5f
	virtual std::pair<D3DXVECTOR3, float> GetWorldRoughSphere();

	virtual KoroActorType GetActorType() const = 0;

public:
	//////////////////////////////////////////////////
	// フラグ類

	// 無視するか
	bool isIgnore;

	// 当たり判定有効
	bool enableCollision;

	// 可視
	bool visible;

private:
	//////////////////////////////////////////////////
	// クリック判定用

	// クリック判定色
	DWORD judgeColor;

	// クリック判定色 ピクセルシェーダ用
	float judgeColorFloat4[4];

public:
	//////////////////////////////////////////////////
	// 衝突判定用

	float friction;		// 抵抗

	float resistance;	// 反発


public:
	//////////////////////////////////////////////////
	// その他

	char id[32];  // ID

protected:
	//////////////////////////////////////////////////
	// 継承された関数

	// GameObject を介して継承されました
	virtual void EventBeginPlay() override {}
	virtual void EventTick(float deltaTime) override {}
	virtual void EventKeyDown(KeyEvent e) override {}
	virtual void EventKeyUp(KeyEvent e) override {}
	virtual void EventMouseDown(MouseEvent e) override {}
	virtual void EventMouseUp(MouseEvent e) override {}

protected:

	// HLSLObject を介して継承されました
	virtual void SetParam(ID3DXEffect* effect) override;


public:
	virtual bool Render(ID3DXEffect* effect) = 0;

	virtual void RenderInEditor() { this->Render(nullptr); };
};