#include "Task/Core/StreamingTask.hpp"
#include "Core/System.hpp"
#include "Import/ImportLevel.hpp"
#include "Core/Level.hpp"
#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Tasks
	{
		void StreamingTask::DoExecute()
		{
			ETERNAL_PROFILER(BASIC)();
			Streaming& StreamingSystem	= GetSystem().GetStreaming();
			RequestQueueType& StreamingRequests = StreamingSystem.GetPendingStreaming().PendingRequests;
			PayloadQueueType& StreamingPayloads = StreamingSystem.GetPendingStreaming().LoadedRequests;

			_ProcessQueues(StreamingRequests, StreamingPayloads);

			StreamingSystem.AdvanceStreaming();
		}

		void StreamingTask::_ProcessQueues(_In_ RequestQueueType& InStreamingRequests, _Inout_ PayloadQueueType& InOutStreamingPayloads)
		{
			Streaming& StreamingSystem = GetSystem().GetStreaming();

			for (uint32_t QueueType = 0; QueueType < InStreamingRequests.size(); ++QueueType)
			{
				vector<StreamingRequest*>& Requests = InStreamingRequests[QueueType];

				const StreamingLoader* Loader = StreamingSystem.GetLoader(static_cast<AssetType>(QueueType));
				for (uint32_t RequestIndex = 0; RequestIndex < Requests.size(); ++RequestIndex)
				{
					InOutStreamingPayloads[QueueType].push_back(nullptr);
					Payload*& LastPayload = InOutStreamingPayloads[QueueType].back();
					Loader->LoadPayload(Requests[RequestIndex], LastPayload);

					_ProcessQueues(LastPayload->AdditionalRequests, InOutStreamingPayloads);

					delete Requests[RequestIndex];
					Requests[RequestIndex] = nullptr;
				}

				Requests.clear();
			}
		}
	}
}
