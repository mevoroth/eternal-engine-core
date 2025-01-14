#pragma once

namespace Eternal
{
	namespace InputSystem
	{
		class InputMapping;
	}
	namespace Core
	{
		using namespace Eternal::InputSystem;

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
			virtual void End() { delete this; }
			virtual void UpdateDebug() {}

		protected:
			Game& GetGame() { return _Game; }
			System& GetSystem();
			InputMapping& GetInputMapping();

		private:
			Game& _Game;
		};
	}
}
