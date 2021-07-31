#include "Core/Component.hpp"
#include "Core/GameObject.hpp"
#include "Core/World.hpp"

#include "Components/TransformComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"

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

		template<typename ComponentType>
		ComponentPool<ComponentType>::ComponentPool()
		{
			ETERNAL_ASSERT(!IsInitialized);
			_ComponentsAdded.reserve(World::ComponentsAddedCountInitialPool);
			_ComponentsToUpdate.reserve(World::ComponentsToUpdateCountInitialPool);
			IsInitialized = true;
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::OnAddComponent(_In_ ComponentType* InComponent)
		{
			_ComponentsAdded.push_back(InComponent);
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::OnRemoveComponent()
		{
		}

		template<typename ComponentType>
		void ComponentPool<ComponentType>::Update(_In_ TimeSecondsT InDeltaSeconds)
		{
			uint32_t ComponentsCount = static_cast<uint32_t>(_ComponentsAdded.size());
			for (uint32_t ComponentIndex = 0; ComponentIndex < ComponentsCount; ++ComponentIndex)
			{
				while (_ComponentsAdded[ComponentIndex]->GetWorld() && ComponentIndex < ComponentsCount)
				{
					if (_ComponentsAdded[ComponentIndex]->_ComponentState.ComponentUpdatesEveryFrame)
						_ComponentsToUpdate.push_back(_ComponentsAdded[ComponentIndex]);

					swap(_ComponentsAdded[ComponentIndex], _ComponentsAdded[--ComponentsCount]);
				}
			}
			_ComponentsAdded.erase(_ComponentsAdded.begin() + ComponentsCount, _ComponentsAdded.end());

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
				ComponentPool<ComponentType>::OnRemoveComponent();
		}

		//////////////////////////////////////////////////////////////////////////

		template void Component::OnAddComponent<TransformComponent>();
		template void Component::OnAddComponent<MeshComponent>();
		template void Component::OnAddComponent<CameraComponent>();

		template struct ComponentPool<TransformComponent>;
		template struct ComponentPool<MeshComponent>;
		template struct ComponentPool<CameraComponent>;
	}
}
