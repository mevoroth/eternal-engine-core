#if ETERNAL_PLATFORM_ANDROID

#include "Core/MainInput.hpp"
#include "Core/SystemCreateInformation.hpp"
#include "Graphics/Types.hpp"
#include "Android/AndroidGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMainInput)
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
