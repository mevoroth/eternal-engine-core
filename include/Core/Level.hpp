#pragma once

#include "Core/WorldObject.hpp"
#include <vector>

namespace Eternal
{
	namespace Time
	{
		using TimeSecondsT = double;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Time;

		class GameObject;

		class Level : public WorldObject
		{
		public:
			static constexpr uint32_t GameObjectsInitialPool = 4096;

			Level();

			void Update(_In_ const TimeSecondsT InDeltaSeconds);
			virtual void SetWorld(_In_ World* InWorld) override final;

			void AddGameObject(_In_ GameObject* InGameObject);
			void RemoveGameObject(_In_ GameObject* InGameObject);

			void UpdateDebug();

		private:
			vector<GameObject*> _GameObjects;
		};
	}
}
