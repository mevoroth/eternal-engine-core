#ifndef _SWAP_FRAME_TASK_HPP_
#define _SWAP_FRAME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class SwapChain;
	}

	namespace Core
	{
		class StateSharedData;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::Core;

		class SwapFrameTaskData;
		class SwapFrameTask : public Task
		{
		public:
			SwapFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ StateSharedData* SharedData);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			SwapFrameTaskData* _SwapFrameTaskData = nullptr;
		};
	}
}

#endif
