#include "Task/NextGenGraphics/GraphicTask.hpp"

#include <vector>
#include "Macros/Macros.hpp"
#include "NextGenGraphics/Device.hpp"
#include "Core/StateSharedData.hpp"
#include "Graphics/CommandQueueFactory.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/CommandListFactory.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/CommandQueues.hpp"

using namespace Eternal::Task;
using namespace Eternal::GraphicData;

namespace Eternal
{
	namespace Task
	{
		using namespace std;
		using namespace Eternal::Graphics;

		class GraphicTaskData
		{
		public:
			GraphicTaskData(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
				: _SharedData(SharedData)
			{
				_CommandLists.resize(FRAME_LAG);
				for (uint32_t CommandListIndex = 0; CommandListIndex < _CommandLists.size(); ++CommandListIndex)
					_CommandLists[CommandListIndex] = CreateCommandList(DeviceObj, COMMAND_LIST_TYPE_GRAPHIC);
			}

			CommandList*		GetCommandList()	{ return _CommandLists[_SharedData->CurrentFrame]; }
			StateSharedData*	GetSharedData()		{ return _SharedData; }

		private:
			vector<CommandList*>	_CommandLists;
			StateSharedData*		_SharedData		= nullptr;
		};
	}
}

GraphicTask::GraphicTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: _GraphicTaskData(new GraphicTaskData(DeviceObj, Resources, SharedData))
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

	//ETERNAL_ASSERT(false);
	//CommandList* CommandListObj = _GraphicTaskData->GetCommandQueue()->Pop();
	CommandList* CommandListObj = _GraphicTaskData->GetCommandList();
	//CommandAllocator* CommandAllocatorObj = _GraphicTaskData->GetCommandQueue()->GetCommandAllocator(GetSharedData()->CurrentFrame);
	const Viewport& ViewportObj = *GetViewport();

	CommandListObj->Begin(*GetPipeline());
	CommandListObj->BeginRenderPass(*GetRenderPass());
	CommandListObj->SetViewport(ViewportObj);
	CommandListObj->SetScissorRectangle(ViewportObj);

	Render(CommandListObj);

	CommandListObj->EndRenderPass();
	CommandListObj->End();

	_GraphicTaskData->GetSharedData()->RecordCommandList(CommandListObj);
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
