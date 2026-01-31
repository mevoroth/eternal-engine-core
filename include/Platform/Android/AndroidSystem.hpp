#pragma once

#if ETERNAL_PLATFORM_ANDROID

#include "Core/System.hpp"

struct android_app;

namespace Eternal
{
	namespace Core
	{
		class AndroidSystem : public System
		{
		public:

			static void AndroidLooperProcess(_In_ android_app* InAndroidApplication);

			AndroidSystem(_In_ SystemCreateInformation& InSystemCreateInformation);

			virtual void UpdatePlatform() override;

		};
	}
}

#endif
