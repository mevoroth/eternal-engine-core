#include "Task/NextGenGraphics/GraphicTask.hpp"

#include "Macros/Macros.hpp"
#include "NextGenGraphics/Device.hpp"
#include "Core/StateSharedData.hpp"
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
				_CommandQueue		= Resources->GetCommandQueues()->Get(COMMAND_QUEUE_GRAPHIC_0);
			}

			CommandQueue*		GetCommandQueue()	{ return _CommandQueue; }
			StateSharedData*	GetSharedData()		{ return _SharedData; }

		private:
			CommandQueue*		_CommandQueue	= nullptr;
			StateSharedData*	_SharedData		= nullptr;
		};
	}
}

GraphicTask::GraphicTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
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
	if (!IsRendered())
		return;

	CommandList* CommandListObj = _GraphicTaskData->GetCommandQueue()->Pop();
	CommandAllocator* CommandAllocatorObj = _GraphicTaskData->GetCommandQueue()->GetCommandAllocator(GetSharedData()->CurrentFrame);
	Viewport& ViewportObj = *GetViewport();
	
	CommandListObj->Begin(*CommandAllocatorObj, *GetPipeline());
	CommandListObj->BeginRenderPass(*GetRenderPass());

	Render(CommandListObj);

	CommandListObj->EndRenderPass();
	CommandListObj->End();
}

void GraphicTask::DoReset()
{

}

StateSharedData* GraphicTask::GetSharedData()
{
	return _GraphicTaskData->GetSharedData();
}

const StateSharedData* GraphicTask::GetSharedData() const
{
	return _GraphicTaskData->GetSharedData();
}
