#ifndef _SWAP_FRAME_TASK_HPP_
#define _SWAP_FRAME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Renderer;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;

		class SwapFrameTask : public Task
		{
		public:
			SwapFrameTask(_In_ Renderer& RendererObj);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			Renderer& _Renderer;
		};
	}
}

#endif
