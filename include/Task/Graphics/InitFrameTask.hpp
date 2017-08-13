#ifndef _INIT_FRAME_TASK_HPP_
#define _INIT_FRAME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Device;
		class SwapChain;
		class Fence;
	}

	namespace Core
	{
		class StateSharedData;
	}

	namespace GraphicData
	{
		class ContextCollection;
		class RenderTargetCollection;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::Core;

		class InitFrameTaskData;
		class InitFrameTask : public Task
		{
		public:
			InitFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FenceObj, _In_ StateSharedData* SharedData);
			~InitFrameTask();

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			InitFrameTaskData* _InitFrameTaskData = nullptr;
		};
	}
}

#endif
