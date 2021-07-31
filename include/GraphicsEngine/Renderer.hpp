#pragma once

#include <vector>

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

			Renderer(_In_ GraphicsContext& InContext);
			~Renderer();

			void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem);
			void Present(_In_ GraphicsContext& InContext, _In_ System& InSystem);

			GlobalResources& GetGlobalResources()
			{
				ETERNAL_ASSERT(_GlobalResources);
				return *_GlobalResources;
			}

		private:

			GlobalResources*	_GlobalResources	= nullptr;
			vector<Pass*>		_Passes;
			Pass*				_PresentPass		= nullptr;
		};
	}
}
