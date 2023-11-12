#include "Platform/Windows/WindowsSystem.hpp"
#include "Platform/SystemFactoryPrivate.hpp"
#include "Core/System.hpp"

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
