#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "HLSLDirectLighting.hpp"

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
			
			RenderPass*								_DirectLightingRenderPass			= nullptr;
			DescriptorTable*						_DirectLightingDescriptorTable		= nullptr;
			ConstantBuffer<DirectLightingConstants>	_DirectLightingConstantBuffer;
			StructuredBuffer<LightInformation>		_DirectLightingLightsBuffer;
		};
	}
}
