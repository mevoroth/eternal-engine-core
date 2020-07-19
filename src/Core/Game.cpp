#include "Core/Game.hpp"

using namespace std;
using namespace Eternal::Core;

Game::Game(_In_ GameState* State)
	: _Current(State)
{
	ETERNAL_ASSERT(_Current);
}

void Game::Run()
{
	ETERNAL_ASSERT(_Current);

	_Current->Begin();
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
