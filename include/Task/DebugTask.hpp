#ifndef _DEBUG_TASK_HPP_
#define _DEBUG_TASK_HPP_

#include "Parallel/Task.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	namespace GraphicData
	{
		class ContextCollection;
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
		class StateSharedData;
	}
	namespace Task
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::Parallel;
		using namespace Eternal::Types;

		class DebugTaskData;

		class DebugTask : public Task
		{
		public:
			DebugTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			DebugTaskData*  _DebugTaskData = nullptr;

			void _DrawGizmo();
		};
	}
}

#endif
