#ifndef _GRAPHIC_TASK_HPP_
#define _GRAPHIC_TASK_HPP_

#include <cstdint>
#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Device;
		class Pipeline;
		class Viewport;
		class RenderPass;
		class CommandList;
		class RenderTarget;
		class DepthTest;
		class StencilTest;
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
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;

		class GraphicTaskData;
		class GraphicTask : public Task
		{
		public:
			GraphicTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);
			virtual ~GraphicTask();

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

			virtual bool IsRendered() const = 0;
			virtual void Render(_Inout_ CommandList* CommandListObj) = 0;

			virtual Viewport* GetViewport() = 0;
			virtual Pipeline* GetPipeline() = 0;
			virtual RenderPass* GetRenderPass() = 0;
			virtual RenderTarget* GetRenderTargets() = 0;

			StateSharedData* GetSharedData();
			const StateSharedData* GetSharedData() const;

		private:
			GraphicTaskData* _GraphicTaskData = nullptr;
		};
	}
}

#endif
