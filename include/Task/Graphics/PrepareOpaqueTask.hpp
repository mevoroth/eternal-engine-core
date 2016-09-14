#ifndef _PREPARE_OPAQUE_TASK_HPP_
#define _PREPARE_OPAQUE_TASK_HPP_

#include <vector>
#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Constant;
	}

	namespace Task
	{
		using namespace std;
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;

		class OpaqueTask;
		class PrepareOpaqueTaskData;

		class PrepareOpaqueTask : public Task
		{
		public:
			PrepareOpaqueTask(_In_ OpaqueTask& OpaqueTaskObj);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			OpaqueTask& _OpaqueTask;
			PrepareOpaqueTaskData* _PrepareOpaqueTaskData = nullptr;
		};
	}
}

#endif
