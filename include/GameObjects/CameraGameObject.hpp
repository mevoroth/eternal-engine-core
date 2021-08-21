#pragma once

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace GameObjects
	{
		using namespace Eternal::Core;

		class CameraGameObject final : public GameObject
		{
		public:

			CameraGameObject();

			virtual void Update(const TimeSecondsT InDeltaSeconds) override final;
			virtual void UpdateDebug() override final;
		};
	}
}
