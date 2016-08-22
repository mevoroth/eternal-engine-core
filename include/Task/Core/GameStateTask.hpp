#ifndef _GAME_STATE_TASK_HPP_
#define _GAME_STATE_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Core
	{
		class GameState;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Core;

		class GameStateTask : public Task
		{
		public:
			GameStateTask(_In_ Core::GameState* GameStateObj);

			virtual void Setup();
			virtual void Reset();
			virtual void Execute();

			bool GetRemainingState();

		private:
			Core::GameState* _GameState = nullptr;
			Core::GameState* _ScheduledToBegin = nullptr;
		};
	}
}

#endif
