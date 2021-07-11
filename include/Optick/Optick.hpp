#pragma once

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
	}
	namespace Optick
	{
		using namespace Eternal::Graphics;

		void OptickStart(_In_ GraphicsContext& InContext);
		void OptickStop();

	}
}
