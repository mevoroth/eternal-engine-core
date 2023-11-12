#pragma once

namespace Eternal
{
	namespace Core
	{
		class System;
		struct SystemCreateInformation;

		System* CreateSystem(_In_ SystemCreateInformation& InSystemCreateInformation);
	}
}
