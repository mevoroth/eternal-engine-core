#ifndef _DEBUG_RENDER_TASK_HPP_
#define _DEBUG_RENDER_TASK_HPP_

#include "Task/NextGenGraphics/GraphicTask.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class SwapChain;
	}

	namespace Task
	{
		using namespace Eternal::Graphics;

		class DebugRenderTaskData;

		class DebugRenderTask : public GraphicTask
		{
		public:
			DebugRenderTask(_In_ Device& DeviceObj, _In_ SwapChain& SwapChainObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual bool IsRendered() const override;
			virtual void Render(_Inout_ CommandList* CommandListObj) override;
			virtual const Viewport* GetViewport() override;
			virtual Pipeline* GetPipeline() override;
			virtual RenderPass* GetRenderPass() override;
			//virtual RenderTarget* GetRenderTargets() override;

		private:
			DebugRenderTaskData*	_DebugRenderTaskData	= nullptr;
		};
	}
}

#endif
