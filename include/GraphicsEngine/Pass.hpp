#pragma once

namespace Eternal
{
	namespace Graphics
	{
		class GraphicsContext;
		class RootSignature;
		class InputLayout;
		class RenderPass;
		class Pipeline;
		class DescriptorTable;
	}

	namespace Core
	{
		class System;
	}

	namespace GraphicsEngine
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Core;

		class Renderer;

		class Pass
		{
		public:

			virtual ~Pass() {}

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) = 0;
		};
	}
}
