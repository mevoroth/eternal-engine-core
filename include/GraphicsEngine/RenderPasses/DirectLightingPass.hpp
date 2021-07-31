#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class DirectLightingPass final : public Pass
		{
		public:

			DirectLightingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~DirectLightingPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		private:
			
			RootSignature*		_DirectLightingRootSignature	= nullptr;
			RenderPass*			_DirectLightingRenderPass		= nullptr;
			Pipeline*			_DirectLightingPipeline			= nullptr;
			DescriptorTable*	_DirectLightingDescriptorTable	= nullptr;

		};
	}
}
