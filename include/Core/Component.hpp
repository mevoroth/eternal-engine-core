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

		template<typename ComponentType>
		struct ComponentPool
		{
			ComponentPool();

			static void OnAddComponent(_In_ ComponentType* InComponent);
			static void OnRemoveComponent();
			static void Update(_In_ TimeSecondsT InDeltaSeconds);

			static bool IsInitialized;
			static vector<ComponentType*> _ComponentsAdded;
			static vector<ComponentType*> _ComponentsToUpdate;
		};

		class Component : public WorldObject
		{
			template<typename ComponentType>
			friend struct ComponentPool;

		public:

			using WorldObject::WorldObject;

			void SetHasBehavior() { _ComponentState.ComponentHasBehavior = true; }
			void SetUpdatesEveryFrame() { _ComponentState.ComponentUpdatesEveryFrame = true; }
			void SetIsDirty() { _ComponentState.ComponentIsDirty = true; }

			template<typename ComponentType>
			void OnAddComponent();
			template<typename ComponentType>
			void OnRemoveComponent();

			GameObject* GetParent();

		protected:

			struct ComponentState
			{
				ComponentState();

				uint32_t ComponentHasBehavior : 1;
				uint32_t ComponentUpdatesEveryFrame : 1;
				uint32_t ComponentIsDirty : 1;
			} _ComponentState;

		};
	}
}
