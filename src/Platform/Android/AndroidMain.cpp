#include "Core/Main.hpp"

#if ETERNAL_PLATFORM_ANDROID

#include "Graphics/Types.hpp"
#include "Android/AndroidGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMaintInput)
		{
			RenderSettings Settings(
				DeviceType::DEVICE_TYPE_DEFAULT,
				1600, 900,
				/*InIsVSync =*/ false
			);
			AndroidGraphicsContextCreateInformation* ContextCreateInformation = new AndroidGraphicsContextCreateInformation(Settings);

			return SystemCreateInformation(ContextCreateInformation);
		}
	}
}

#endif
