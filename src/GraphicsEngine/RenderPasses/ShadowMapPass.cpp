#include "GraphicsEngine/RenderPasses/ShadowMapPass.hpp"
#include "Core/System.hpp"
#include "Light/Light.hpp"
#include "GraphicData/ViewGraphicData.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		constexpr uint32_t ShadowMapPass::ShadowMapPassInstancesCount;
		const string ShadowMapPass::ShadowMapPassName = "ShadowMapPass";

		ShadowMapPass::ShadowMapPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: DepthOnlyPass(
				InContext,
				InRenderer,
				ShadowMapPassInstancesCount,
				InRenderer.GetGlobalResources().GetShadowMapViewport(),
				InRenderer.GetGlobalResources().GetShadowMap().GetRenderTargetDepthStencilView(),
				InRenderer.GetGlobalResources().GetShadowMapViewConstantBufferView()
			)
		{
		}

		void ShadowMapPass::_BeginRender(_In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			const vector<ObjectsList<Light>::InstancedObjects>& Lights = InSystem.GetRenderFrame().Lights;
			if (Lights.size() > 0 && Lights[0].Object->GetShadow().ShadowCamera)
			{
				UploadViewCameraToBuffer(
					InRenderer.GetGlobalResources().GetShadowMapViewConstantBuffer(),
					Lights[0].Object->GetShadow().ShadowCamera,
					static_cast<float>(InGlobalResources.GetShadowMapViewport().GetWidth()),
					static_cast<float>(InGlobalResources.GetShadowMapViewport().GetHeight())
				);
			}
		}

		const string& ShadowMapPass::_GetPassName() const
		{
			return ShadowMapPassName;
		}
	}
}
