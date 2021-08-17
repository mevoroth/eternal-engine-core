#pragma once

#include "Core/Component.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Components;

		class MeshComponent : public Component
		{
		public:

			MeshComponent() {}

			void Begin() {}
			void Update(_In_ TimeSecondsT InDeltaSeconds) {}
			void SetMesh(_In_ MeshCollection* InMeshCollection);

		private:
			MeshCollection* _MeshCollection = nullptr;
		};
	}
}
