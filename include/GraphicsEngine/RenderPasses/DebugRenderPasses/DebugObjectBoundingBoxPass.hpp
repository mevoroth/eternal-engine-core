#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "ShadersReflection/HLSLCubePerInstanceInformation.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		class DebugObjectBoundingBoxPass final : public Pass
		{
		public:
			static const string DebugObjectBoundingBoxPassName;

			DebugObjectBoundingBoxPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override;

		private:

			InputLayout*									_DebugObjectBoundingBoxInputLayout		= nullptr;
			RenderPass*										_DebugObjectBoundingBoxRenderPass		= nullptr;
			DescriptorTable*								_DebugObjectBoundingBoxDescriptorTable	= nullptr;
			StructuredBuffer<CubePerInstanceInformation>	_DebugObjectBoundingBoxPerInstanceBuffer;
		};
	}
}
