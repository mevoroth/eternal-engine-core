#include "Core/GameState.hpp"
#include "Core/Game.hpp"

namespace Eternal
{
	namespace Core
	{
		System& GameState::GetSystem()
		{
			return _Game.GetSystem();
		}
	}
}
