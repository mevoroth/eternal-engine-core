#ifndef _GAME_STATE_TASK_HPP_
#define _GAME_STATE_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Core
	{
		class GameState;
		class StateSharedData;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Core;

		class GameStateTask : public Task
		{
		public:
			GameStateTask(_In_ Core::GameState* GameStateObj, _In_ StateSharedData* SharedData);

			virtual void DoSetup();
			virtual void DoReset();
			virtual void DoExecute();

			bool GetRemainingState();

		private:
			Core::GameState* _GameState = nullptr;
			Core::GameState* _ScheduledToBegin = nullptr;
			StateSharedData* _SharedData = nullptr;
		};
	}
}

#endif
