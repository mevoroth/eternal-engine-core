#ifndef _IMGUI_END_TASK_HPP_
#define _IMGUI_END_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Context;
		class Shader;
		class RenderTarget;
		class Constant;
		class Sampler;
		class Texture;
		class BlendState;
		class Viewport;
	}
	namespace GraphicData
	{
		class SamplerCollection;
		class ViewportCollection;
	}
	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;

		class ImguiEndTask : public Task
		{
		public:
			ImguiEndTask(_In_ Context& ContextObj, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports);

			virtual void Setup() override;
			virtual void Execute() override;
			virtual void Reset() override;

			void SetRenderTarget(_In_ RenderTarget* RenderTargetObj)
			{
				_RenderTarget = RenderTargetObj;
			}

		private:
			Context& _Context;

			Shader* _VS = nullptr;
			Shader* _PS = nullptr;

			RenderTarget* _RenderTarget = nullptr;

			Constant* _ViewProjectionMatrixConstant = nullptr;
			Texture* _Texture = nullptr;
			Sampler* _Sampler = nullptr;
			BlendState* _BlendState = nullptr;
			Viewport* _Viewport = nullptr;
		};
	}
}

#endif
