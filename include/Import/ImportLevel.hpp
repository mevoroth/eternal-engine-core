#pragma once

#include <string>

namespace Eternal
{
	namespace Core
	{
		class Level;
	}

	namespace Import
	{
		using namespace std;
		using namespace Eternal::Core;

		class ImportLevel
		{
		public:
			ImportLevel();

			Level* Import(_In_ const string& InName);
		};
	}
}
