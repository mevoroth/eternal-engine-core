#include "Components/MeshComponent.hpp"
#include "Core/CoreHelper.hpp"
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
			SetHasBehavior();
		}

		void MeshComponent::Begin()
		{
			System& EngineSystem = GetSystem(GetWorld());
			TransformComponent* Transform = GetParent()->GetComponent<TransformComponent>();
			ETERNAL_ASSERT(_MeshCollection);
			EngineSystem.GetGameFrame().MeshCollections.AddObject(_MeshCollection, Transform);
		}

		void MeshComponent::End()
		{
			System& EngineSystem = GetSystem(GetWorld());
			TransformComponent* Transform = GetParent()->GetComponent<TransformComponent>();
			ETERNAL_ASSERT(_MeshCollection);
			EngineSystem.GetGameFrame().MeshCollections.RemoveObject(_MeshCollection, Transform);
		}

		void MeshComponent::SetMesh(_In_ MeshCollection* InMeshCollection)
		{
			_MeshCollection = InMeshCollection;
		}
	}
}
