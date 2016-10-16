#ifndef _SWAP_FRAME_TASK_HPP_
#define _SWAP_FRAME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Renderer;
		class Context;
	}

	namespace GraphicData
	{
		class ContextCollection;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;

		class SwapFrameTask : public Task
		{
		public:
			SwapFrameTask(_In_ Renderer& RendererObj, _In_ Context& MainContext, _In_ ContextCollection& DeferredContexts);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			Renderer& _Renderer;

			Context& _MainContext;
			ContextCollection& _DeferredContexts;
		};
	}
}

#endif
