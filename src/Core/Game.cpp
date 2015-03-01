#include "Core/Game.hpp"

#include <cassert>

using namespace std;
using namespace Eternal::Core;

Game::Game(GameState* State)
	: _current(State)
{
	assert(_current);
	_current->Begin();
}

void Game::Run()
{
	assert(_current);

	for (;;)
	{
		_current->Update();
		GameState* NextState = _current->NextState();

		if (!NextState)
		{
			_current->End();
			return;
		}

		if (_current != NextState)
		{
			_current->End();
			delete _current;
			NextState->Begin();
			_current = NextState;
		}
	}
}
