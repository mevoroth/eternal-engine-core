#ifndef _OPAQUE_TASK_
#define _OPAQUE_TASK_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Components
	{
		class Mesh;
		class Camera;
	}

	namespace Graphics
	{
		class Context;
		class Constant;
		class Viewport;
		class Sampler;
	}
	
	namespace GraphicData
	{
		class GraphicObjects;
		class ContextCollection;
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
		class BlendStateCollection;
		class GraphicTaskConfig;
	}

	namespace Core
	{
		class StateSharedData;
		class GraphicGameObject;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Components;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;
		using namespace Eternal::Core;
		using namespace std;

		class RenderObjectsTaskData;

		class RenderObjectsTask : public Task
		{
		public:
			static constexpr int MAX_INSTANCES = 4096;

			RenderObjectsTask(_In_ const GraphicTaskConfig& Config, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData);
			~RenderObjectsTask();

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			RenderObjectsTaskData* _RenderObjectsTaskData = nullptr;

			void _SetupFrameConstants(_In_ Context& ContextObj, _In_ Camera* CameraObj);
			void _SetupInstanceStructuredBuffer(_In_ Context& ContextObj, _In_ GraphicGameObject* GraphicObject);
			void _SetupObjectConstants(_In_ Context& ContextObj, _In_ Mesh& MeshObj);
			void _Draw(_In_ Context& ContextObj, _In_ Mesh& MeshObj, _In_ int InstanceCount);
		};
	}
}

#endif
