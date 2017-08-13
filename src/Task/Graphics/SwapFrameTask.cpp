#include "Task/Graphics/SwapFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/SwapChain.hpp"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class SwapFrameTaskData
		{
		public:
			SwapFrameTaskData(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ CommandQueue& MainCommandQueue, _In_ StateSharedData* SharedData)
				: _Device(DeviceObj)
				, _SwapChain(SwapChainObj)
				, _MainCommandQueue(MainCommandQueue)
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

			CommandQueue& GetMainCommandQueue()
			{
				return _MainCommandQueue;
			}

			StateSharedData* GetSharedData()
			{
				return _SharedData;
			}

		private:
			Device&				_Device;
			SwapChain&			_SwapChain;
			CommandQueue&		_MainCommandQueue;
			StateSharedData*	_SharedData = nullptr;
		};
	}
}

SwapFrameTask::SwapFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ StateSharedData* SharedData)
	: _SwapFrameTaskData(new SwapFrameTaskData(DeviceObj, SwapChainObj, SharedData))
{
}

void SwapFrameTask::DoSetup()
{
}

void SwapFrameTask::DoExecute()
{
	SwapChain& SwapChainObj = _SwapFrameTaskData->GetSwapChain();
	SwapChainObj.Present()
}

void SwapFrameTask::DoReset()
{
}
