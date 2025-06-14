#pragma once

#include "Core/World.hpp"

namespace Eternal
{
	namespace Time
	{
		using TimeSecondsT = double;
	}
	namespace Core
	{
		using namespace Eternal::Time;

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

			World& GetWorld()
			{
				return _World;
			}

			bool* GetRunningPointer() { return &_Running; }

			template<typename GameStateType>
			GameStateType* GetGameState()
			{
				return static_cast<GameStateType*>(_CurrentGameState);
			}

			TimeSecondsT GetGameDeltaSeconds() const;

			void PauseGame();

		protected:
			void Update();
			void UpdateDebug();
			void ProcessStreamingPayloads();

			World			_World;

			GameState*		_CurrentGameState	= nullptr;

			System*			_System				= nullptr;
			bool			_Running			= true;
			TimeSecondsT	_ElapsedTime		= 0.0;
			TimeSecondsT	_DeltaSeconds		= 0.0;
			TimeSecondsT	_TimeDilation		= 1.0;
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
