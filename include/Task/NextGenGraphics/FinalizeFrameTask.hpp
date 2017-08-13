#ifndef _FINALIZE_FRAME_HPP_
#define _FINALIZE_FRAME_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Device;
		class SwapChain;
		class Fence;
	}

	namespace GraphicData
	{
		class GraphicResources;
	}

	namespace Core
	{
		class StateSharedData;
	}

	namespace Task
	{
		using namespace Eternal::Core;
		using namespace Eternal::Graphics;
		using namespace Eternal::Parallel;
		using namespace Eternal::GraphicData;

		class FinalizeFrameTaskData;
		class FinalizeFrameTask : public Task
		{
		public:
			FinalizeFrameTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ Fence& FrameFence, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);
			virtual ~FinalizeFrameTask();

			virtual void DoSetup() override;
			virtual void DoExecute() override;
			virtual void DoReset() override;

		private:
			FinalizeFrameTaskData* _FinalizeFrameTaskData = nullptr;
		};
	}
}

#endif
