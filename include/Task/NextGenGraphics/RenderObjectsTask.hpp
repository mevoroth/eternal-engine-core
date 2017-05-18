#ifndef _RENDER_OBJECT_TASK_HPP_
#define _RENDER_OBJECT_TASK_HPP_

#include "Task/NextGenGraphics/GraphicTask.hpp"

namespace Eternal
{
	namespace Core
	{
		class StateSharedData;
	}

	namespace GraphicData
	{
		class GraphicResources;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Core;

		class RenderObjectsTaskData;
		class RenderObjectsTask : public GraphicTask
		{
		public:
			RenderObjectsTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);

			virtual void Render(_Inout_ CommandList* CommandListObj) override;

			virtual Viewport* GetViewport() override;
			virtual Pipeline* GetPipeline() override;
			virtual RenderPass* GetRenderPass() override;
			virtual RenderTarget* GetRenderTargets() override;

		protected:
			RenderObjectsTaskData* GetRenderObjectsTaskData() { return _RenderObjectsTaskData; }
			const RenderObjectsTaskData* GetRenderObjectsTaskData() const { return _RenderObjectsTaskData; }

		private:
			RenderObjectsTaskData* _RenderObjectsTaskData = nullptr;
		};

		class RenderOpaqueObjectsTask : public RenderObjectsTask
		{
		public:
			RenderOpaqueObjectsTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);
			virtual bool IsRendered() const override;
		};
	}
}

#endif
