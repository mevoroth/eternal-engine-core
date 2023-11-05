#include "Task/Tools/AutoRecompileShaderTask.hpp"

namespace Eternal
{
	namespace Tasks
	{
		AutoRecompileShaderTask::AutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ GraphicsContext& InContext)
			: Task(InTaskCreateInformation)
			, _Context(InContext)
		{
		}

		AutoRecompileShaderTask::~AutoRecompileShaderTask()
		{
			delete _AutoRecompileShaderData;
			_AutoRecompileShaderData = nullptr;
		}
	}
}
