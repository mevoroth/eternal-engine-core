#ifndef _INIT_FRAME_TASK_HPP_
#define _INIT_FRAME_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Renderer;
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
		using namespace Eternal::GraphicData;

		class InitFrameTask : public Task
		{
		public:
			InitFrameTask(_In_ Renderer& RendererObj, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection** RenderTargetCollections, _In_ int RenderTargetsCollectionsCount);
			~InitFrameTask();

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			Renderer& _Renderer;

			ContextCollection& _Contexts;
			RenderTargetCollection** _RenderTargetCollections;
			int _RenderTargetCollectionsCount = 0;
		};
	}
}

#endif
