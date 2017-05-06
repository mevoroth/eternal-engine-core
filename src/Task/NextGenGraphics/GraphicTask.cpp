#include "Task/NextGenGraphics/GraphicTask.hpp"

#include "Macros/Macros.hpp"
#include "NextGenGraphics/Device.hpp"
#include "Graphics/CommandQueueFactory.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/CommandQueue.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/CommandQueues.hpp"

using namespace Eternal::Task;
using namespace Eternal::GraphicData;

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Graphics;

		class GraphicTaskData
		{
		public:
			GraphicTaskData(_In_ Device& DeviceObj, _In_ GraphicResources* Resources)
			{
				_CommandQueue = Resources->GetCommandQueues()->Get(GRAPHIC_QUEUE_0);
			}

			CommandQueue*	GetCommandQueue()	{ return _CommandQueue; }

		private:
			CommandQueue*	_CommandQueue	= nullptr;
		};
	}
}

uint32_t GraphicTask::CurrentFrame = 0u;

void GraphicTask::SetCurrentFrame(_In_ uint32_t CurrentFrame)
{
	GraphicTask::CurrentFrame = CurrentFrame;
}

GraphicTask::GraphicTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources)
	: _GraphicTaskData(new GraphicTaskData(DeviceObj, Resources))
{
}

GraphicTask::~GraphicTask()
{
	delete _GraphicTaskData;
	_GraphicTaskData = nullptr;
}

void GraphicTask::DoSetup()
{

}

void GraphicTask::DoExecute()
{
	CommandList* CommandListObj = _GraphicTaskData->GetCommandQueue()->Pop();
	CommandAllocator* CommandAllocatorObj = _GraphicTaskData->GetCommandQueue()->GetCommandAllocator(GraphicTask::CurrentFrame);
	Viewport& ViewportObj = *GetViewport();
	
	CommandListObj->Begin(*CommandAllocatorObj, *GetPipeline());
	CommandListObj->BeginRenderPass(*GetRenderPass(), *GetRenderTargets(), ViewportObj);

	Render(CommandListObj);

	CommandListObj->EndRenderPass();
	CommandListObj->End();
}

void GraphicTask::DoReset()
{

}
