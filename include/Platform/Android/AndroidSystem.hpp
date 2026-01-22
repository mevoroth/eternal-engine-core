#pragma once

#include "Core/System.hpp"

#if ETERNAL_PLATFORM_ANDROID

namespace Eternal
{
	namespace Core
	{
		class AndroidSystem : public System
		{
		public:

			AndroidSystem(_In_ SystemCreateInformation& InSystemCreateInformation);

			virtual void UpdatePlatform() override;

		};
	}
}

#endif
