#include "Gravity.hpp"

Gravity* Gravity::instance;

Gravity::Gravity() : GameObject(ObjectType::GRAVITY)
{
	Gravity::instance = this;

	this->location = D3DXVECTOR3(0, 0, 0);
	this->rotation = Rotation(0, 0, 0);
	this->rotationDeg = Rotation(0, 0, 0);
	this->scale = D3DXVECTOR3(1, 1, 1);
	this->vector = D3DXVECTOR3(0, -1, 0);
	this->constantVelocity = 9.807f * 2.0f;
	this->velocity = constantVelocity / (60.0f * 60.0f);

	this->pressLeft = false;
	this->pressRight = false;
	this->pressUp = false;
	this->pressDown = false;

	materials = nullptr;
	numMaterials = 0;
	mesh = nullptr;

	this->EnableInput();
}

Gravity::~Gravity()
{


}

bool Gravity::Init()
{
	HRESULT hr = D3DXLoadMeshFromX
	(
		"./resources/x/arrow.x",
		D3DXMESH_MANAGED,
		DX9::instance->device3d,
		NULL,
		&this->materials,
		NULL,
		&this->numMaterials,
		&this->mesh
	);

	if (FAILED(hr))
	{
		OutputDebugString("Koro::Init() Error\n");
		return false;
	}

	return true;
}

void Gravity::Update()
{
	/*
	if (pressLeft == true) {
		this->rotationDeg.rotationAmo -= 1;
	}
	if (pressRight == true) {
		this->rotationDeg.rotationAmo += 1;
	}
	if (pressUp == true) {
		this->rotationDeg.pitch -= 1;
	}
	if (pressDown == true) {
		this->rotationDeg.pitch += 1;
	}

	if (this->rotationDeg.rotationAmo > 30) {
		this->rotationDeg.rotationAmo = 30;
	}
	if (this->rotationDeg.rotationAmo < -30) {
		this->rotationDeg.rotationAmo = -30;
	}
	if (this->rotationDeg.pitch > 30) {
		this->rotationDeg.pitch = 30;
	}
	if (this->rotationDeg.pitch < -30) {
		this->rotationDeg.pitch = -30;
	}

	this->SetRotation(this->rotationDeg * D3DX_PI / 180);
	this->SetGravity(this->rotation);

	D3DXVECTOR3 tmpCamVec = this->vector;
	D3DXVec3Normalize(&this->vector, &tmpCamVec);
	*/

}

bool Gravity::Render()
{
	for (int i = 0; i < this->numMaterials; i++)
	{
		D3DXMATERIAL mat = ((D3DXMATERIAL*)(this->materials->GetBufferPointer()))[i];
		DX9::instance->device3d->SetMaterial(&(mat.MatD3D));
		this->mesh->DrawSubset(i);
	};

	return true;
}

bool Gravity::UpdateMatrix()
{
	D3DXMATRIX translate;
	D3DXMATRIX rotate;
	D3DXMATRIX scale;
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	D3DXMatrixTranslation(&translate, this->location.x, this->location.y, this->location.z);
	D3DXMatrixRotationYawPitchRoll(&rotate, this->rotation.yaw, this->rotation.pitch, this->rotation.roll);
	D3DXMatrixScaling(&scale, this->scale.x, this->scale.y, this->scale.z);
	world = scale * rotate * translate;

	if (FAILED(DX9::instance->device3d->SetTransform(D3DTS_WORLD, &world)))
	{
		return false;
	}

	return true;
}

void Gravity::EventTick(float deltaTime)
{
	this->velocity = constantVelocity * deltaTime * deltaTime;
	this->UpdateMatrix();
	//this->Render();
}

void Gravity::EventKeyDown(KeyEvent e)
{
	switch (e.keyCode) {
	case 'A':
		this->pressLeft = true;
		break;
	case 'D':
		this->pressRight = true;
		break;
	case 'W':
		this->pressUp = true;
		break;
	case 'S':
		this->pressDown = true;
		break;
	}
}

void Gravity::EventKeyUp(KeyEvent e)
{
	switch (e.keyCode) {
	case 'A':
		this->pressLeft = false;
		break;
	case 'D':
		this->pressRight = false;
		break;
	case 'W':
		this->pressUp = false;
		break;
	case 'S':
		this->pressDown = false;
		break;
	}
}

void Gravity::EventMouseDown(MouseEvent e)
{
}

void Gravity::EventMouseUp(MouseEvent e)
{
}

void Gravity::EventBeginPlay()
{
}

