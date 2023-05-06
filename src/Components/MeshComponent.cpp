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

		void MeshComponent::SetMesh(_In_ MeshCollection* InMeshCollection)
		{
			_MeshCollection = InMeshCollection;
			System& EngineSystem = GetWorld()->GetGame().GetSystem();
			TransformComponent* Transform = GetParent()->GetComponent<TransformComponent>();
			EngineSystem.GetGameFrame().MeshCollections.AddObject(InMeshCollection, Transform);
		}
	}
}
