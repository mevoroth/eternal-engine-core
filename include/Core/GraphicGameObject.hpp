#ifndef _GRAPHIC_GAME_OBJECT_HPP_
#define _GRAPHIC_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class GraphicGameObjectInstance;
		class GraphicGameObjectData;
		class GraphicGameObjectInstanceData;
		class TransformComponent;
		class MeshComponent;
		class MaterialComponent;

		class GraphicGameObject : public InstanciableGameObject<GraphicGameObjectInstance>
		{
		public:
			GraphicGameObject();

			void SetMeshComponent(_In_ MeshComponent* MeshComponentObj);
			void SetMaterialComponent(_In_ MaterialComponent* MaterialComponentObj);

			MeshComponent* GetMeshComponent();
			MaterialComponent* GetMaterialComponent();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

		private:
			GraphicGameObjectData* _GraphicGameObjectData = nullptr;
		};

		class GraphicGameObjectInstance : public GameObjectInstance
		{
		public:
			GraphicGameObjectInstance();
			~GraphicGameObjectInstance();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			void CopyTransformComponent(_In_ TransformComponent* MeshComponentObj);
			TransformComponent* GetTransformComponent();

		private:
			GraphicGameObjectInstanceData* _GraphicGameObjectInstanceData = nullptr;
		};
	}
}

#endif
