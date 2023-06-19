#include "GraphicsEngine/Pass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		Pass::~Pass()
		{
			DestroyPipeline(_Pipeline);
			DestroyRootSignature(_RootSignature);
		}

		bool Pass::CanRenderPass() const
		{
			return _Pipeline && _Pipeline->IsPipelineCompiled();
		}
	}
}
