#include "Core/GameObject.hpp"
#include "Core/Component.hpp"
#include "Core/Level.hpp"
#include "Core/World.hpp"

namespace Eternal
{
	namespace Core
	{
		GameObject::GameObject()
			: WorldObject()
		{
			_Components.reserve(World::ComponentsPerGameObjectInitialPool);
		}

		void GameObject::SetWorld(_In_ World* InWorld)
		{
			WorldObject::SetWorld(InWorld);
			for (uint32_t ComponentIndex = 0; ComponentIndex < _Components.size(); ++ComponentIndex)
				_Components[ComponentIndex]->SetWorld(InWorld);
		}

		Level* GameObject::GetParent()
		{
			return static_cast<Level*>(GetParentObject());
		}

		//////////////////////////////////////////////////////////////////////////

		GameObject::GameObjectState::GameObjectState()
			: ObjectUpdatesEveryFrame(false)
		{
		}
	}
}
