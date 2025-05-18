#include "GraphicsEngine/RendererPBR.hpp"
#include "GraphicData/GlobalResourcesPBR.hpp"
#include "GraphicsEngine/RenderPasses/SkyPass.hpp"
#include "GraphicsEngine/RenderPasses/ShadowMapPass.hpp"
#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "GraphicsEngine/RenderPasses/DistantObjectPass.hpp"
#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "GraphicsEngine/RenderPasses/RayTracedReflectionsPass.hpp"
#include "GraphicsEngine/RenderPasses/VolumetricCloudsPass.hpp"
#include "GraphicsEngine/RenderPasses/TonemappingPass.hpp"
#include "GraphicsEngine/RenderPasses/DebugRenderPasses/DebugObjectBoundingBoxPass.hpp"
#include "GraphicsEngine/RenderPasses/DebugRenderPasses/DebugRayTracingPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		RendererPBR::RendererPBR(_In_ GraphicsContext& InContext)
			: Renderer(
				InContext,
				new GlobalResourcesPBR(InContext),
				[](_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer) -> vector<Pass*>
				{
					return {
						new SkyPass(InContext, InRenderer),
						new ShadowMapPass(InContext, InRenderer),
						new OpaquePass(InContext, InRenderer),
						new DistantObjectPass(InContext, InRenderer),
						new DirectLightingPass(InContext, InRenderer),
						new RayTracedReflectionsPass(InContext, InRenderer),
						new DebugObjectBoundingBoxPass(InContext, InRenderer),
						new VolumetricCloudsPass(InContext, InRenderer),
						new TonemappingPass(InContext, InRenderer),
						new DebugRayTracingPass(InContext, InRenderer)
					};
				}
			)
		{
		}
	}
}
