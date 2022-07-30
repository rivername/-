#include "GoalPoint.hpp"

#include "GoalFlag.hpp"

#include "DayNightCycleSkyBoxParam.hpp"

GoalPoint::GoalPoint() : GoalPoint(nullptr)
{}

GoalPoint::GoalPoint(const D3DXMATRIX* const matrix, Transform* parent) : Actor(matrix, parent, "GOAL_POINT")
{
	GoalFlag::instance->SetWorldMatrix(&this->worldMatrix);
	GoalFlag::instance->BecomeParents(this);
	GoalFlag::instance->SetLocalLocationY(1.0f);

	this->visible = false;
}

GoalPoint::~GoalPoint()
{
	OutputDebugFormat("\n {} called. \n", __func__);
}


//Actor* GoalPoint::CreateClone()
//{
//	return nullptr;
//}

KoroActorType GoalPoint::GetActorType() const
{
	return KoroActorType::GOAL_POINT;
}

void GoalPoint::SetParam(ID3DXEffect* effect)
{
	// 行列設定
	this->hlslParam.SetBasicMatrices("mat_World", "mat_View", "mat_Proj");
	effect->SetMatrix("mat_LocalWorld", &GoalFlag::instance->GetWorldMatrix());

	// ライト色
	this->hlslParam.lightColor = SkyBoxParam::ambientLightColor;
	auto lightColor = this->hlslParam.GetLightColor();
	effect->SetVector("lightColor", &lightColor);

	// カラーを設定
	auto mulColor = D3DXVECTOR4(1,1,1,1);
	effect->SetVector("mulColor", &mulColor);
}

bool GoalPoint::Render(ID3DXEffect* effect)
{
	if (!this->isIgnore && this->visible)
	{
		GoalFlag::instance->Render();
	}

	return true;
}