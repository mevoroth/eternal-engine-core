#include "Task/NextGenGraphics/FinalizeFrameTask.hpp"

#include "Core/StateSharedData.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/CommandQueues.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/SwapChain.hpp"
#include "Graphics/Fence.hpp"
#include "NextGenGraphics/Context.hpp"
#include "Log/Log.hpp"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class FinalizeFrameTaskData
		{
		public:
			FinalizeFrameTaskData(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FrameFence, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
				: _Device(DeviceObj)
				, _SwapChain(SwapChainObj)
				, _FrameFence(FrameFence)
				, _SharedData(SharedData)
				, _GraphicResources(Resources)
			{
			}

			StateSharedData*	GetSharedData()			{ return _SharedData; }
			GraphicResources*	GetGraphicResources()	{ return _GraphicResources; }
			Device&				GetDevice()				{ return _Device; }
			SwapChain&			GetSwapChain()			{ return _SwapChain; }
			Fence&				GetFrameFence()			{ return _FrameFence; }

		private:
			StateSharedData*	_SharedData			= nullptr;
			GraphicResources*	_GraphicResources	= nullptr;
			Device&				_Device;
			SwapChain&			_SwapChain;
			Fence&				_FrameFence;
		};
	}
}

FinalizeFrameTask::FinalizeFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FrameFence, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: _FinalizeFrameTaskData(new FinalizeFrameTaskData(DeviceObj, SwapChainObj, FrameFence, Resources, SharedData))
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
	//ETERNAL_ASSERT(false);
	StateSharedData* SharedData			= _FinalizeFrameTaskData->GetSharedData();
	uint32_t CurrentFrame				= SharedData->CurrentFrame;
	GCommandQueue* MainGCommandQueue	= _FinalizeFrameTaskData->GetGraphicResources()->GetCommandQueues()->Get(COMMAND_QUEUE_GRAPHIC);
	SwapChain& SwapChainObj				= _FinalizeFrameTaskData->GetSwapChain();
	Device& DeviceObj					= _FinalizeFrameTaskData->GetDevice();
	//Fence& FrameFence					= _FinalizeFrameTaskData->GetFrameFence();
	
	//CommandList* CommandListObj		= MainGCommandQueue->Queue->Pop();

	Context* GfxContext = SharedData->GfxContexts[CurrentFrame];
	//GfxContext->GetFrameFence()

	GCommandQueue* GraphicCommandQueue	= _FinalizeFrameTaskData->GetGraphicResources()->GetCommandQueues()->Get(COMMAND_QUEUE_GRAPHIC);
	CommandList** RecordedCommandLists	= SharedData->RecordedCommandLists[CurrentFrame];
	int CommandListsCount				= SharedData->CommandListsCount[CurrentFrame]->Load();
	//CommandQueue0->Queue->Submit(CurrentFrame, RecordedCommandLists, CommandListsCount > 1 ? 1 : CommandListsCount, *GfxContext->GetFrameFence(), SwapChainObj);
	//Eternal::Log::Log::Get()->Write(Eternal::Log::Log::Warning, Eternal::Log::Log::Graphics, "FinalizeFrameTask Submit");
	GraphicCommandQueue->Queue->Submit(*GfxContext, RecordedCommandLists, CommandListsCount);
	//Eternal::Log::Log::Get()->Write(Eternal::Log::Log::Warning, Eternal::Log::Log::Graphics, "FinalizeFrameTask Signal");
	GfxContext->GetFrameFence()->Signal(*MainGCommandQueue->Queue);
	SwapChainObj.Present(DeviceObj, *GraphicCommandQueue->Queue, *GfxContext);

	_FinalizeFrameTaskData->GetSharedData()->CurrentFrame = (_FinalizeFrameTaskData->GetSharedData()->CurrentFrame + 1) % FRAME_LAG;
}

void FinalizeFrameTask::DoReset()
{
}
