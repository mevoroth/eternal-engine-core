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
			if (_Pipeline)
				DestroyPipeline(_Pipeline);

			if (_RootSignature)
				DestroyRootSignature(_RootSignature);
		}

		bool Pass::CanRenderPass() const
		{
			return _IsPassEnabled && _CanRenderPass();
		}

		bool Pass::_CanRenderPass() const
		{
			return _Pipeline && _Pipeline->IsPipelineCompiled();
		}
	}
}
