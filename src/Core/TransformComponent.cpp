#include "Core/TransformComponent.hpp"

using namespace Eternal::Core;

TransformComponent::TransformComponent()
	: Transform(Components::Transform())
	, _RelativeToWorld(Components::Transform())
	, _Parent(0)
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
	assert(Parent);
	assert(Parent != this);

#ifdef ETERNAL_DEBUG
	while (Parent->_Parent)
	{
		assert(Parent->_Parent != this);
	}
#endif

	Components::Transform CurrentRelativeToWorld = _Parent->GetRelativeToWorldComputed();

	_Parent = Parent;
}

Components::Transform TransformComponent::GetRelativeToWorldComputed() const
{
	return _RelativeToWorld; // TODO: WRONG
	//_Parent
}
