#pragma once

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class GameObject;

		class Level
		{
		public:
			Level();

			void AddGameObject(_In_ GameObject* InGameObject)
			{
				_GameObjects.push_back(InGameObject);
			}
			void RemoveGameObject(_In_ GameObject* InGameObject)
			{
				vector<GameObject*>::iterator GameObjectIterator = remove(_GameObjects.begin(), _GameObjects.end(), InGameObject);
			}

		private:
			vector<GameObject*> _GameObjects;
		};
	}
}
