#if ETERNAL_PLATFORM_WINDOWS || ETERNAL_PLATFORM_SCARLETT

#include "Platform/Microsoft/MicrosoftSystem.hpp"
#include "Core/Game.hpp"

namespace Eternal
{
	namespace Core
	{
		MicrosoftSystem::MicrosoftSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{
			MicrosoftProcess::SetInputHandler(_Input);
			MicrosoftProcess::SetIsRunning(InSystemCreateInformation.GameContext->GetRunningPointer());
		}

		void MicrosoftSystem::UpdatePlatform()
		{
			ETERNAL_PROFILER(BASIC)();
			MicrosoftProcess::ExecuteMessageLoop();
		}
	}
}

#endif
