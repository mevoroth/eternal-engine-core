#include "GraphicsEngine/RenderPasses/DebugRenderPasses/DebugPass.hpp"
#include "Core/System.hpp"
#include "Core/DebugDraw/DebugDrawPrimitives.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		const string DebugPass::DebugPassName = "DebugPass";

		DebugPass::DebugPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _DebugPerDrawConstantBuffer(InContext, "DebugPerDrawConstantBuffer")
			, _DebugVertexBuffer(InContext, "DebugVertexBuffer")
		{
			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			Shader* DebugVertex	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_VERTEX, "DebugVertex", "object.vertex.hlsl"));
			Shader* DebugPixel	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "DebugColorPixel", "DebugShaders/debugcolor.pixel.hlsl"));

			_RootSignature			= CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX)
					},
					/*InHasInputAssembler=*/ true
				)
			);

			_DebugDescriptorTable	= _RootSignature->CreateRootDescriptorTable(InContext);

			_DebugInputLayout		= CreateInputLayout(
				InContext,
				{
					VertexStream<PositionVertex>({
						{ Format::FORMAT_RGB323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_POSITION }
					})
				}
			);

			_DebugRenderPass		= CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Load_Store
				)
			);

			_Pipeline				= CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					_RootSignature,
					_DebugInputLayout,
					_DebugRenderPass,
					DebugVertex, DebugPixel,
					DepthStencilTestWriteNone,
					RasterizerLine,
					PrimitiveTopology::PRIMITIVE_TOPOLOGY_LINE_LIST
				)
			);

			{
				MapScope<PerDrawConstants> PerDrawBufferMapScope(_DebugPerDrawConstantBuffer);
				PerDrawBufferMapScope->SubMeshToWorldTransposed = Matrix4x4::Identity;
			}
		}

		void DebugPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			using namespace Eternal::Core;

			const Debug::DebugDrawPrimitives& DebugPrimitives = InSystem.GetRenderFrame().DebugPrimitives;

			if (DebugPrimitives.Lines.size() <= 0)
				return;

			{
				MapScope<PositionVertex> VertexBufferMapScope(_DebugVertexBuffer);
				memcpy(VertexBufferMapScope.GetDataPointer(), DebugPrimitives.Lines.data(), sizeof(PositionVertex) * DebugPrimitives.Lines.size());
			}

			CommandListScope DebugCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "DebugPass");
			{
				_DebugDescriptorTable->SetDescriptor(0, static_cast<View*>(_DebugPerDrawConstantBuffer));
				_DebugDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());

				const Resource* CurrentVertexBUffer = _DebugVertexBuffer;

				DebugCommandList->BeginRenderPass(*_DebugRenderPass);
				DebugCommandList->SetVertexBuffers(&CurrentVertexBUffer);
				DebugCommandList->SetGraphicsPipeline(*_Pipeline);
				DebugCommandList->SetGraphicsDescriptorTable(InContext, *_DebugDescriptorTable);
				DebugCommandList->DrawInstanced(static_cast<uint32_t>(DebugPrimitives.Lines.size()), 1);
				DebugCommandList->EndRenderPass();
			}
		}

		void DebugPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
		}

		void DebugPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
		}
	}
}
