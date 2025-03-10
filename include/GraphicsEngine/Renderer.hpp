#pragma once

#include "GraphicsEngine/MipMapGeneration.hpp"
#include "NextGenGraphics/FrameGraph.hpp"
#include "NextGenGraphics/StencilTracker.hpp"
#include <vector>
#include <functional>

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
	}

	namespace GraphicData
	{
		class GlobalResources;
	}

	namespace Core
	{
		class System;
	}

	namespace GraphicsEngine
	{
		using namespace std;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicData;
		using namespace Eternal::Core;

		class Pass;

		class Renderer
		{
		public:

			static constexpr bool UseFrameGraph = false;

			virtual ~Renderer();

			void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem);
			void Present(_In_ GraphicsContext& InContext, _In_ System& InSystem);
			void RenderDebug();

			GlobalResources& GetGlobalResources()
			{
				ETERNAL_ASSERT(_GlobalResources);
				return *_GlobalResources;
			}

			StencilTracker& GetStencilTracker() { return _StencilTracker; }
			MipMapGeneration& GetMipMapGeneration() { return _MipMapGeneration; }

		protected:

			Renderer(_In_ GraphicsContext& InContext, _In_ GlobalResources* InGlobalResources, _In_ const std::function<vector<Pass*>(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)>& InPassPopulateFunction);

			FrameGraph			_FrameGraph;
			StencilTracker		_StencilTracker;
			MipMapGeneration	_MipMapGeneration;
			GlobalResources*	_GlobalResources	= nullptr;
			vector<Pass*>		_Passes;
			Pass*				_PresentPass		= nullptr;
		};
	}
}
