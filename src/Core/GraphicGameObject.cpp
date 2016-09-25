#include "Core/GraphicGameObject.hpp"

#include "Core/MaterialComponent.hpp"
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

			void SetMaterialComponent(_In_ MaterialComponent* MaterialComponentObj)
			{
				_MaterialComponent = MaterialComponentObj;
			}

			MaterialComponent* GetMaterialComponent()
			{
				return _MaterialComponent;
			}

		private:
			PoolHandle _TransformHandle = InvalidHandle;
			TransformComponent* _TransformComponent = nullptr;
			MeshComponent* _MeshComponent = nullptr;
			MaterialComponent* _MaterialComponent = nullptr;
		};

	}
}

using namespace Eternal::Core;


void GraphicGameObject::Begin()
{
}

void GraphicGameObject::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
}

void GraphicGameObject::End()
{
}

GraphicGameObject::GraphicGameObject()
{
	_GraphicGameObjectData = new GraphicGameObjectData();
}

void GraphicGameObject::CopyTransformComponent(_In_ TransformComponent* MeshComponentObj)
{
	memcpy(&_GraphicGameObjectData->GetTransformComponent()->GetTransform(), &MeshComponentObj->GetTransform(), sizeof(Transform));
}

void GraphicGameObject::SetMeshComponent(_In_ MeshComponent* MeshComponentObj)
{
	_GraphicGameObjectData->SetMeshComponent(MeshComponentObj);
}

void GraphicGameObject::SetMaterialComponent(_In_ MaterialComponent* MaterialComponentObj)
{
	_GraphicGameObjectData->SetMaterialComponent(MaterialComponentObj);
}

TransformComponent* GraphicGameObject::GetTransformComponent()
{
	return _GraphicGameObjectData->GetTransformComponent();
}

MeshComponent* GraphicGameObject::GetMeshComponent()
{
	return _GraphicGameObjectData->GetMeshComponent();
}

MaterialComponent* GraphicGameObject::GetMaterialComponent()
{
	return _GraphicGameObjectData->GetMaterialComponent();
}
