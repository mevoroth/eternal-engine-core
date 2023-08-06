#pragma once

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Resources
	{
		class Streaming;
	}
	namespace Tasks
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Resources;

		class StreamingTask final : public Task
		{
		public:
			StreamingTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ Streaming& InStreamingSystem);

			virtual void DoExecute() override final;

		private:
			Streaming& _StreamingSystem;
		};
	}
}
