#include "Core/Game.hpp"

#include "Macros/Macros.hpp"

using namespace std;
using namespace Eternal::Core;

Game::Game(GameState* State)
	: _Current(State)
{
	ETERNAL_ASSERT(_Current);
	_Current->Begin();
}

void Game::Run()
{
	ETERNAL_ASSERT(_Current);

	for (;;)
	{
		_Current->Update();
		GameState* NextState = _Current->NextState();

		if (!NextState)
		{
			_Current->End();
			return;
		}

		if (_Current != NextState)
		{
			_Current->End();
			NextState->Begin();
			_Current = NextState;
		}
	}
}
