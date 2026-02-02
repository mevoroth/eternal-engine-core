#if ETERNAL_PLATFORM_ANDROID

#include "Platform/Android/AndroidSystem.hpp"
#include "Graphics/GraphicsContextFactory.hpp"
#include "Android/AndroidGraphicsContext.hpp"
#include "Input/InputFactory.hpp"
#include <android/native_app_glue/android_native_app_glue.h>

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::InputSystem;

		std::vector<InputType> System::InputChannels = { InputType::INPUT_TYPE_TOUCH };

		void AndroidSystem::AndroidLooperProcess(_In_ android_app* InAndroidApplication)
		{
			android_poll_source* AndroidPollSource	= nullptr;
			int FileDescriptor						= 0;
			int Events								= 0;

			int AndroidLooperPollOnceResult = ALooper_pollOnce(-1, &FileDescriptor, &Events, reinterpret_cast<void**>(&AndroidPollSource));

			switch (AndroidLooperPollOnceResult)
			{
			case ALOOPER_POLL_WAKE:
			{
				return;
			} break;
			case ALOOPER_POLL_CALLBACK:
			{
			} break;
			case ALOOPER_POLL_TIMEOUT:
			{
			} break;
			case ALOOPER_POLL_ERROR:
			{
			} break;
			default:
			{
				if (AndroidLooperPollOnceResult < 0)
				{
					ETERNAL_BREAK();
					return;
				}
			} break;
			}

			AndroidPollSource->process(InAndroidApplication, AndroidPollSource);
		}

		AndroidSystem::AndroidSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{
			_GraphicsContext = CreateGraphicsContext(*static_cast<const AndroidGraphicsContextCreateInformation*>(InSystemCreateInformation.ContextInformation));
		}

		void AndroidSystem::UpdatePlatform()
		{
			AndroidLooperProcess(reinterpret_cast<android_app*>(_SystemCreateInformation.ExecutableInput.AndroidNativeActivity->instance));
		}
	}
}

#endif
