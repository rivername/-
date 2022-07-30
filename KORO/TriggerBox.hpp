#pragma once

#include "Actor.hpp"

//////////////////////////////////////////////////
// �g���K�[�{�b�N�X�ł̏����̎��
// TriggerBoxType::NUM �̓��[�v�p
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
	// �^�C�v�ʂŃC�x���g���`

	void BeginOverlap();

	void EndOverlap();

private:

	TriggerBoxType type;

	bool doOnce;

	bool isOverlapping;

private:
	//////////////////////////////////////////////////
	// �p��

	//virtual Actor* CreateClone() override;

	virtual KoroActorType GetActorType() const override;

	virtual void SetParam(ID3DXEffect* effect) override {};

	virtual bool Render(ID3DXEffect* effect) override;
};