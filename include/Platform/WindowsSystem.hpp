#pragma once

#if ETERNAL_PLATFORM_WINDOWS

#include "Core/System.hpp"
#include "Platform/WindowsProcess.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Platform;

		class WindowsSystem : public System
		{
		public:
			WindowsSystem(_In_ SystemCreateInformation& InSystemCreateInformation);

			virtual void UpdatePlatform() override;

		private:
			WindowsProcess _WindowProcess;
		};
	}
}

#endif
