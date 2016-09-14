#ifndef _CAMERA_GAME_OBJECT_HPP_
#define _CAMERA_GAME_OBJECT_HPP_

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace Core
	{
		class TransformComponent;
		class CameraComponent;

		class CameraGameObject : public GameObject
		{
		public:
			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

			TransformComponent* GetTransformComponent();
			CameraComponent* GetCameraComponent();

		private:
			TransformComponent* _TransformComponent = nullptr;
			CameraComponent* _CameraComponent = nullptr;
		};
	}
}

#endif
