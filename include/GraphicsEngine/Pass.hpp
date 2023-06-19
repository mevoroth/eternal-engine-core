#pragma once

#include "GraphicsEngine/Renderer.hpp"
#include "GraphicData/GlobalResources.hpp"
#include "Graphics/GraphicsInclude.hpp"

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

			virtual ~Pass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) = 0;
			virtual void RenderDebug() {}

			bool CanRenderPass() const;

		protected:

			RootSignature*	_RootSignature	= nullptr;
			Pipeline*		_Pipeline		= nullptr;

		};
	}
}
