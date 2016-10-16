#include "Core/LightGameObject.hpp"

#include "GamePools.hpp"

using namespace Eternal::Core;

namespace Eternal
{
	namespace Core
	{
		class LightGameObjectData
		{
		public:
			LightGameObjectData()
			{
				_TransformComponent = g_TransformComponentPool->Initialize(_TransformHandle);
				_LightComponent = g_LightComponentPool->Initialize(_LightHandle);
			}

			~LightGameObjectData()
			{
				g_TransformComponentPool->Release(_TransformHandle);
				g_LightComponentPool->Release(_LightHandle);
			}

			TransformComponent* GetTransformComponent()
			{
				ETERNAL_ASSERT(_TransformComponent);
				return _TransformComponent;
			}

			LightComponent* GetLightComponent()
			{
				ETERNAL_ASSERT(_LightComponent);
				return _LightComponent;
			}

		private:
			PoolHandle _TransformHandle = InvalidHandle;
			PoolHandle _LightHandle = InvalidHandle;

			TransformComponent* _TransformComponent = nullptr;
			LightComponent* _LightComponent = nullptr;
		};
	}
}

LightGameObject::LightGameObject()
{
	_LightGameObjectData = new LightGameObjectData();
}

LightGameObject::~LightGameObject()
{
	delete _LightGameObjectData;
	_LightGameObjectData = nullptr;
}

void LightGameObject::Begin()
{

}

void LightGameObject::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{

}

void LightGameObject::End()
{

}

TransformComponent* LightGameObject::GetTransformComponent()
{
	return _LightGameObjectData->GetTransformComponent();
}

LightComponent* LightGameObject::GetLightComponent()
{
	return _LightGameObjectData->GetLightComponent();
}
