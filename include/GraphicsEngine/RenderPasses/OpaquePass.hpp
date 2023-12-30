#pragma once

#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		class OpaquePass final : public ObjectPass
		{
		public:
			static constexpr uint32_t OpaquePassInstancesCount = 1024u;
			static const string OpaquePassName;

			OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

		protected:
			virtual const string& _GetPassName() const override final;
		};
	}
}
