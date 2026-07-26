#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using RenderFunctor = std::function<void(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)>;

		class LambdaPass : public Pass
		{
		public:

			LambdaPass(_In_ const RenderFunctor& InRenderFunction);
			
			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override;
			
			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const;

		protected:

			virtual bool _CanRenderPass() const override;

		private:

			RenderFunctor _RenderFunction = nullptr;

		};
	}
}
