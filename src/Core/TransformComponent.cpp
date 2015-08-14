#include "Core/TransformComponent.hpp"

#include "Macros/Macros.hpp"

using namespace Eternal::Core;

TransformComponent::TransformComponent()
	: Transform(Components::Transform())
{
}

void TransformComponent::Begin()
{
}

void TransformComponent::Update(const TimeT& ElapsedMilliseconds)
{
}

void TransformComponent::End()
{
}

void TransformComponent::AttachTo(TransformComponent* Parent)
{
	ETERNAL_ASSERT(Parent);
	ETERNAL_ASSERT(Parent != this);

#ifdef ETERNAL_DEBUG
	while (Parent->_Parent)
	{
		ETERNAL_ASSERT(Parent->_Parent != this);
	}
#endif

	_Parent = Parent;
}
