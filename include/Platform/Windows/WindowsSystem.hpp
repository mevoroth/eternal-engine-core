#pragma once

#if ETERNAL_PLATFORM_WINDOWS

#include "Platform/Microsoft/MicrosoftSystem.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Platform;

		class WindowsSystem : public MicrosoftSystem
		{
		public:

			WindowsSystem(_In_ SystemCreateInformation& InSystemCreateInformation);

		};
	}
}

#endif
