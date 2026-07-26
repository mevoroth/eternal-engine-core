#include "GraphicsEngine/Utility/LambdaPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		LambdaPass::LambdaPass(_In_ const RenderFunctor& InRenderFunction)
			: Pass()
			, _RenderFunction(InRenderFunction)
		{
		}

		void LambdaPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			_RenderFunction(InContext, InSystem, InRenderer);
		}
		
		void LambdaPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
		}

		void LambdaPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
		}

		bool LambdaPass::_CanRenderPass() const
		{
			return true;
		}
	}
}
