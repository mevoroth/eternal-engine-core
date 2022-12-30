#pragma once

#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class OpaquePass final : public ObjectPass
		{
		public:
			static const string OpaquePassName;

			OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);
			~OpaquePass();

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		protected:
			virtual const string& _GetPassName() const override final;
		};
	}
}
