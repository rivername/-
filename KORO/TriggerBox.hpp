#pragma once

#include "Actor.hpp"

//////////////////////////////////////////////////
// トリガーボックスでの処理の種類
// TriggerBoxType::NUM はループ用
enum class TriggerBoxType : unsigned char
{
	TEST_AREA,
	TEST_AREA2,

	NUM
};


class TriggerBox : public virtual Actor
{
public:
	//////////////////////////////////////////////////
	// ctor, dtor

	TriggerBox();
	TriggerBox(const D3DXMATRIX* matrix, Transform* parent = nullptr);

	~TriggerBox();

public:

	void ResetDoOnce() { this->doOnce = false; }

	virtual std::tuple<D3DXVECTOR3, float> CollisionWithKoro(D3DXVECTOR3 center, float radius) override;


private:

	//////////////////////////////////////////////////
	// タイプ別でイベントを定義

	void BeginOverlap();

	void EndOverlap();

private:

	TriggerBoxType type;

	bool doOnce;

	bool isOverlapping;

private:
	//////////////////////////////////////////////////
	// 継承

	//virtual Actor* CreateClone() override;

	virtual KoroActorType GetActorType() const override;

	virtual void SetParam(ID3DXEffect* effect) override {};

	virtual bool Render(ID3DXEffect* effect) override;
};