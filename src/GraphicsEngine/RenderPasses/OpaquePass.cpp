#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, "OpaqueVS", "opaque.vs.hlsl");
			Shader* OpaqueVS = InContext.GetShader(OpaqueVSCreateInformation);
			ShaderCreateInformation OpaquePSCreateInformation(ShaderType::PS, "OpaquePS", "opaque.ps.hlsl");
			Shader* OpaquePS = InContext.GetShader(OpaquePSCreateInformation);

			_OpaqueRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
					},
					{}, {},
					/*InHasInputAssembler=*/ true
				)
			);
			_OpaqueDescriptorTable = _OpaqueRootSignature->CreateRootDescriptorTable(InContext);
			
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

			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();
			_OpaqueRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateAdditive, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferAlbedo().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferNormals().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Clear_Store
				)
			);

			GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
				*_OpaqueRootSignature,
				_OpaqueInputLayout,
				_OpaqueRenderPass,
				OpaqueVS, OpaquePS,
				DepthStencilTestWriteGreaterNone
			);
			_OpaquePipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
		}

		OpaquePass::~OpaquePass()
		{
			DestroyPipeline(_OpaquePipeline);
			DestroyRenderPass(_OpaqueRenderPass);
			DestroyInputLayout(_OpaqueInputLayout);
			DestroyDescriptorTable(_OpaqueDescriptorTable);
			DestroyRootSignature(_OpaqueRootSignature);
		}

		void OpaquePass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			ETERNAL_PROFILER(BASIC)();

			const vector<MeshCollection*>& MeshCollections = InSystem.GetRenderFrame().MeshCollections;
			if (MeshCollections.size() == 0)
				return;

			CommandList* OpaqueCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "OpaquePass");

			OpaqueCommandList->Begin(InContext);
			OpaqueCommandList->BeginRenderPass(*_OpaqueRenderPass);
			OpaqueCommandList->SetGraphicsPipeline(*_OpaquePipeline);

			_OpaqueDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_OpaqueDescriptorTable->SetDescriptor(5, InContext.GetBilinearClampSampler());

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				vector<Mesh*>& Meshes = MeshCollections[CollectionIndex]->Meshes;
				for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
				{
					GPUMesh& CurrentGPUMesh = Meshes[MeshIndex]->GetGPUMesh();
					const Resource* MeshVertexBuffer = CurrentGPUMesh.MeshVertexBuffer;
					OpaqueCommandList->SetIndexBuffer(*CurrentGPUMesh.MeshIndexBuffer);
					OpaqueCommandList->SetVertexBuffers(&MeshVertexBuffer);
					for (uint32_t DrawIndex = 0; DrawIndex < CurrentGPUMesh.PerDrawInformations.size(); ++DrawIndex)
					{
						GPUMesh::PerDrawInformation& DrawInformation = CurrentGPUMesh.PerDrawInformations[DrawIndex];

						DrawInformation.PerDrawMaterial->CommitMaterial(*_OpaqueDescriptorTable);

						_OpaqueDescriptorTable->SetDescriptor(0, DrawInformation.PerDrawConstantBufferVS);
						OpaqueCommandList->SetGraphicsDescriptorTable(InContext, *_OpaqueDescriptorTable);
						OpaqueCommandList->DrawIndexedInstanced(
							DrawInformation.IndicesCount, 1,
							DrawInformation.IndicesOffset,
							DrawInformation.VerticesOffset
						);
					}
				}
			}
			
			OpaqueCommandList->EndRenderPass();
			OpaqueCommandList->End();
		}

	}
}
