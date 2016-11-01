#include "Core/CameraGameObject.hpp"

#include "Core/TransformComponent.hpp"
#include "Core/CameraComponent.hpp"
#include "GamePools.hpp"

using namespace Eternal::Core;

namespace Eternal
{
	namespace Core
	{
		class CameraGameObjectData
		{
		public:
			CameraGameObjectData()
			{
				_CameraComponent = g_CameraComponentPool->Initialize(_CameraHandle);
				_TransformComponent = g_TransformComponentPool->Initialize(_TransformHandle);
			}
			~CameraGameObjectData()
			{
				g_CameraComponentPool->Release(_CameraHandle);
				g_TransformComponentPool->Release(_TransformHandle);
			}

			TransformComponent* GetTransformComponent()
			{
				return _TransformComponent;
			}

			CameraComponent* GetCameraComponent()
			{
				return _CameraComponent;
			}

		private:
			PoolHandle _CameraHandle = InvalidHandle;
			PoolHandle _TransformHandle = InvalidHandle;

			TransformComponent* _TransformComponent = nullptr;
			CameraComponent* _CameraComponent = nullptr;
		};
	}
}

CameraGameObject::CameraGameObject()
{
	_CameraGameObjectData = new CameraGameObjectData();
}

CameraGameObject::~CameraGameObject()
{

}

void CameraGameObject::Begin()
{
}

void CameraGameObject::End()
{
}

void CameraGameObject::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
}

TransformComponent* CameraGameObject::GetTransformComponent()
{
	return _CameraGameObjectData->GetTransformComponent();
}

CameraComponent* CameraGameObject::GetCameraComponent()
{
	return _CameraGameObjectData->GetCameraComponent();
}

void CameraGameObjectInstance::Begin()
{
}

void CameraGameObjectInstance::End()
{
}

void CameraGameObjectInstance::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
}
