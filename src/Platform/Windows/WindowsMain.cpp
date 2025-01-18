#include "Core/Main.hpp"

#if ETERNAL_PLATFORM_WINDOWS

#include "Graphics/Types.hpp"
#include "Platform/Microsoft/MicrosoftProcess.hpp"
#include "Windows/WindowsArguments.hpp"
#include "Windows/WindowsGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMaintInput)
		{
			RenderSettings Settings(
				DeviceType::DEVICE_TYPE_DEFAULT,
				1600, 900,
				/*InIsVSync =*/ true
			);
			WindowsArguments WinArguments(
				static_cast<HINSTANCE>(InMaintInput.hInstance),
				static_cast<HINSTANCE>(InMaintInput.hPrevInstance),
				static_cast<LPSTR>(InMaintInput.lpCmdLine),
				InMaintInput.nCmdShow,
				InMaintInput.ApplicationName,
				InMaintInput.ApplicationName,
				Platform::MicrosoftProcess::WindowProc
			);
			WindowsGraphicsContextCreateInformation* ContextCreateInformation = new WindowsGraphicsContextCreateInformation(Settings, WinArguments);

			return SystemCreateInformation(ContextCreateInformation);
		}
	}
}

#endif
