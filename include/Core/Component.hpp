#pragma once

#include "Core/WorldObject.hpp"
#include "Parallel/AtomicS32.hpp"
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
		using namespace Eternal::Parallel;

		template<typename ComponentType>
		struct ComponentPool
		{
			ComponentPool();
			~ComponentPool();

			static void OnAddComponent(_In_ ComponentType* InComponent);
			static void OnRemoveComponent(_In_ ComponentType* InComponent);
			static void Update(_In_ TimeSecondsT InDeltaSeconds);

			static bool IsInitialized;
			static vector<ComponentType*> _ComponentsAdded;
			static vector<ComponentType*> _ComponentsToUpdate;
			static vector<ComponentType*> _ComponentsRemoved;
			static AtomicS32* _ComponentsAddedGuard;
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
