#include "Task/Core/StreamingTask.hpp"
#include "Core/System.hpp"
#include "Import/ImportLevel.hpp"
#include "Core/Level.hpp"
#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Tasks
	{
		StreamingTask::StreamingTask(_In_ StreamingTaskCreateInformation& InStreamingTaskCreateInformation)
			: _StreamingTaskCreateInformation(InStreamingTaskCreateInformation)
		{
		}

		void StreamingTask::DoExecute()
		{
			//Level* Sponzalevel = GetSystem().GetImportLevel().Import("Sponza.json");
			GetSystem().GetStreaming().AdvanceStreaming();
		}
	}
}
