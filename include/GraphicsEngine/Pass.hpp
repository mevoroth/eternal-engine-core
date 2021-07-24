#pragma once

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
		class RootSignature;
		class InputLayout;
		class RenderPass;
	}

	namespace Core
	{
		class System;
	}

	namespace GraphicsEngine
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Core;

		class Pass
		{
		public:

			virtual ~Pass() {}

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem) = 0;
		};
	}
}
