#pragma once

#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		class System;
	}
	namespace Resources
	{
		struct TexturePayload;
		class TextureFactory;
	}
	namespace GraphicsCommands
	{
		using namespace Eternal::Core;
		using namespace Eternal::Graphics;
		using namespace Eternal::Resources;

		struct GraphicsCommandUploadTexture final : public GraphicsCommand
		{
			GraphicsCommandUploadTexture(_In_ TexturePayload& InPayload, _In_ TextureFactory& InTextureFactory, _In_ System& InSystem);

			virtual void Execute(_In_ GraphicsContext& InContext) override final;

		private:
			TexturePayload&	_Payload;
			TextureFactory&	_Factory;
			System&			_System;
		};
	}
}
