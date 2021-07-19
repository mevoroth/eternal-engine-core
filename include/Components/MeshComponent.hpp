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

			void SetMesh(_In_ Mesh* InMesh) { _Mesh = InMesh; }

		private:
			Mesh* _Mesh = nullptr;
		};
	}
}
