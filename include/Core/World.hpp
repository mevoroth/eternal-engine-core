#pragma once

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class Game;
		class Level;

		class World
		{
		public:

			World(_In_ Game& InGame)
				: _Game(InGame)
			{
			}

			Game& GetGame() { return _Game; }
			void AddLevel(_In_ Level* InLevel);

		private:
			Game&			_Game;
			vector<Level*>	_Levels;
		};
	}
}
