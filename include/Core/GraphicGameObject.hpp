#ifndef _GRAPHIC_GAME_OBJECT_HPP_
#define _GRAPHIC_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class GraphicGameObjectData;
		class TransformComponent;
		class MeshComponent;
		class MaterialComponent;

		class GraphicGameObject : public GameObject
		{
		public:
			GraphicGameObject();

			void CopyTransformComponent(_In_ TransformComponent* MeshComponentObj);
			void SetMeshComponent(_In_ MeshComponent* MeshComponentObj);
			void SetMaterialComponent(_In_ MaterialComponent* MaterialComponentObj);

			TransformComponent* GetTransformComponent();
			MeshComponent* GetMeshComponent();
			MaterialComponent* GetMaterialComponent();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

		private:
			GraphicGameObjectData* _GraphicGameObjectData = nullptr;
		};
	}
}

#endif
