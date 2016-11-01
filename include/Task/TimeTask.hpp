#ifndef _TIME_TASK_HPP_
#define _TIME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Time
	{
		class Time;
	}
	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Time;

		class TimeTask : public Task
		{
		public:
			TimeTask(_In_ Eternal::Time::Time* TimeObj);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			Eternal::Time::Time* _Time = nullptr;
		};
	}
}

#endif
