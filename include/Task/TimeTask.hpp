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

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			Eternal::Time::Time* _Time = nullptr;
		};
	}
}

#endif
