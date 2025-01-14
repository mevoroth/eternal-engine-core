#include "Core/GameState.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace Core
	{
		System& GameState::GetSystem()
		{
			return _Game.GetSystem();
		}

		InputMapping& GameState::GetInputMapping()
		{
			return GetSystem().GetInputMapping();
		}
	}
}
