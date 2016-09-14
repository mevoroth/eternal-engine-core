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

		class CameraComponent : public GameComponent
		{
		public:
			virtual void Begin() override;
			virtual void Update(_In_ const TimeT& ElapsedMicroSeconds) override;
			virtual void End() override;

			Camera* GetCamera();

		private:
			Camera* _Camera = nullptr;
		};
	}
}

#endif
