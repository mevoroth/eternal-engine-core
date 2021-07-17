#pragma once

#include "Parallel/Task.hpp"

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

		struct StreamingTaskCreateInformation
		{

			StreamingTaskCreateInformation(_In_ System& InSystem)
				: SystemContext(InSystem)
			{
			}

			System& SystemContext;
		};

		class StreamingTask final : public Task
		{
		public:
			StreamingTask(_In_ StreamingTaskCreateInformation& InStreamingTaskCreateInformation);

			virtual void DoExecute() override final;

		private:
			System& GetSystem() { return _StreamingTaskCreateInformation.SystemContext; }

			StreamingTaskCreateInformation _StreamingTaskCreateInformation;
		};
	}
}
