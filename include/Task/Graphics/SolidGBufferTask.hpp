#ifndef _SOLIDGBUFFER_TASK_
#define _SOLIDGBUFFER_TASK_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Parallel;

		class SolidGBufferTask : public Task
		{
		public:
			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;
		};
	}
}

#endif
