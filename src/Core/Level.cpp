#include "Core/Level.hpp"
#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		Level::Level()
			: WorldObject()
		{
			_GameObjects.reserve(GameObjectsInitialPool);
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
			InGameObject->SetParent(this);
		}

		void Level::RemoveGameObject(_In_ GameObject* InGameObject)
		{
			InGameObject->SetParent(nullptr);
			vector<GameObject*>::iterator GameObjectIterator = find(_GameObjects.begin(), _GameObjects.end(), InGameObject);
			_GameObjects.erase(GameObjectIterator);
		}

		void Level::Update(_In_ const TimeSecondsT InDeltaSeconds)
		{
			for (uint32_t GameObjectIndex = 0; GameObjectIndex < _GameObjects.size(); ++GameObjectIndex)
				_GameObjects[GameObjectIndex]->Update(InDeltaSeconds);
		}

		void Level::UpdateDebug()
		{
			for (uint32_t GameObjectIndex = 0; GameObjectIndex < _GameObjects.size(); ++GameObjectIndex)
				_GameObjects[GameObjectIndex]->UpdateDebug();
		}
	}
}
