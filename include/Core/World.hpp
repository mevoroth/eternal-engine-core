#pragma once

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class Level;

		class World
		{
		public:

			void AddLevel(_In_ Level* InLevel)
			{
				_Levels.push_back(InLevel);
			}

		private:
			vector<Level*> _Levels;
		};
	}
}
