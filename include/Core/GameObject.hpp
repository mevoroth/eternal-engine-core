#pragma once

#include "Core/WorldObject.hpp"
#include <vector>
#include <functional>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class Component;
		class Level;

		class GameObject : public WorldObject
		{
		public:
			GameObject();

			virtual void SetWorld(_In_ World* InWorld) override final;

			template<typename ComponentType>
			inline ComponentType* AddComponent(_In_ const function<void (_Inout_ ComponentType*)>& SetupFunctor = [](_Inout_ ComponentType*) {})
			{
				ComponentType* OutComponent = new ComponentType();
				AddComponent<ComponentType>(OutComponent, SetupFunctor);
				return OutComponent;
			}

			template<typename ComponentType>
			inline void AddComponent(_In_ ComponentType* InComponent, _In_ const function<void (_Inout_ ComponentType*)>& SetupFunctor = [](_Inout_ ComponentType*) {})
			{
				_Components.push_back(InComponent);
				InComponent->SetParent(this);
				if (SetupFunctor)
					SetupFunctor(InComponent);
				InComponent->OnAddComponent<ComponentType>();
			}

			template<typename ComponentType>
			inline void RemoveComponent(_In_ ComponentType* InComponent)
			{
				InComponent->OnRemoveComponent<ComponentType>();
				InComponent->SetParent(nullptr);
				vector<Component*>::iterator ComponentIterator = remove(_Components.begin(), _Components.end(), InComponent);
			}


		protected:

			Level* GetParent();

		private:
			vector<Component*> _Components;
		};
	}
}
