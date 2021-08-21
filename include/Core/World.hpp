#pragma once

#include <vector>

namespace Eternal
{
	namespace Time
	{
		using TimeSecondsT = double;
	}
	namespace Core
	{
		using namespace std;
		using namespace Eternal::Time;

		class Game;
		class Level;
		class Component;

		class World
		{
		public:

			static constexpr uint32_t ComponentsPerGameObjectInitialPool = 4;
			static constexpr uint32_t LevelsInitialPool = 4;
			static constexpr uint32_t ComponentsAddedCountInitialPool = 128;
			static constexpr uint32_t ComponentsToUpdateCountInitialPool = 4096;

			World(_In_ Game& InGame);

			Game& GetGame() { return _Game; }
			void AddLevel(_In_ Level* InLevel);

			void Update(_In_ TimeSecondsT InDeltaSeconds);
			void UpdateDebug();

		private:
			void _UpdateComponents(_In_ TimeSecondsT InDeltaSeconds);

			Game&				_Game;
			vector<Level*>		_Levels;
		};
	}
}
