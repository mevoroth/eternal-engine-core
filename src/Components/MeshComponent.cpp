#include "Components/MeshComponent.hpp"
#include "Core/CoreHelper.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Math/Math.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Math;

		static ComponentPool<MeshComponent> MeshComponentsPool;

		MeshComponent::MeshComponent()
		{
			SetHasBehavior();
		}

		void MeshComponent::Begin()
		{
			System& EngineSystem = GetSystem(GetWorld());
			vector<TransformComponent*> TranformComponents;
			GetParent()->GetComponents<TransformComponent>(TranformComponents);
			ETERNAL_ASSERT(_MeshCollection);
			if (TranformComponents.size() > 0)
				EngineSystem.GetGameFrame().MeshCollections.AddObject(_MeshCollection, TranformComponents[Min(_TransformIndex, static_cast<uint32_t>(TranformComponents.size()) - 1)]);
		}

		void MeshComponent::End()
		{
			System& EngineSystem = GetSystem(GetWorld());
			vector<TransformComponent*> TranformComponents;
			GetParent()->GetComponents<TransformComponent>(TranformComponents);
			ETERNAL_ASSERT(_MeshCollection);
			if (TranformComponents.size() > 0)
				EngineSystem.GetGameFrame().MeshCollections.RemoveObject(_MeshCollection, TranformComponents[Min(_TransformIndex, static_cast<uint32_t>(TranformComponents.size()) - 1)]);
		}

		void MeshComponent::SetMesh(_In_ MeshCollection* InMeshCollection)
		{
			_MeshCollection = InMeshCollection;
		}

		void MeshComponent::SetTranformIndex(_In_ uint32_t InTransformIndex)
		{
			_TransformIndex = InTransformIndex;
		}
	}
}
