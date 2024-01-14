#include "Components/MeshComponent.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		static ComponentPool<MeshComponent> MeshComponentsPool;

		MeshComponent::MeshComponent()
		{
		}

		void MeshComponent::Begin()
		{
			System& EngineSystem = GetWorld()->GetGame().GetSystem();
			TransformComponent* Transform = GetParent()->GetComponent<TransformComponent>();
			ETERNAL_ASSERT(_MeshCollection);
			EngineSystem.GetGameFrame().MeshCollections.AddObject(_MeshCollection, Transform);
		}

		void MeshComponent::SetMesh(_In_ MeshCollection* InMeshCollection)
		{
			_MeshCollection = InMeshCollection;
			ComponentPool<MeshComponent>::OnAddComponent(this);
			SetHasBehavior();
		}
	}
}
