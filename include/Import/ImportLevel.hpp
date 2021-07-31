#pragma once

#include <string>

namespace Eternal
{
	namespace Resources
	{
		struct LevelPayload;
	}
	namespace Import
	{
		using namespace std;
		using namespace Eternal::Resources;

		class ImportLevel
		{
		public:
			ImportLevel();

			LevelPayload* Import(_In_ const string& InName);
		};
	}
}
