#include "Task/Core/StreamingTask.hpp"
#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Tasks
	{
		StreamingTask::StreamingTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ Streaming& InStreamingSystem)
			: Task(InTaskCreateInformation)
			, _StreamingSystem(InStreamingSystem)
		{
		}

		void StreamingTask::DoExecute()
		{
			ETERNAL_PROFILER(BASIC)();
			_StreamingSystem.CommitEnqueued_StreamingThread();
			_StreamingSystem.ProcessPending_StreamingThread();
			_StreamingSystem.AdvanceStreaming_StreamingThread();
		}
	}
}
