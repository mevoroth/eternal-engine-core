#pragma once

#include "Core/World.hpp"
#include "Core/SystemCreateInformation.hpp"

namespace Eternal
{
	namespace Time
	{
		using TimeSecondsT = double;
	}
	namespace Core
	{
		using namespace Eternal::Time;

		class Game;
		class GameState;
		class System;

		struct GameCreateInformation
		{
			GameCreateInformation(_In_ SystemCreateInformation&& InSystemCreateInformation)
				: SystemInformation(std::move(InSystemCreateInformation))
			{
			}

			SystemCreateInformation SystemInformation;
		};

		void* AndroidApplicationEntry(_In_ void* InAndroidApplication);
		void RunGame(_In_ GameCreateInformation& InGameCreateInformation, _Inout_ Game* InOutGame);

		class Game
		{
			friend System;
		public:
			Game(_In_ GameCreateInformation&& InGameCreateInformation);
			virtual ~Game();

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
			void Run();

			void Update();
			void UpdateDebug();
			void ProcessStreamingPayloads();

			GameCreateInformation	_GameCreateInformation;

			World					_World;

			GameState*				_CurrentGameState	= nullptr;

			System*					_System				= nullptr;
			bool					_Running			= true;
			TimeSecondsT			_ElapsedTime		= 0.0;
			TimeSecondsT			_DeltaSeconds		= 0.0;
			TimeSecondsT			_TimeDilation		= 1.0;

			friend void* AndroidApplicationEntry(_In_ void* InAndroidApplication);
			friend void RunGame(_In_ GameCreateInformation& InGameCreateInformation, _Inout_ Game* InOutGame);
		};

		template<typename GameStateType>
		class StartGame : public Game
		{
		public:
			StartGame(_In_ GameCreateInformation&& InGameCreateInformation)
				: Game(std::move(InGameCreateInformation))
			{
				_CurrentGameState = new GameStateType(static_cast<Game&>(*this));
			}

			void RunGame()
			{
				Eternal::Core::RunGame(_GameCreateInformation, this);
			}
		};
	}
}
