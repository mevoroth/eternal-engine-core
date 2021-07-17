#include "Core/Game.hpp"
#include "Core/GameState.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace Core
	{
		Game::Game(_In_ GameCreateInformation& InGameCreateInformation)
		{
			InGameCreateInformation.SystemInformation.GameContext = this;
			_System = new System(InGameCreateInformation.SystemInformation);
		}

		Game::~Game()
		{
			delete _System;
			_System = nullptr;
		}

		void Game::Run()
		{
			ETERNAL_ASSERT(_CurrentGameState);

			OPTICK_THREAD("MainThread");

			_CurrentGameState->Begin();
			while (_Running)
			{
				OPTICK_FRAME("GameFrame");
				GetSystem().StartFrame();
				GetSystem().Update();
				{
					ETERNAL_PROFILER(BASIC)("GameState Update");
					_CurrentGameState->Update();
				}
				{
					ETERNAL_PROFILER(BASIC)("GameState Debug Update");
					_CurrentGameState->UpdateDebug();
				}
				GetSystem().EndFrame();

				{
					ETERNAL_PROFILER(BASIC)("GameState End");
					GameState* NextState = _CurrentGameState->NextState();

					if (!NextState)
					{
						_CurrentGameState->End();
						_CurrentGameState = nullptr;
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
}
