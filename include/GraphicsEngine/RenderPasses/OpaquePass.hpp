#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class OpaquePass final : public Pass
		{
		public:

			OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~OpaquePass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		private:
			RootSignature*		_OpaqueRootSignature	= nullptr;
			InputLayout*		_OpaqueInputLayout		= nullptr;
			RenderPass*			_OpaqueRenderPass		= nullptr;
			Pipeline*			_OpaquePipeline			= nullptr;
			DescriptorTable*	_OpaqueDescriptorTable	= nullptr;
		};
	}
}
