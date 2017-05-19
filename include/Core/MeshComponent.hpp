#ifndef _MESH_COMPONENT_HPP_
#define _MESH_COMPONENT_HPP_

#include "GameComponent.hpp"
#include "Mesh/GenericMesh.hpp"

namespace Eternal
{
	namespace Components
	{
		class Mesh;
		class BoundingBox;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class MeshComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			void SetMesh(_In_ Mesh* MeshObj);
			Mesh* GetMesh();

			void SetBoundingBox(_In_ Mesh* BoundingBoxObj);
			Mesh* GetBoundingBox();

		private:
			Mesh*	_Mesh			= nullptr;
			Mesh*	_BoundingBox	= nullptr;
		};
	}
}

#endif
