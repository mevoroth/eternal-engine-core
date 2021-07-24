#include "Core/GameObject.hpp"
#include "Core/Component.hpp"

namespace Eternal
{
	namespace Core
	{
		GameObject::GameObject()
			: WorldObject()
		{
		}

		void GameObject::SetWorld(_In_ World* InWorld)
		{
			WorldObject::SetWorld(InWorld);
			for (uint32_t ComponentIndex = 0; ComponentIndex < _Components.size(); ++ComponentIndex)
				_Components[ComponentIndex]->SetWorld(InWorld);
		}

		void GameObject::AddComponent(_In_ Component* InComponent)
		{
			_Components.push_back(InComponent);
			InComponent->SetWorld(GetWorld());
		}

		void GameObject::RemoveComponent(_In_ Component* InComponent)
		{
			InComponent->SetWorld(nullptr);
			vector<Component*>::iterator ComponentIterator = remove(_Components.begin(), _Components.end(), InComponent);
		}
	}
}
