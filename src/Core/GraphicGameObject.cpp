#include "Core/GraphicGameObject.hpp"

#include "GamePools.hpp"

namespace Eternal
{
	namespace Core
	{
		class GraphicGameObjectData
		{
		public:
			GraphicGameObjectData()
			{
				_TransformComponent = g_TransformComponentPool->Initialize(_TransformHandle);
			}

			TransformComponent* GetTransformComponent()
			{
				return _TransformComponent;
			}

			MeshComponent* GetMeshComponent()
			{
				return _MeshComponent;
			}

			void SetMeshComponent(_In_ MeshComponent* MeshComponentObj)
			{
				_MeshComponent = MeshComponentObj;
			}

		private:
			PoolHandle _TransformHandle = InvalidHandle;
			TransformComponent* _TransformComponent = nullptr;
			MeshComponent* _MeshComponent = nullptr;
		};

	}
}

using namespace Eternal::Core;


void GraphicGameObject::Begin()
{
}

void GraphicGameObject::Update(_In_ const TimeT& ElapsedMicroSeconds)
{
}

void GraphicGameObject::End()
{
}

GraphicGameObject::GraphicGameObject()
{
	_GraphicGameObjectData = new GraphicGameObjectData();
}

void GraphicGameObject::SetMeshComponent(_In_ MeshComponent* MeshComponentObj)
{
	_GraphicGameObjectData->SetMeshComponent(MeshComponentObj);
}

TransformComponent* GraphicGameObject::GetTransformComponent()
{
	return _GraphicGameObjectData->GetTransformComponent();
}

MeshComponent* GraphicGameObject::GetMeshComponent()
{
	return _GraphicGameObjectData->GetMeshComponent();
}
