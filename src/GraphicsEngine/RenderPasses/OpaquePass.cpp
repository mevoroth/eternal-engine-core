#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "Core/System.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "GraphicData/RenderTargetTexture.hpp"
#include "GraphicData/SceneResources.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ System& InSystem)
		{
			ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, "OpaqueVS", "opaque.vs.hlsl");
			Shader& OpaqueVS = *InContext.GetShader(OpaqueVSCreateInformation);
			ShaderCreateInformation OpaquePSCreateInformation(ShaderType::PS, "OpaquePS", "opaque.ps.hlsl");
			Shader& OpaquePS = *InContext.GetShader(OpaquePSCreateInformation);

			_OpaqueRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
					}
				)
			);
			
			_OpaqueInputLayout = CreateInputLayout(
				InContext,
				{
					VertexStream<PositionNormalTangentBinormalUVVertex>({
						{ Format::FORMAT_RGBA32323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_POSITION },
						{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_NORMAL },
						{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_TANGENT },
						{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_BINORMAL },
						{ Format::FORMAT_RG3232_FLOAT,			VertexDataType::VERTEX_DATA_TYPE_UV }
					})
				}
			);

			_OpaqueRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateAdditive, RenderTargetOperator::Load_Store, InSystem.GetSceneResources().GetGBufferEmissive().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InSystem.GetSceneResources().GetGBufferAlbedo().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InSystem.GetSceneResources().GetGBufferNormals().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InSystem.GetSceneResources().GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView())
					},
					InSystem.GetSceneResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Load_Store
				)
			);

			PipelineCreateInformation OpaquePipelineCreateInformation(
				*_OpaqueRootSignature,
				*_OpaqueInputLayout,
				*_OpaqueRenderPass,
				OpaqueVS, OpaquePS,
				DepthStencilTestWriteLessNone
			);
			_OpaquePipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
		}

		OpaquePass::~OpaquePass()
		{

		}

		void OpaquePass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem)
		{
			CommandList* OpaqueCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "OpaquePass");

			vector<Mesh*>& Meshes = InSystem.GetRenderFrame().Meshes;
			for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
			{

			}
		}
	}
}
