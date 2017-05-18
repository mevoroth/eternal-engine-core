#include "Task/NextGenGraphics/FinalizeFrameTask.hpp"

#include "Core/StateSharedData.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/CommandQueues.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/SwapChain.hpp"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class FinalizeFrameTaskData
		{
		public:
			FinalizeFrameTaskData(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
				: _Device(DeviceObj)
				, _SwapChain(SwapChainObj)
				, _SharedData(SharedData)
				, _GraphicResources(Resources)
			{
			}

			StateSharedData*	GetSharedData()			{ return _SharedData; }
			GraphicResources*	GetGraphicResources()	{ return _GraphicResources; }
			Device&				GetDevice()				{ return _Device; }
			SwapChain&			GetSwapChain()			{ return _SwapChain; }

		private:
			StateSharedData*	_SharedData			= nullptr;
			GraphicResources*	_GraphicResources	= nullptr;
			Device&				_Device;
			SwapChain&			_SwapChain;
		};
	}
}

FinalizeFrameTask::FinalizeFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: _FinalizeFrameTaskData(new FinalizeFrameTaskData(DeviceObj, SwapChainObj, Resources, SharedData))
{
}

FinalizeFrameTask::~FinalizeFrameTask()
{
	delete _FinalizeFrameTaskData;
	_FinalizeFrameTaskData = nullptr;
}

void FinalizeFrameTask::DoSetup()
{
}

void FinalizeFrameTask::DoExecute()
{
	uint32_t CurrentFrame			= _FinalizeFrameTaskData->GetSharedData()->CurrentFrame;
	CommandQueue* MainCommandQueue	= _FinalizeFrameTaskData->GetGraphicResources()->GetCommandQueues()->Get(COMMAND_QUEUE_PRESENT);
	SwapChain& SwapChainObj			= _FinalizeFrameTaskData->GetSwapChain();
	Device& DeviceObj				= _FinalizeFrameTaskData->GetDevice();
	
	SwapChainObj.Present(DeviceObj, *MainCommandQueue, CurrentFrame);
}

void FinalizeFrameTask::DoReset()
{
}
