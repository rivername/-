#pragma once

#include "Actor.hpp"

class GoalPoint : public virtual Actor
{
public:
    static inline GoalPoint* instance;

	// インスタンス化し、返す
	static GoalPoint* Instantiate(const D3DXMATRIX* const matrix = nullptr, Transform* parent = nullptr)
	{
        static auto instance = new GoalPoint(matrix, parent);
        GoalPoint::instance = instance;
        return instance;
	}

protected:
	//////////////////////////////////////////////////
	// ctor

	GoalPoint();
	GoalPoint(const D3DXMATRIX* const matrix, Transform* parent = nullptr);

public:
	//////////////////////////////////////////////////
	// dtor
	~GoalPoint();

private:
	//////////////////////////////////////////////////
	// 継承された純粋仮想関数

	//virtual Actor* CreateClone() override;

	virtual KoroActorType GetActorType() const override;


	// パラメーターを設定
	virtual void SetParam(ID3DXEffect* effect) override;

	// 描画
	virtual bool Render(ID3DXEffect* effect) override;
};
