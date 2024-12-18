#include "GraphicsEngine/Renderer.hpp"
#include "GraphicData/GlobalResources.hpp"
#include "GraphicsEngine/RenderPasses/SkyPass.hpp"
#include "GraphicsEngine/RenderPasses/ShadowMapPass.hpp"
#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "GraphicsEngine/RenderPasses/DistantObjectPass.hpp"
#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "GraphicsEngine/RenderPasses/RayTracedReflectionsPass.hpp"
#include "GraphicsEngine/RenderPasses/VolumetricCloudsPass.hpp"
#include "GraphicsEngine/RenderPasses/TonemappingPass.hpp"
#include "GraphicsEngine/RenderPasses/PresentPass.hpp"
#include "GraphicsEngine/RenderPasses/Debug/DebugObjectBoundingBoxPass.hpp"
#include "GraphicsEngine/RenderPasses/Debug/DebugRayTracingPass.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		constexpr bool Renderer::UseFrameGraph;

		static void DestroyPass(_Inout_ Pass*& InOutPass)
		{
			delete InOutPass;
			InOutPass = nullptr;
		}

		Renderer::Renderer(_In_ GraphicsContext& InContext)
			: _MipMapGeneration(InContext)
			, _GlobalResources(new GlobalResources(InContext))
			, _Passes({
				new SkyPass(InContext, *this),
				new ShadowMapPass(InContext, *this),
				new OpaquePass(InContext, *this),
				new DistantObjectPass(InContext, *this),
				new DirectLightingPass(InContext, *this),
				new RayTracedReflectionsPass(InContext, *this),
				new DebugObjectBoundingBoxPass(InContext, *this),
				new VolumetricCloudsPass(InContext, *this),
				new TonemappingPass(InContext, *this),
				new DebugRayTracingPass(InContext, *this)
			})
			, _PresentPass(new PresentPass(InContext, *this))
		{
			GraphicsContext::SetOnStencilWriteFunctor(
				[this](_In_ uint32_t InStencilBit, _In_ const string& InOwner)
				{
					_StencilTracker.AcquireStencil(InStencilBit, InOwner);
				}
			);
			Pass::RegisterRenderer(this);

			if (UseFrameGraph)
			{
				for (uint32_t PassIndex = 0; PassIndex < _Passes.size(); ++PassIndex)
				{
					if (_Passes[PassIndex]->CanRenderPass())
						_FrameGraph.RegisterGraphPass(_Passes[PassIndex]);
				}
			}
		}

		Renderer::~Renderer()
		{
			for (uint32_t PassIndex = 0; PassIndex < _Passes.size(); ++PassIndex)
				DestroyPass(_Passes[PassIndex]);

			delete _GlobalResources;
			_GlobalResources = nullptr;
		}

		void Renderer::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem)
		{
			ETERNAL_PROFILER(BASIC)("Frame");
			_StencilTracker.Validate();
			if (_GlobalResources->BeginRender(InContext, InSystem))
			{
				if (UseFrameGraph)
				{
					_FrameGraph.RunGraph(
						[this, &InContext, &InSystem](_In_ FrameGraphPass* InPass)
						{
							static_cast<Pass*>(InPass)->Render(InContext, InSystem, *this);
						}
					);
				}
				else
				{
					for (uint32_t PassIndex = 0; PassIndex < _Passes.size(); ++PassIndex)
					{
						if (_Passes[PassIndex]->CanRenderPass())
							_Passes[PassIndex]->Render(InContext, InSystem, *this);
					}
				}
			}
		}

		void Renderer::Present(_In_ GraphicsContext& InContext, _In_ System& InSystem)
		{
			_StencilTracker.Validate();
			_PresentPass->Render(InContext, InSystem, *this);
		}

		void Renderer::RenderDebug()
		{
			for (uint32_t PassIndex = 0; PassIndex < _Passes.size(); ++PassIndex)
				_Passes[PassIndex]->RenderDebug();
			_PresentPass->RenderDebug();
		}
	}
}
