#include "Platform/Windows/WindowsSystem.hpp"
#include "Core/System.hpp"
#if ETERNAL_USE_PRIVATE
#include "Platform/SystemFactoryPrivate.hpp"
#endif

namespace Eternal
{
	namespace Core
	{
		System* CreateSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
		{
			System* OutSystem = nullptr;

#if ETERNAL_USE_PRIVATE
			OutSystem = CreateSystemPrivate(InSystemCreateInformation);
#endif

#if ETERNAL_PLATFORM_WINDOWS
			OutSystem = new WindowsSystem(InSystemCreateInformation);
#endif

			ETERNAL_ASSERT(OutSystem);
			OutSystem->InitializeSystem();

			return OutSystem;
		}
	}
}
