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
			MeshComponent* _MeshComponent = nullptr;
			MaterialComponent* _MaterialComponent = nullptr;
		};

		class GraphicGameObjectInstanceData
		{
		public:
			GraphicGameObjectInstanceData()
			{
				_TransformComponent = g_TransformComponentPool->Initialize(_TransformHandle);
			}

			TransformComponent* GetTransformComponent()
			{
				return _TransformComponent;
			}
		private:
			PoolHandle _TransformHandle = InvalidHandle;
			TransformComponent* _TransformComponent = nullptr;
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

void GraphicGameObject::SetMeshComponent(_In_ MeshComponent* MeshComponentObj)
{
	_GraphicGameObjectData->SetMeshComponent(MeshComponentObj);
}

void GraphicGameObject::SetMaterialComponent(_In_ MaterialComponent* MaterialComponentObj)
{
	_GraphicGameObjectData->SetMaterialComponent(MaterialComponentObj);
}

MeshComponent* GraphicGameObject::GetMeshComponent()
{
	return _GraphicGameObjectData->GetMeshComponent();
}

MaterialComponent* GraphicGameObject::GetMaterialComponent()
{
	return _GraphicGameObjectData->GetMaterialComponent();
}

GraphicGameObjectInstance::GraphicGameObjectInstance()
{
	_GraphicGameObjectInstanceData = new GraphicGameObjectInstanceData();
}

GraphicGameObjectInstance::~GraphicGameObjectInstance()
{
	delete _GraphicGameObjectInstanceData;
	_GraphicGameObjectInstanceData = nullptr;
}

void GraphicGameObjectInstance::Begin()
{
}

void GraphicGameObjectInstance::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
}

void GraphicGameObjectInstance::End()
{
}

void GraphicGameObjectInstance::CopyTransformComponent(_In_ TransformComponent* MeshComponentObj)
{
	memcpy(&_GraphicGameObjectInstanceData->GetTransformComponent()->GetTransform(), &MeshComponentObj->GetTransform(), sizeof(Transform));
}

TransformComponent* GraphicGameObjectInstance::GetTransformComponent()
{
	return _GraphicGameObjectInstanceData->GetTransformComponent();
}
