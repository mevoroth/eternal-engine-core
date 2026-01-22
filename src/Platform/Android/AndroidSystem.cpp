#if ETERNAL_PLATFORM_ANDROID

#include "Platform/Android/AndroidSystem.hpp"
#include "Graphics/GraphicsContextFactory.hpp"
#include "Android/AndroidGraphicsContext.hpp"
#include <android/native_app_glue/android_native_app_glue.h>

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;

		AndroidSystem::AndroidSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{
			_GraphicsContext = CreateGraphicsContext(*static_cast<const AndroidGraphicsContextCreateInformation*>(InSystemCreateInformation.ContextInformation));
		}

		void AndroidSystem::UpdatePlatform()
		{
			android_poll_source* AndroidPollSource	= nullptr;
			int FileDescriptor						= 0;
			int Events								= 0;

			if (ALooper_pollOnce(0, &FileDescriptor, &Events, reinterpret_cast<void**>(&AndroidPollSource)) < 0)
			{
				ETERNAL_BREAK();
				return;
			}

			AndroidPollSource->process(reinterpret_cast<android_app*>(_SystemCreateInformation.ExecutableInput.AndroidNativeActivity->instance), AndroidPollSource);
		}
	}
}

#endif
