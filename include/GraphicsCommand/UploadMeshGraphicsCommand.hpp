#pragma once

#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Resources
	{
		class MeshPayload;
	}
	namespace GraphicsCommands
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Resources;

		struct UploadMeshGraphicsCommand final : public GraphicsCommand
		{
			UploadMeshGraphicsCommand(_In_ MeshPayload& InPayload);

			virtual void Execute(_In_ GraphicsContext& InContext, _In_ CommandList& InCommandList) override final;

		private:
			MeshPayload& _Payload;
		};
	}
}
