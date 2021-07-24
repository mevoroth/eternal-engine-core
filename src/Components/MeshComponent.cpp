#pragma once

#include "Components/MeshComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		void MeshComponent::SetMesh(_In_ MeshCollection* InMeshCollection)
		{
			_MeshCollection = InMeshCollection;
			//GetWorld()->GetGame().
		}
	}
}
