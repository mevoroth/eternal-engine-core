#pragma once

#if ETERNAL_PLATFORM_WINDOWS || ETERNAL_PLATFORM_SCARLETT

#include "Core/System.hpp"
#include "Platform/Microsoft/MicrosoftProcess.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Platform;

		class MicrosoftSystem : public System
		{
		public:

			virtual void UpdatePlatform() override;

		protected:

			MicrosoftSystem(_In_ SystemCreateInformation& InSystemCreateInformation);

		private:

			MicrosoftProcess _MicrosoftProcess;
		};
	}
}

#endif
