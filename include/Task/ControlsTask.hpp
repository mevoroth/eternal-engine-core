#ifndef _CONTROLS_TASK_HPP_
#define _CONTROLS_TASK_HPP_

#include <vector>

#include "Parallel/Task.hpp"

using namespace std;

namespace Eternal
{
	namespace Input
	{
		class Input;
	}
	namespace Task
	{
		using namespace Eternal::Parallel;
		
		class ControlsTask : public Task
		{
		public:
			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

			void RegisterInput(_In_ Input::Input* InputObj);

		private:
			vector<Input::Input*> _Inputs;
		};
	}
}

#endif
