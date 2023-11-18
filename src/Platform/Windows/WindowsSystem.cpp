#if ETERNAL_PLATFORM_WINDOWS

#include "Platform/Windows/WindowsSystem.hpp"
#include "Platform/Windows/WindowsAutoRecompileShaderTask.hpp"
#include "Windows/WindowsGraphicsContext.hpp"
#include "Core/Game.hpp"

namespace Eternal
{
	namespace Core
	{
		WindowsSystem::WindowsSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{
			WindowsProcess::SetInputHandler(_Input);
			WindowsProcess::SetIsRunning(InSystemCreateInformation.GameContext->GetRunningPointer());

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
