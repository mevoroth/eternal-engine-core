#ifndef _LIGHT_COMPONENT_HPP_
#define _LIGHT_COMPONENT_HPP_

#include "Core/GameComponent.hpp"
#include "Light/Shadow.hpp"

namespace Eternal
{
	namespace Components
	{
		class Light;
	}

	namespace Core
	{
		using namespace Eternal::Components;

		class TransformComponent;

		class LightComponent : public GameComponent
		{
		public:
			static void Initialize();
			static void Release();

			virtual void Begin() override;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) override;
			virtual void End() override;

			void SetLight(_In_ Light* LightObj);
			Light* GetLight();

			Shadow* GetShadow();
			void SetShadow(_In_ Shadow* ShadowObj);

			bool GetShadowEnabled() const;

			void AttachTo(_In_ TransformComponent* TransformComponentObj);

		private:
			TransformComponent* _TransformComponent = nullptr;
			Light* _Light = nullptr;
			Shadow* _Shadow = nullptr;
		};
	}
}

#endif
