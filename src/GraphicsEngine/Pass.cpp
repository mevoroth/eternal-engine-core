#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		Renderer* Pass::StaticRenderer = nullptr;

		void Pass::RegisterRenderer(_In_ Renderer* InRenderer)
		{
			ETERNAL_ASSERT(!StaticRenderer);
			StaticRenderer = InRenderer;
		}

		Pass::~Pass()
		{
			DestroyPipeline(_Pipeline);
			DestroyRootSignature(_RootSignature);
		}

		bool Pass::CanRenderPass() const
		{
			return _IsPassEnabled && _Pipeline && _Pipeline->IsPipelineCompiled();
		}
	}
}
