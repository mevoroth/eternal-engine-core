#pragma once

#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Resources
	{
		struct TexturePayload;
		class TextureFactory;
	}
	namespace GraphicsCommands
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Resources;

		struct GraphicsCommandUploadTexture final : public GraphicsCommand
		{
			GraphicsCommandUploadTexture(_In_ TexturePayload& InPayload, _In_ TextureFactory& InTextureFactory);

			virtual void Execute(_In_ GraphicsContext& InContext) override final;

		private:
			TexturePayload& _Payload;
			TextureFactory& _Factory;
		};
	}
}
