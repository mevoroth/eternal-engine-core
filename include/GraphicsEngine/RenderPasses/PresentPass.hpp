#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class PresentPass : public Pass
		{
		public:
			PresentPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		private:
			RootSignature*		_PresentRootSignature	= nullptr;
			RenderPass*			_PresentRenderPass		= nullptr;
			Pipeline*			_PresentPipeline		= nullptr;
			DescriptorTable*	_PresentDescriptorTable	= nullptr;
		};
	}
}
