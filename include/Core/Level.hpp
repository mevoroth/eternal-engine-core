#pragma once

#include "Core/WorldObject.hpp"
#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class GameObject;

		class Level : public WorldObject
		{
		public:
			static constexpr uint32_t GameObjectsInitialPool = 4096;

			Level();

			virtual void SetWorld(_In_ World* InWorld) override final;

			void AddGameObject(_In_ GameObject* InGameObject);
			void RemoveGameObject(_In_ GameObject* InGameObject);

		private:
			vector<GameObject*> _GameObjects;
		};
	}
}
