#pragma once

#include "Parallel/Task.hpp"
#include <vector>

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
	}

	namespace Tasks
	{
		using namespace std;
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;

		struct AutoRecompileShaderPrivateData;

		class AutoRecompileShaderTask final : public Task
		{
		public:
			AutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ GraphicsContext& InContext);
			~AutoRecompileShaderTask();

			virtual void DoExecute() override final;

		private:

			AutoRecompileShaderPrivateData* _AutoRecompileShaderData = nullptr;
			GraphicsContext& _Context;
		};
	}
}
