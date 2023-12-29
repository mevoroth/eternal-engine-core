#pragma once

#include "Core/Component.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace Components
	{
		class Camera;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class CameraComponent;

		struct CameraOnTransformChanged : public OnTransformChangedEvent
		{
			CameraOnTransformChanged(_In_ CameraComponent* InCameraComponent)
				: _CameraComponent(InCameraComponent)
			{
			}

			virtual void OnTransformChanged(_In_ const Transform& InTransform) override final;

		private:
			CameraComponent* _CameraComponent = nullptr;
		};

		class CameraComponent : public Component
		{
		public:

			CameraComponent(_In_ World* InWorld = nullptr);

			void Begin();
			void Update(_In_ TimeSecondsT InDeltaSeconds)
			{
				(void)InDeltaSeconds;
			}
			void SetCamera(_In_ Camera* InCamera)
			{
				_Camera = InCamera;
			}

			const Camera& GetCamera() const
			{
				ETERNAL_ASSERT(_Camera);
				return *_Camera;
			}

			Camera& GetCamera()
			{
				ETERNAL_ASSERT(_Camera);
				_ComponentState.ComponentIsDirty = true;
				return *_Camera;
			}

			OnTransformChangedEvent& OnTransformChangedReceiver() { return _OnTransformChangedReceiver; }

		private:
			Camera* _Camera = nullptr;
			CameraOnTransformChanged _OnTransformChangedReceiver;

		};
	}
}
