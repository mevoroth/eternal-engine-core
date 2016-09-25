#ifndef _OPAQUE_TASK_
#define _OPAQUE_TASK_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Components
	{
		class Mesh;
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
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
		class BlendStateCollection;
	}

	namespace Core
	{
		class StateSharedData;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Components;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;
		using namespace Eternal::Core;
		using namespace std;

		class OpaqueTaskData;

		class OpaqueTask : public Task
		{
		public:
			OpaqueTask(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData);
			~OpaqueTask();

			Constant* GetCommonConstant();
			Viewport* GetViewport();
			Sampler* GetSampler();

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			OpaqueTaskData* _OpaqueTaskData = nullptr;

			void _SetupCommonConstants(_In_ Context& ContextObj);
			void _SetupObjectConstants(_In_ Context& ContextObj, _In_ Mesh& MeshObj);
			void _Draw(_In_ Mesh& MeshObj);
		};
	}
}

#endif
