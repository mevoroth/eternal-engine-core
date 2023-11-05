#pragma once

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
	}

	namespace Tasks
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;

		struct AutoRecompileShaderPrivateData;

		class AutoRecompileShaderTask : public Task
		{
		public:
			AutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ GraphicsContext& InContext);
			~AutoRecompileShaderTask();

		protected:

			AutoRecompileShaderPrivateData* _AutoRecompileShaderData = nullptr;
			GraphicsContext& _Context;
		};
	}
}
