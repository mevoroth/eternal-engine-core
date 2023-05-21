#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class SkyPass final : public Pass
		{
		public:
			static float AtmospherePhaseG[];
			static float AtmospherePhaseBlend;

			SkyPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~SkyPass();
			
			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;
			virtual void RenderDebug() override final;

		private:
			DescriptorTable*					_SkyDescriptorTable	= nullptr;
			RenderPass*							_SkyRenderPass		= nullptr;
		};
	}
}
