#include "Task/Graphics/InitFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Fence.hpp"
#include "Graphics/SwapChain.hpp"
#include "Core/StateSharedData.hpp"
#include "NextGenGraphics/Context.hpp"

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
}

void InitFrameTask::DoExecute()
{
	Device& DeviceObj			= _InitFrameTaskData->GetDevice();
	SwapChain& SwapChainObj		= _InitFrameTaskData->GetSwapChain();
	StateSharedData* SharedData	= _InitFrameTaskData->GetSharedData();

	Fence* FrameFence = SharedData->GfxContexts[SharedData->CurrentFrame]->GetFrameFence();

	FrameFence->Wait(DeviceObj);
	FrameFence->Reset(DeviceObj);
	SwapChainObj.AcquireFrame(DeviceObj, *SharedData->GfxContexts[SharedData->CurrentFrame]);
	SharedData->Reset();
	///*SharedData->CurrentFrame = */SwapChainObj.AcquireFrame(DeviceObj, FrameFence);
}

void InitFrameTask::DoReset()
{
}
