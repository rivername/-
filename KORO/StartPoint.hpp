#pragma once

#include "Actor.hpp"

class StartPoint : public virtual Actor
{
public:
    static inline StartPoint* instance;

	// インスタンス化し、返す
	static StartPoint* Instantiate(const D3DXMATRIX* const matrix = nullptr, Transform* parent = nullptr)
	{
        static auto instance = new StartPoint(matrix, parent);
        StartPoint::instance = instance;
        return instance;
	}

protected:
	//////////////////////////////////////////////////
	// ctor

	StartPoint();
	StartPoint(const D3DXMATRIX* const matrix, Transform* parent = nullptr);

public:
	//////////////////////////////////////////////////
	// dtor
	~StartPoint();

private:
	//////////////////////////////////////////////////
	// 継承された仮想関数

	//virtual Actor* CreateClone() override;

	virtual KoroActorType GetActorType() const override;

	// パラメーターを設定
	virtual void SetParam(ID3DXEffect* effect) override;

	// 描画
	virtual bool Render(ID3DXEffect* effect) override;
};
