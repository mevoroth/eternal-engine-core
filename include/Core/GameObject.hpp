#pragma once

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class Component;

		class GameObject
		{
		public:

			void AddComponent(_In_ Component* InComponent)
			{
				_Components.push_back(InComponent);
			}

			void RemoveComponent(_In_ Component* InComponent)
			{
				vector<Component*>::iterator ComponentIterator = remove(_Components.begin(), _Components.end(), InComponent);
			}

		private:
			vector<Component*> _Components;
		};
	}
}
