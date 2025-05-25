#pragma once

#include "GraphicsEngine/Pass.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "ShadersReflection/HLSLPerDrawConstants.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace std;
		using namespace Eternal::HLSL;

		class DebugPass final : public Pass
		{
		public:

			static const string DebugPassName;

			DebugPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer);

			virtual void Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer) override final;

			virtual void GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const override final;
			virtual void GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const override final;

		private:

			DescriptorTable*						_DebugDescriptorTable	= nullptr;
			InputLayout*							_DebugInputLayout		= nullptr;
			RenderPass*								_DebugRenderPass		= nullptr;
			FixedConstantBuffer<PerDrawConstants>	_DebugPerDrawConstantBuffer;
			VertexBuffer<PositionVertex>			_DebugVertexBuffer;

		};
	}
}
