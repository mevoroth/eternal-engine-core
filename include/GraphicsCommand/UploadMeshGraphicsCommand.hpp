#pragma once

#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Resources
	{
		struct MeshPayload;
	}
	namespace GraphicsCommands
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Resources;

		struct UploadMeshGraphicsCommand final : public GraphicsCommand
		{
			UploadMeshGraphicsCommand(_In_ MeshPayload& InPayload);

			virtual void Execute(_In_ GraphicsContext& InContext) override final;

		private:
			MeshPayload& _Payload;
		};
	}
}
