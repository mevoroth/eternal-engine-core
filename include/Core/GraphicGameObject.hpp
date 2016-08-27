#ifndef _GRAPHIC_GAME_OBJECT_HPP_
#define _GRAPHIC_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace GraphicData
	{
		class Material;
	}

	namespace Core
	{
		using namespace Eternal::GraphicData;

		class GraphicGameObjectData;
		class TransformComponent;
		class MeshComponent;

		class GraphicGameObject : public GameObject
		{
		public:
			GraphicGameObject();

			void SetMeshComponent(_In_ MeshComponent* MeshComponentObj);

			TransformComponent* GetTransformComponent();
			MeshComponent* GetMeshComponent();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

		private:
			GraphicGameObjectData* _GraphicGameObjectData = nullptr;
		};
	}
}

#endif
