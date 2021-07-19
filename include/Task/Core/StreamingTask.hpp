#pragma once

#include "Parallel/Task.hpp"
#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Core
	{
		class System;
	}
	namespace Tasks
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Core;
		using namespace Eternal::Resources;

		class StreamingTask final : public Task
		{
		public:
			using Task::Task;

			virtual void DoExecute() override final;

		private:

			void _ProcessQueues(_In_ RequestQueueType& InStreamingRequests, _Inout_ PayloadQueueType& InOutStreamingPayloads);
		};
	}
}
