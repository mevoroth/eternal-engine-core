#include "Core/TransformComponent.hpp"

#include "Macros/Macros.hpp"
#include "Transform/Transform.hpp"

using namespace Eternal::Core;
using namespace Eternal::Components;

Pool<Transform>* g_TransformPool = nullptr;

void TransformComponent::Init()
{
	g_TransformPool = new Pool<Transform>();
}

TransformComponent::TransformComponent()
	: //_Transform(Components::Transform())
	//, _LocalToParentMatrix(NewIdentity())
	//, 
	_LocalToWorldMatrix(NewIdentity())
{
	g_TransformPool->Initialize(_TransformHandle);
}

void TransformComponent::Begin()
{
}

void TransformComponent::Update(_In_ const TimeT& ElapsedMilliseconds)
{
	bool Dirty = false;
	_UpdateCache(Dirty);
}

void TransformComponent::_UpdateCache(bool& Dirty)
{
	if (_Parent)
		_Parent->_UpdateCache(Dirty);

	if (_Dirty)
		Dirty = true;

	if (Dirty)
	{
		if (_Parent)
			_LocalToWorldMatrix = _Parent->_LocalToWorldMatrix * GetTransform().GetModelMatrix();
		else
			_LocalToWorldMatrix = GetTransform().GetModelMatrix();
		_Dirty = false;
	}
}

void TransformComponent::End()
{
}

void TransformComponent::AttachTo(_In_ TransformComponent* Parent)
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
	_Dirty = true;
}

const Components::Transform& TransformComponent::GetTransform() const
{
	return *g_TransformPool->Get(_TransformHandle);
}

Components::Transform& TransformComponent::GetTransform()
{
	_Dirty = true;
	return *g_TransformPool->Get(_TransformHandle);
}
