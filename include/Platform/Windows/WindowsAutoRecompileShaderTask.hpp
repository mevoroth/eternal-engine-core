#pragma once

#if ETERNAL_PLATFORM_WINDOWS

#include "Task/Tools/AutoRecompileShaderTask.hpp"

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

		class WindowsAutoRecompileShaderTask final : public AutoRecompileShaderTask
		{
		public:
			WindowsAutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ GraphicsContext& InContext);
			~WindowsAutoRecompileShaderTask();

			virtual void DoExecute() override final;
		};
	}
}

#endif
