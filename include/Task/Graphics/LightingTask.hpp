#ifndef _LIGHTING_TASK_HPP_
#define _LIGHTING_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace GraphicData
	{
		class ContextCollection;
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
	}

	namespace Graphics
	{
		class Context;
	}

	namespace Core
	{
		class StateSharedData;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;
		using namespace Eternal::Core;

		class LightingTaskData;

		class LightingTask : public Task
		{
		public:
			LightingTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData);
			~LightingTask();

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			LightingTaskData* _LightingTaskData = nullptr;

			void _SetupFrameConstants(_In_ Context& ContextObj);
			void _SetupLightConstants(_In_ Context& ContextObj);
		};
	}
}

#endif
