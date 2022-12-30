#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		static constexpr bool UseMeshPipeline = false;

		ObjectPass::ObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
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
			_ObjectDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);
			
			_ObjectInputLayout = CreateInputLayout(
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
			_ObjectRenderPass = CreateRenderPass(
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
				ShaderCreateInformation OpaqueMSCreateInformation(ShaderType::MS, "ObjectMS", "object.ms.hlsl", Defines);
				Shader* OpaqueMS = InContext.GetShader(OpaqueMSCreateInformation);
				
				MeshPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_ObjectRenderPass,
					OpaqueMS, OpaquePS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
			else
			{
				ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, "ObjectVS", "object.vs.hlsl", Defines);
				Shader* OpaqueVS = InContext.GetShader(OpaqueVSCreateInformation);

				GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_ObjectInputLayout,
					_ObjectRenderPass,
					OpaqueVS, OpaquePS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
		}

		ObjectPass::~ObjectPass()
		{
			DestroyRenderPass(_ObjectRenderPass);
			DestroyInputLayout(_ObjectInputLayout);
			DestroyDescriptorTable(_ObjectDescriptorTable);
		}

		template<
			typename TransitionFunctor,
			typename PerPassFunctor
		>
		void ObjectPass::_RenderInternal(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunctor InTransitionFunction,
			_In_ PerPassFunctor InPerPassFunction
		)
		{
			ETERNAL_PROFILER(BASIC)();

			const vector<MeshCollection*>& MeshCollections = InSystem.GetRenderFrame().MeshCollections;
			if (MeshCollections.size() == 0)
				return;

			CommandListScope ObjectCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, _GetPassName());

			InTransitionFunction(ObjectCommandList, InRenderer);

			ObjectCommandList->BeginRenderPass(*_ObjectRenderPass);
			ObjectCommandList->SetGraphicsPipeline(*_Pipeline);

			InPerPassFunction(InContext, InRenderer);

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				vector<Mesh*>& Meshes = MeshCollections[CollectionIndex]->Meshes;
				for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
				{
					GPUMesh& CurrentGPUMesh = Meshes[MeshIndex]->GetGPUMesh();
					const Resource* MeshVertexBuffer = CurrentGPUMesh.MeshVertexBuffer;
					if (UseMeshPipeline)
					{
						_ObjectDescriptorTable->SetDescriptor(6, CurrentGPUMesh.MeshVertexStructuredBufferView);
						_ObjectDescriptorTable->SetDescriptor(7, CurrentGPUMesh.MeshIndexStructuredBufferView);
					}
					else
					{
						ObjectCommandList->SetIndexBuffer(*CurrentGPUMesh.MeshIndexBuffer);
						ObjectCommandList->SetVertexBuffers(&MeshVertexBuffer);
					}
					for (uint32_t DrawIndex = 0; DrawIndex < CurrentGPUMesh.PerDrawInformations.size(); ++DrawIndex)
					{
						GPUMesh::PerDrawInformation& DrawInformation = CurrentGPUMesh.PerDrawInformations[DrawIndex];

						DrawInformation.PerDrawMaterial->CommitMaterial(*_ObjectDescriptorTable);

						_ObjectDescriptorTable->SetDescriptor(0, DrawInformation.PerDrawConstantBufferMSVS);
						ObjectCommandList->SetGraphicsDescriptorTable(InContext, *_ObjectDescriptorTable);
						if (UseMeshPipeline)
						{
							ObjectCommandList->DispatchMesh();
						}
						else
						{
							ObjectCommandList->DrawIndexedInstanced(
								DrawInformation.IndicesCount, 1,
								DrawInformation.IndicesOffset,
								DrawInformation.VerticesOffset
							);
						}
					}
				}
			}
			ObjectCommandList->EndRenderPass();
		}

		template void ObjectPass::_RenderInternal<TransitionFunctorType, PerPassFunctorType>(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunctorType InTransitionFunction,
			_In_ PerPassFunctorType InPerPassFunction
		);
	}
}
