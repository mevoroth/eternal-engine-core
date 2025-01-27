#include "Core/Component.hpp"
#include "Core/GameObject.hpp"
#include "Core/World.hpp"
#include "Parallel/AtomicS32Factory.hpp"
#include "Parallel/AtomicGuard.hpp"

#include "Components/TransformComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/LightComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		Component::ComponentState::ComponentState()
			: ComponentHasBehavior(false)
			, ComponentUpdatesEveryFrame(false)
			, ComponentIsDirty(false)
		{
		}

		//////////////////////////////////////////////////////////////////////////

		template<typename ComponentType> bool ComponentPool<ComponentType>::IsInitialized = false;
		template<typename ComponentType> vector<ComponentType*> ComponentPool<ComponentType>::_ComponentsAdded;
		template<typename ComponentType> vector<ComponentType*> ComponentPool<ComponentType>::_ComponentsToUpdate;
		template<typename ComponentType> vector<ComponentType*> ComponentPool<ComponentType>::_ComponentsRemoved;
		template<typename ComponentType> AtomicS32* ComponentPool<ComponentType>::_ComponentsAddedGuard = nullptr;

		template<typename ComponentType>
		ComponentPool<ComponentType>::ComponentPool()
		{
			ETERNAL_ASSERT(!IsInitialized);
			_ComponentsAdded.reserve(World::ComponentsAddedCountInitialPool);
			_ComponentsToUpdate.reserve(World::ComponentsToUpdateCountInitialPool);
			_ComponentsRemoved.reserve(World::ComponentsRemovedCountInitialPool);
			_ComponentsAddedGuard = CreateAtomicS32();
			IsInitialized = true;
		}

		template<typename ComponentType>
		ComponentPool<ComponentType>::~ComponentPool()
		{
			DestroyAtomicS32(_ComponentsAddedGuard);
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::OnAddComponent(_In_ ComponentType* InComponent)
		{
			ETERNAL_ASSERT(IsInitialized);
			AtomicGuard AddComponentGuard(_ComponentsAddedGuard);
			_ComponentsAdded.push_back(InComponent);
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::OnRemoveComponent(_In_ ComponentType* InComponent)
		{
			ETERNAL_ASSERT(IsInitialized);
			_ComponentsRemoved.push_back(InComponent);
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::Update(_In_ TimeSecondsT InDeltaSeconds)
		{
			ETERNAL_ASSERT(IsInitialized);
			{
				uint32_t ComponentsRemovedCount = static_cast<uint32_t>(_ComponentsRemoved.size());
				for (uint32_t ComponentIndex = 0; ComponentIndex < ComponentsRemovedCount; ++ComponentIndex)
				{
					_ComponentsRemoved[ComponentIndex]->End();
					if (_ComponentsRemoved[ComponentIndex]->_ComponentState.ComponentUpdatesEveryFrame)
					{
						auto FoundComponentIterator = find(_ComponentsToUpdate.begin(), _ComponentsToUpdate.end(), _ComponentsRemoved[ComponentIndex]);
						_ComponentsToUpdate.erase(FoundComponentIterator);
					}
				}
				_ComponentsRemoved.clear();
			}
			{
				AtomicGuard AddComponentGuard(_ComponentsAddedGuard);
				uint32_t ComponentsAddedCount = static_cast<uint32_t>(_ComponentsAdded.size());
				for (uint32_t ComponentIndex = 0; ComponentIndex < ComponentsAddedCount; ++ComponentIndex)
				{
					while (_ComponentsAdded[ComponentIndex]->GetWorld() && ComponentIndex < ComponentsAddedCount)
					{
						if (_ComponentsAdded[ComponentIndex]->_ComponentState.ComponentUpdatesEveryFrame)
							_ComponentsToUpdate.push_back(_ComponentsAdded[ComponentIndex]);

						_ComponentsAdded[ComponentIndex]->Begin();

						swap(_ComponentsAdded[ComponentIndex], _ComponentsAdded[--ComponentsAddedCount]);
					}
				}
				_ComponentsAdded.erase(_ComponentsAdded.begin() + ComponentsAddedCount, _ComponentsAdded.end());
			}

			for (uint32_t ComponentIndex = 0; ComponentIndex < _ComponentsToUpdate.size(); ++ComponentIndex)
			{
				if (_ComponentsToUpdate[ComponentIndex]->_ComponentState.ComponentIsDirty)
				{
					_ComponentsToUpdate[ComponentIndex]->_ComponentState.ComponentIsDirty = false;
					_ComponentsToUpdate[ComponentIndex]->Update(InDeltaSeconds);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////

		GameObject* Component::GetParent()
		{
			return static_cast<GameObject*>(GetParentObject());
		}

		template<typename ComponentType>
		void Component::OnAddComponent()
		{
			if (_ComponentState.ComponentHasBehavior)
				ComponentPool<ComponentType>::OnAddComponent(static_cast<ComponentType*>(this));
		}

		template<typename ComponentType>
		void Component::OnRemoveComponent()
		{
			if (_ComponentState.ComponentHasBehavior)
				ComponentPool<ComponentType>::OnRemoveComponent(static_cast<ComponentType*>(this));
		}

		//////////////////////////////////////////////////////////////////////////

		template void Component::OnAddComponent<TransformComponent>();
		template void Component::OnAddComponent<MeshComponent>();
		template void Component::OnAddComponent<CameraComponent>();
		template void Component::OnAddComponent<LightComponent>();

		template void Component::OnRemoveComponent<TransformComponent>();
		template void Component::OnRemoveComponent<MeshComponent>();
		template void Component::OnRemoveComponent<CameraComponent>();
		template void Component::OnRemoveComponent<LightComponent>();

		template struct ComponentPool<TransformComponent>;
		template struct ComponentPool<MeshComponent>;
		template struct ComponentPool<CameraComponent>;
		template struct ComponentPool<LightComponent>;
	}
}
