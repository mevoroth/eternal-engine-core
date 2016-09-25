#ifndef _CAMERA_GAME_OBJECT_HPP_
#define _CAMERA_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class TransformComponent;
		class CameraComponent;
		class CameraGameObjectData;

		class CameraGameObject : public GameObject
		{
		public:
			CameraGameObject();
			~CameraGameObject();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			TransformComponent* GetTransformComponent();
			CameraComponent* GetCameraComponent();

		private:
			CameraGameObjectData* _CameraGameObjectData = nullptr;
		};
	}
}

#endif
