#include "Core/Game.hpp"

#if ETERNAL_PLATFORM_ANDROID

namespace Eternal
{
	namespace Core
	{
		template<typename GameStateType>
		void StartGame<GameStateType>::RunGame()
		{
			ETERNAL_BREAK();
		}
	}
}

#endif
