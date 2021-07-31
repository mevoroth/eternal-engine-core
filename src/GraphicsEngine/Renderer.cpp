#include "GraphicsEngine/Renderer.hpp"
#include "GraphicData/GlobalResources.hpp"
#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "GraphicsEngine/RenderPasses/PresentPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		static void DestroyPass(_Inout_ Pass*& InOutPass)
		{
			delete InOutPass;
			InOutPass = nullptr;
		}

		Renderer::Renderer(_In_ GraphicsContext& InContext)
			: _GlobalResources(new GlobalResources(InContext))
			, _Passes({
				new OpaquePass(InContext, *this),
				new DirectLightingPass(InContext, *this)
			})
			, _PresentPass(new PresentPass(InContext, *this))
		{
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
			if (_GlobalResources->BeginRender(InSystem))
			{
				for (uint32_t PassIndex = 0; PassIndex < _Passes.size(); ++PassIndex)
					_Passes[PassIndex]->Render(InContext, InSystem, *this);
			}
		}

		void Renderer::Present(_In_ GraphicsContext& InContext, _In_ System& InSystem)
		{
			_PresentPass->Render(InContext, InSystem, *this);
		}
	}
}
