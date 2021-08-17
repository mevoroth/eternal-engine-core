#pragma once

#include "Core/Component.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace Components
	{
		class Light;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class LightComponent;

		struct LightOnTransformChanged : public OnTransformChangedEvent
		{
			LightOnTransformChanged(_In_ LightComponent* InLightComponent)
				: _LightComponent(InLightComponent)
			{
			}

			virtual void OnTransformChanged(_In_ const Transform& InTransform) override final;

		private:
			LightComponent* _LightComponent = nullptr;
		};

		class LightComponent : public Component
		{
		public:

			LightComponent(_In_ World* InWorld = nullptr);

			void Begin();
			void Update(_In_ const TimeSecondsT DeltaSeconds) {}
			Light& GetLight()
			{
				ETERNAL_ASSERT(_Light);
				return *_Light;
			}
			void SetLight(_In_ Light* InLight)
			{
				_Light = InLight;
			}

			OnTransformChangedEvent& OnTransformChangedReceiver() { return _OnTransformChangedReceiver; }

		private:

			Light* _Light = nullptr;
			LightOnTransformChanged _OnTransformChangedReceiver;

		};
	}
}
