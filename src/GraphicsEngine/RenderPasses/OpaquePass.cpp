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

		static constexpr bool UseMeshPipeline = true;

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			vector<string> Defines =
			{
				"OBJECT_NEEDS_NORMAL",		"1",
				"OBJECT_NEEDS_TANGENT",		"1",
				"OBJECT_NEEDS_BINORMAL",	"1",
				"OBJECT_NEEDS_UV",			"1"
			};

			ShaderCreateInformation OpaquePSCreateInformation(ShaderType::PS, "OpaquePS", "opaque.ps.hlsl", Defines);
			Shader* OpaquePS = InContext.GetShader(OpaquePSCreateInformation);

			vector<RootSignatureParameter> ParametersVSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
			};

			vector<RootSignatureParameter> ParametersMSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS)
			};

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
					{}, {},
					/*InHasInputAssembler=*/ true
				)
			);
			_OpaqueDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);
			
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

			if (UseMeshPipeline)
			{
				ShaderCreateInformation OpaqueMSCreateInformation(ShaderType::MS, "OpaqueMS", "object.ms.hlsl", Defines);
				Shader* OpaqueMS = InContext.GetShader(OpaqueMSCreateInformation);
				
				MeshPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_OpaqueRenderPass,
					OpaqueMS, OpaquePS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
			else
			{
				ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, "OpaqueVS", "object.vs.hlsl", Defines);
				Shader* OpaqueVS = InContext.GetShader(OpaqueVSCreateInformation);

				GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_OpaqueInputLayout,
					_OpaqueRenderPass,
					OpaqueVS, OpaquePS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
		}

		OpaquePass::~OpaquePass()
		{
			DestroyRenderPass(_OpaqueRenderPass);
			DestroyInputLayout(_OpaqueInputLayout);
			DestroyDescriptorTable(_OpaqueDescriptorTable);
		}

		void OpaquePass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			ETERNAL_PROFILER(BASIC)();

			const vector<MeshCollection*>& MeshCollections = InSystem.GetRenderFrame().MeshCollections;
			if (MeshCollections.size() == 0)
				return;

			GraphicsCommandListScope OpaqueCommandList = InContext.CreateNewGraphicsCommandList(*_OpaqueRenderPass, "OpaquePass");

			OpaqueCommandList->SetGraphicsPipeline(*_Pipeline);

			_OpaqueDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_OpaqueDescriptorTable->SetDescriptor(5, InContext.GetBilinearClampSampler());

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				vector<Mesh*>& Meshes = MeshCollections[CollectionIndex]->Meshes;
				for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
				{
					GPUMesh& CurrentGPUMesh = Meshes[MeshIndex]->GetGPUMesh();
					const Resource* MeshVertexBuffer = CurrentGPUMesh.MeshVertexBuffer;
					if (UseMeshPipeline)
					{
						_OpaqueDescriptorTable->SetDescriptor(6, CurrentGPUMesh.MeshVertexStructuredBufferView);
						_OpaqueDescriptorTable->SetDescriptor(7, CurrentGPUMesh.MeshIndexStructuredBufferView);
					}
					else
					{
						OpaqueCommandList->SetIndexBuffer(*CurrentGPUMesh.MeshIndexBuffer);
						OpaqueCommandList->SetVertexBuffers(&MeshVertexBuffer);
					}
					for (uint32_t DrawIndex = 0; DrawIndex < CurrentGPUMesh.PerDrawInformations.size(); ++DrawIndex)
					{
						GPUMesh::PerDrawInformation& DrawInformation = CurrentGPUMesh.PerDrawInformations[DrawIndex];

						DrawInformation.PerDrawMaterial->CommitMaterial(*_OpaqueDescriptorTable);

						_OpaqueDescriptorTable->SetDescriptor(0, DrawInformation.PerDrawConstantBufferMSVS);
						OpaqueCommandList->SetGraphicsDescriptorTable(InContext, *_OpaqueDescriptorTable);
						if (UseMeshPipeline)
						{

						}
						else
						{
							OpaqueCommandList->DrawIndexedInstanced(
								DrawInformation.IndicesCount, 1,
								DrawInformation.IndicesOffset,
								DrawInformation.VerticesOffset
							);
						}
					}
				}
			}
		}

	}
}
