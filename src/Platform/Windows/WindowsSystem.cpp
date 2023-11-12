#if ETERNAL_PLATFORM_WINDOWS

#include "Platform/WindowsSystem.hpp"
#include "Platform/WindowsAutoRecompileShaderTask.hpp"

namespace Eternal
{
	namespace Core
	{
		WindowsSystem::WindowsSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{
			WindowsProcess::SetInputHandler(_Input);
			WindowsProcess::SetIsRunning(&InSystemCreateInformation.GameContext->_Running);

			_GraphicsContext = CreateGraphicsContext(static_cast<const WindowsGraphicsContextCreateInformation&>(InSystemCreateInformation.ContextInformation));

			TaskCreateInformation AutoRecompileShaderTaskCreateInformation("AutoRecompileShaderTask");
			_AutoRecompileShaderTask = new WindowsAutoRecompileShaderTask(AutoRecompileShaderTaskCreateInformation, *_GraphicsContext);
		}

		void WindowsSystem::UpdatePlatform()
		{
			ETERNAL_PROFILER(BASIC)();
			WindowsProcess::ExecuteMessageLoop();
		}
	}
}

#endif
