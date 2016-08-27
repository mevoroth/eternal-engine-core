#ifndef _PREPARE_SOLID_TASK_HPP_
#define _PREPARE_SOLID_TASK_HPP_

#include <vector>
#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace std;
		using namespace Eternal::Parallel;

		class SolidGBufferTask;
		class PrepareSolidTaskData;

		class PrepareSolidTask : public Task
		{
		public:
			PrepareSolidTask(_In_ SolidGBufferTask& SolidGBufferTaskObj);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			SolidGBufferTask& _SolidGBufferTask;
			PrepareSolidTaskData* _SolidTaskData = nullptr;
		};
	}
}

#endif
