#pragma once

#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class PresentPass final : public Pass
		{
		public:
			PresentPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~PresentPass();
			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		private:
			DescriptorTable*	_PresentDescriptorTable	= nullptr;
		};
	}
}
