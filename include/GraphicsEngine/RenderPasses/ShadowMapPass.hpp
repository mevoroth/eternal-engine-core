#pragma once

#include "GraphicsEngine/RenderPasses/DepthOnlyPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class ShadowMapPass final : public DepthOnlyPass
		{
		public:
			static const string ShadowMapPassName;

			ShadowMapPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

		protected:
			virtual const string& _GetPassName() const override final;
		};
	}
}
