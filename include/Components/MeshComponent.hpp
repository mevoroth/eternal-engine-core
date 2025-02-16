#pragma once

#include "Core/Component.hpp"

namespace Eternal
{
	namespace Components
	{
		class MeshCollection;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class MeshComponent : public Component
		{
		public:

			MeshComponent();

			void Begin();
			void Update(_In_ TimeSecondsT InDeltaSeconds)
			{
				(void)InDeltaSeconds;
			}
			void End();
			void SetMesh(_In_ MeshCollection* InMeshCollection);
			void SetTranformIndex(_In_ uint32_t InTransformIndex);

			MeshCollection* GetMeshCollection() { return _MeshCollection; }

		private:
			MeshCollection*	_MeshCollection	= nullptr;
			uint32_t		_TransformIndex	= 0u;
		};
	}
}
