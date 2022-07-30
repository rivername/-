#include "Debug.hpp"
#include "utils.hpp"
#include "GameObject.hpp"
#include "DX9.hpp"
#include "HLSL.hpp"

#pragma once

namespace goalFlag
{
	constexpr unsigned int NUM_VERTICES = 44;
	constexpr unsigned int NUM_INDICES = 6;
}

struct GoalFlagVertex
{
	D3DXVECTOR3 location;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;

	static const DWORD FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
};

class GoalFlag : public HLSLObject, public GameObject, public Transform
{
public:
	static GoalFlag* instance;

public:

	GoalFlag(D3DXVECTOR3 location = D3DXVECTOR3(0,0,0));

	bool Init();

public:

    virtual void SetParam(ID3DXEffect* effect) override;
    virtual bool Render(ID3DXEffect* effect) override;

    bool Render();


private:

	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9* indexBuffer;

	D3DMATERIAL9 material;


	GoalFlagVertex vertices[goalFlag::NUM_VERTICES];

	float deg;

	// GameObject を介して継承されました
	virtual void EventRender() override;

	virtual void EventBeginPlay() override;

	virtual void EventTick(float deltaTime) override;

	virtual void EventKeyDown(KeyEvent e) override;

	virtual void EventKeyUp(KeyEvent e) override;

	virtual void EventMouseDown(MouseEvent e) override;

	virtual void EventMouseUp(MouseEvent e) override;
};

