#include "Core/Game.hpp"
#include "Core/GameState.hpp"
#include "Core/System.hpp"

using namespace std;
using namespace Eternal::Core;

namespace Eternal
{
	namespace Core
	{
		Game::Game(_In_ GameCreateInformation& InGameCreateInformation)
		{
			_System = new System(InGameCreateInformation.SystemInformation);
		}

		Game::~Game()
		{
			delete _CurrentGameState;
			delete _System;
		}

		void Game::Run()
		{
			ETERNAL_ASSERT(_CurrentGameState);

			OPTICK_THREAD();

			_CurrentGameState->Begin();
			while (_Running)
			{
				OPTICK_FRAME("MainThread");
				_CurrentGameState->Update();

				GameState* NextState = _CurrentGameState->NextState();

				if (!NextState)
				{
					_CurrentGameState->End();
					return;
				}

				if (_CurrentGameState != NextState)
				{
					_CurrentGameState->End();
					NextState->Begin();
					_CurrentGameState = NextState;
				}
			}
		}
	}
}
