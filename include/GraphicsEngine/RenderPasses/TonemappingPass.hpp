#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class TonemappingPass : public Pass
		{
		public:
			TonemappingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~TonemappingPass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		private:

			RootSignature* _TonemappingRootSignature		= nullptr;
			Pipeline* _TonemappingPipeline					= nullptr;
			DescriptorTable* _TonemappingDescriptorTable	= nullptr;

		};
	}
}
