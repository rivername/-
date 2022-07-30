#include "TriggerBox.hpp"

#include "Particle.hpp"

TriggerBox::TriggerBox()
	: TriggerBox(nullptr, nullptr)
{}

TriggerBox::TriggerBox(const D3DXMATRIX* matrix, Transform* parent)
	: Actor(matrix, parent), type(TriggerBoxType::TEST_AREA), doOnce(false), isOverlapping(false)
{
}

TriggerBox::~TriggerBox()
{}

std::tuple<D3DXVECTOR3, float> TriggerBox::CollisionWithKoro(D3DXVECTOR3 center, float radius)
{
	auto result = this->Actor::CollisionWithKoro(center, radius);
	if (std::get<1>(result) != 0)
	{
		if (!this->isOverlapping)
		{
			this->BeginOverlap();
			this->isOverlapping = true;
		}
	}
	else
	{
		if (this->isOverlapping)
		{
			this->EndOverlap();
			this->isOverlapping = false;
		}
	}

	return result;
}

void TriggerBox::BeginOverlap()
{
	switch (this->type)
	{
	case TriggerBoxType::TEST_AREA:
		break;


	case TriggerBoxType::NUM:
	default:
		break;
	}


	OutputDebugFormat("\n\nbegin overlap\n\n");
}

void TriggerBox::EndOverlap()
{
	switch (this->type)
	{
	case TriggerBoxType::TEST_AREA:

		break;


	case TriggerBoxType::NUM:
	default:
		break;
	}


	OutputDebugFormat("\n\nend overlap\n\n");
}

//Actor* TriggerBox::CreateClone()
//{
//	return new TriggerBox(&this->worldMatrix);
//}

KoroActorType TriggerBox::GetActorType() const
{
	return KoroActorType::TRIGGER_BOX;
}

bool TriggerBox::Render(ID3DXEffect* effect)
{
	if (!this->isIgnore && this->visible)
	{
		Debug::AddLine("", __func__);
	}

	return true;
}
