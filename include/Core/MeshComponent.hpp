#ifndef _MESH_COMPONENT_HPP_
#define _MESH_COMPONENT_HPP_

#include "GameComponent.hpp"
#include "Mesh/GenericMesh.hpp"

namespace Eternal
{
	namespace Components
	{
		class Mesh;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class MeshComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

		private:
			Mesh* _Mesh = nullptr;
		};
	}
}

#endif
