#pragma once

#include "Parallel/Task.hpp"
#include <vector>

namespace Eternal
{
	namespace Tasks
	{
		using namespace std;
		using namespace Eternal::Parallel;

		struct AutoRecompileShaderPrivateData;

		class AutoRecompileShaderTask final : public Task
		{
		public:
			AutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation);
			~AutoRecompileShaderTask();

			virtual void DoExecute() override final;

		private:

			AutoRecompileShaderPrivateData* _AutoRecompileShaderData = nullptr;
		};
	}
}
