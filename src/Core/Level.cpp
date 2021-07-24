#include "Core/Level.hpp"
#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		Level::Level()
			: WorldObject()
		{
			_GameObjects.reserve(4096);
		}

		void Level::SetWorld(_In_ World* InWorld)
		{
			WorldObject::SetWorld(InWorld);
			for (uint32_t GameObjectIndex = 0; GameObjectIndex < _GameObjects.size(); ++GameObjectIndex)
				_GameObjects[GameObjectIndex]->SetWorld(InWorld);
		}

		void Level::AddGameObject(_In_ GameObject* InGameObject)
		{
			_GameObjects.push_back(InGameObject);
			InGameObject->SetWorld(GetWorld());
		}

		void Level::RemoveGameObject(_In_ GameObject* InGameObject)
		{
			InGameObject->SetWorld(nullptr);
			vector<GameObject*>::iterator GameObjectIterator = remove(_GameObjects.begin(), _GameObjects.end(), InGameObject);
		}
	}
}
