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

		struct GraphicsCommandUploadMesh final : public GraphicsCommand
		{
			GraphicsCommandUploadMesh(_In_ MeshPayload& InPayload);

			virtual void Execute(_In_ GraphicsContext& InContext) override final;

		private:
			MeshPayload& _Payload;
		};
	}
}
