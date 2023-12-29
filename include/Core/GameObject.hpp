#pragma once

#include "Core/WorldObject.hpp"
#include <vector>
#include <functional>

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

		class Component;
		class Level;

		class GameObject : public WorldObject
		{
		public:
			GameObject();

			virtual void Update(const TimeSecondsT InDeltaSeconds)
			{
				(void)InDeltaSeconds;
			}
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
				InComponent->template OnAddComponent<ComponentType>();
			}

			template<typename ComponentType>
			inline void RemoveComponent(_In_ ComponentType* InComponent)
			{
				InComponent->template OnRemoveComponent<ComponentType>();
				InComponent->SetParent(nullptr);
				vector<Component*>::iterator ComponentIterator = remove(_Components.begin(), _Components.end(), InComponent);
			}

			template<typename ComponentType>
			inline ComponentType* GetComponent()
			{
				for (uint32_t ComponentIndex = 0; ComponentIndex < _Components.size(); ++ComponentIndex)
				{
					if (ComponentType* CastedComponent = dynamic_cast<ComponentType*>(_Components[ComponentIndex]))
						return CastedComponent;
				}
				return nullptr;
			}

			virtual void UpdateDebug() {}

		protected:

			Level* GetParent();

			struct GameObjectState
			{
				GameObjectState();

				uint32_t ObjectUpdatesEveryFrame : 1;
			} _ObjectState;

		private:

			vector<Component*> _Components;
		};
	}
}
