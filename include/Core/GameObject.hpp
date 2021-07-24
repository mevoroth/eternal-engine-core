#pragma once

#include "Core/WorldObject.hpp"
#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class Component;

		class GameObject : public WorldObject
		{
		public:

			GameObject();

			virtual void SetWorld(_In_ World* InWorld) override final;

			template<typename ComponentType>
			inline ComponentType* AddComponent()
			{
				ComponentType* OutComponent = new ComponentType();
				AddComponent(OutComponent);
				return OutComponent;
			}

			void AddComponent(_In_ Component* InComponent);
			void RemoveComponent(_In_ Component* InComponent);

		private:
			vector<Component*> _Components;
		};
	}
}
