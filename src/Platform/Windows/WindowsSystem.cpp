#if ETERNAL_PLATFORM_WINDOWS

#include "Platform/Windows/WindowsSystem.hpp"
#include "Platform/Windows/WindowsAutoRecompileShaderTask.hpp"
#include "Windows/WindowsGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		WindowsSystem::WindowsSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: MicrosoftSystem(InSystemCreateInformation)
		{
			_GraphicsContext = CreateGraphicsContext(*static_cast<const WindowsGraphicsContextCreateInformation*>(InSystemCreateInformation.ContextInformation));

			TaskCreateInformation AutoRecompileShaderTaskCreateInformation("AutoRecompileShaderTask");
			_AutoRecompileShaderTask = new WindowsAutoRecompileShaderTask(AutoRecompileShaderTaskCreateInformation, *_GraphicsContext);
		}
	}
}

#endif
