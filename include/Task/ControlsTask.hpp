#ifndef _CONTROLS_TASK_HPP_
#define _CONTROLS_TASK_HPP_

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
			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

			void RegisterInput(_In_ Input::Input* InputObj);

		private:
			vector<Input::Input*> _Inputs;
		};
	}
}

#endif
