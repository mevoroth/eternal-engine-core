#include "Task/Graphics/InitFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Fence.hpp"
#include "Graphics/SwapChain.hpp"
#include "Core/StateSharedData.hpp"
#include "NextGenGraphics/Context.hpp"
#include "Log/Log.hpp"

#include "include/RenderDoc.h"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class InitFrameTaskData
		{
		public:
			InitFrameTaskData(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FrameFence, _In_ StateSharedData* SharedData)
				: _Device(DeviceObj)
				, _SwapChain(SwapChainObj)
				, _FrameFence(FrameFence)
				, _SharedData(SharedData)
			{
			}

			Device& GetDevice()
			{
				return _Device;
			}

			SwapChain& GetSwapChain()
			{
				return _SwapChain;
			}

			Fence& GetFrameFence()
			{
				return _FrameFence;
			}

			StateSharedData* GetSharedData()
			{
				return _SharedData;
			}

		private:
			Device&				_Device;
			SwapChain&			_SwapChain;
			Fence&				_FrameFence;
			StateSharedData*	_SharedData = nullptr;
		};
	}
}

InitFrameTask::InitFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FenceObj, _In_ StateSharedData* SharedData)
	: _InitFrameTaskData(new InitFrameTaskData(DeviceObj, SwapChainObj, FenceObj, SharedData))
{
}

InitFrameTask::~InitFrameTask()
{
}

void InitFrameTask::DoSetup()
{
	static bool once = false;
	if (!once)
	{
		RenderDoc::TriggerCapture();
		once = true;
	}
}

void InitFrameTask::DoExecute()
{
	Device& DeviceObj			= _InitFrameTaskData->GetDevice();
	SwapChain& SwapChainObj		= _InitFrameTaskData->GetSwapChain();
	StateSharedData* SharedData	= _InitFrameTaskData->GetSharedData();

	Fence* FrameFence = SharedData->GfxContexts[SharedData->CurrentFrame]->GetFrameFence();

	SwapChainObj.AcquireFrame(DeviceObj, *SharedData->GfxContexts[SharedData->CurrentFrame]);

	//Eternal::Log::Log::Get()->Write(Eternal::Log::Log::Warning, Eternal::Log::Log::Graphics, "InitFrameTask Wait");
	FrameFence->Wait(DeviceObj);
	//Eternal::Log::Log::Get()->Write(Eternal::Log::Log::Warning, Eternal::Log::Log::Graphics, "InitFrameTask Reset");
	FrameFence->Reset(DeviceObj);
	SharedData->Reset();
	///*SharedData->CurrentFrame = */SwapChainObj.AcquireFrame(DeviceObj, FrameFence);
}

void InitFrameTask::DoReset()
{
}
