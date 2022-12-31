#include "GraphicsEngine/RenderPasses/ShadowMapPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		const string ShadowMapPass::ShadowMapPassName = "ShadowMapPass";

		ShadowMapPass::ShadowMapPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: DepthOnlyPass(
				InContext,
				InRenderer,
				InRenderer.GetGlobalResources().GetShadowMap().GetRenderTargetDepthStencilView()
			)
		{
		}

		const string& ShadowMapPass::_GetPassName() const
		{
			return ShadowMapPassName;
		}
	}
}
