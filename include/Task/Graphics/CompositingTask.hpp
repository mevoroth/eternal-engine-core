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
		class ContextCollection;
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
		class BlendStateCollection;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;

		class CompositingTask : public Task
		{
		public:
			CompositingTask(_In_ ContextCollection& DeferredContexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& LightingRenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			ContextCollection& _Contexts;
			RenderTargetCollection& _OpaqueRenderTargets;
			RenderTargetCollection& _LightingRenderTargets;
			SamplerCollection& _Samplers;
			ViewportCollection& _Viewports;
			BlendStateCollection& _BlendStates;
			Shader* _VS = nullptr;
			Shader* _PS = nullptr;
		};
	}
}

#endif
