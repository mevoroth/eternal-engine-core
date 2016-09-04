#ifndef _COMPOSITING_TASK_HPP_
#define _COMPOSITING_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Context;
		class Shader;
	}

	namespace GraphicData
	{
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;

		class CompositingTask : public Task
		{
		public:
			CompositingTask(_In_ Context& MainContext, _In_ Context** DeferredContexts, _In_ int DeferredContextCount, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			Context& _MainContext;
			Context** _DeferredContexts;
			int _DeferredContextsCount = 0;
			RenderTargetCollection& _RenderTargets;
			SamplerCollection& _Samplers;
			ViewportCollection& _Viewports;
			Shader* _VS = nullptr;
			Shader* _PS = nullptr;
		};
	}
}

#endif
