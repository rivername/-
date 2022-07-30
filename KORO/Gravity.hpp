#include "DX9.hpp"
#include "GameObject.hpp"
#include <d3d9.h>
#include "const.hpp"
#pragma once

class Gravity : public GameObject
{
public:
	static Gravity* instance;

public:
	Gravity();
	~Gravity();

	bool Init();
	void Update();

	// location
	void SetLocation(D3DXVECTOR3 location) { this->location = location; }
	void SetLocation(float x, float y, float z) { this->location = D3DXVECTOR3(x, y, z); }

	// rotation
	Rotation GetRotation() const { return this->rotation; }
	Rotation GetRotationDeg() const { return this->rotationDeg; }
	void SetRotation(Rotation rotation) { this->rotation = rotation; }
	void SetRotation(float x, float y, float z) { this->rotation = Rotation(x, y, z); }
	void AddRotation(Rotation rotation) { this->rotation += rotation; }
	void AddRotation(float x, float y, float z) { this->rotation += Rotation(x, y, z); }
	void SetGravity(Rotation rotation) {
		this->vector = -rotation.RadianToVector3(1);
	}

	D3DXVECTOR3 GetGravityVector() const { return this->vector * this->velocity; }
	D3DXVECTOR3 GetGravityNormal() const { return this->vector; }
	float GetGravityVelocity() const { return this->velocity; }

private:
	bool Render();
	bool UpdateMatrix();
	
public:

private:
	D3DXVECTOR3 location;
	Rotation rotation;
	Rotation rotationDeg;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 vector;
	float constantVelocity;
	float velocity;

	bool pressLeft;
	bool pressRight;
	bool pressUp;
	bool pressDown;


	ID3DXBuffer* materials;
	DWORD numMaterials;
	ID3DXMesh* mesh;
	
private:
	virtual void EventBeginPlay() override;

	virtual void EventTick(float deltaTime) override;

	virtual void EventKeyDown(KeyEvent e) override;

	virtual void EventKeyUp(KeyEvent e) override;

	virtual void EventMouseDown(MouseEvent e) override;

	virtual void EventMouseUp(MouseEvent e) override;

};