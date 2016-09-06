#ifndef _SOLIDGBUFFER_TASK_
#define _SOLIDGBUFFER_TASK_

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

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Components;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;
		using namespace std;

		class SolidGBufferData;

		class SolidGBufferTask : public Task
		{
		public:
			SolidGBufferTask(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates);

			Constant* GetCommonConstant();
			Viewport* GetViewport();
			Sampler* GetSampler();
			void SetGraphicObjects(_In_ GraphicObjects& Objects);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			SolidGBufferData* _SolidGBufferData = nullptr;

			void _Draw(_In_ Mesh& MeshObj);
		};
	}
}

#endif
