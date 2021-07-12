#pragma once

namespace Eternal
{
	namespace Core
	{
		class Game;
		class System;

		class GameState
		{
		public:
			GameState(_In_ Game& InGame)
				: _Game(InGame)
			{
			}

			virtual ~GameState() {}
			virtual void Begin() = 0;
			virtual void Update() = 0;
			virtual GameState* NextState() = 0;
			virtual void End() = 0;

		protected:
			Game& GetGame() { return _Game; }
			System& GetSystem();

		private:
			Game& _Game;
		};
	}
}
