#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "GameState.hpp"

namespace Eternal
{
	namespace Core
	{
		class Game
		{
		public:
			Game(GameState* State);
			void Run();
		private:
			GameState* _Current;
		};
	}
}

#endif
