#pragma once

#include "Core/World.hpp"

namespace Eternal
{
	namespace Core
	{
		class GameState;
		class System;

		struct SystemCreateInformation;

		struct GameCreateInformation
		{
			GameCreateInformation(_In_ SystemCreateInformation& InSystemCreateInformation)
				: SystemInformation(InSystemCreateInformation)
			{
			}

			SystemCreateInformation& SystemInformation;
		};

		class Game
		{
			friend System;
		public:
			Game(_In_ GameCreateInformation& InGameCreateInformation);
			virtual ~Game();
			void Run();

			System& GetSystem()
			{
				ETERNAL_ASSERT(_System);
				return *_System;
			}

		protected:
			void Update();

			World		_World;

			GameState*	_CurrentGameState	= nullptr;

			System*		_System				= nullptr;
			bool		_Running			= true;
		};

		template<typename GameStateType>
		class StartGame : public Game
		{
		public:
			StartGame(_In_ GameCreateInformation& InGameCreateInformation)
				: Game(InGameCreateInformation)
			{
				_CurrentGameState = new GameStateType(static_cast<Game&>(*this));
			}
		};
	}
}
