#ifndef _CAMERA_COMPONENT_HPP_
#define _CAMERA_COMPONENT_HPP_

#include "Core/GameComponent.hpp"

namespace Eternal
{
	namespace Components
	{
		class Camera;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class TransformComponent;

		class CameraComponent : public GameComponent
		{
		public:
			CameraComponent();
			~CameraComponent();

			static void Initialize();
			static void Release();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			Camera* GetCamera();
			void SetCamera(_In_ Camera* CameraObj);

			void AttachTo(_In_ TransformComponent* TransformComponentObj);

		private:
			TransformComponent* _TransformComponent = nullptr;
			Camera* _Camera = nullptr;
		};
	}
}

#endif
