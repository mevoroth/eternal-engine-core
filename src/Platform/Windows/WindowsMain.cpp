#if ETERNAL_PLATFORM_WINDOWS

#include "Core/MainInput.hpp"
#include "Core/SystemCreateInformation.hpp"
#include "Graphics/Types.hpp"
#include "Platform/Microsoft/MicrosoftProcess.hpp"
#include "Windows/WindowsArguments.hpp"
#include "Windows/WindowsGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMainInput)
		{
			RenderSettings Settings(
				DeviceType::DEVICE_TYPE_DEFAULT,
				1600, 900,
				/*InIsVSync =*/ true
			);
			WindowsArguments WinArguments(
				static_cast<HINSTANCE>(InMainInput.hInstance),
				static_cast<HINSTANCE>(InMainInput.hPrevInstance),
				static_cast<LPSTR>(InMainInput.lpCmdLine),
				InMainInput.nCmdShow,
				InMainInput.ApplicationName,
				InMainInput.ApplicationName,
				Platform::MicrosoftProcess::WindowProc
			);
			WindowsGraphicsContextCreateInformation* ContextCreateInformation = new WindowsGraphicsContextCreateInformation(Settings, WinArguments);

			return SystemCreateInformation(ContextCreateInformation);
		}
	}
}

#endif
