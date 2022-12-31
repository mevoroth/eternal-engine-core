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

		ObjectPass::~ObjectPass()
		{
			DestroyRenderPass(_ObjectRenderPass);
			DestroyInputLayout(_ObjectInputLayout);
			DestroyDescriptorTable(_ObjectDescriptorTable);
		}

		void ObjectPass::_InitializeObjectPass(_In_ GraphicsContext& InContext, _In_ const ObjectPassCreateInformation& InObjectPassCreateInformation)
		{
			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					InObjectPassCreateInformation.RootSignatureParameters,
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

			_ObjectRenderPass = CreateRenderPass(
				InContext,
				InObjectPassCreateInformation.RenderPassInformation
			);

			char ShaderName[256];

			if (UseMeshPipeline)
			{
				sprintf_s(ShaderName, "%s%s", _GetPassName().c_str(), "MS");

				ShaderCreateInformation OpaqueMSCreateInformation(ShaderType::MS, ShaderName, "object.ms.hlsl", InObjectPassCreateInformation.Defines);
				Shader* OpaqueMS = InContext.GetShader(OpaqueMSCreateInformation);
				
				MeshPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_ObjectRenderPass,
					OpaqueMS, InObjectPassCreateInformation.ObjectPS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
			else
			{
				sprintf_s(ShaderName, "%s%s", _GetPassName().c_str(), "VS");

				ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, ShaderName, "object.vs.hlsl", InObjectPassCreateInformation.Defines);
				Shader* OpaqueVS = InContext.GetShader(OpaqueVSCreateInformation);

				GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
					*_RootSignature,
					_ObjectInputLayout,
					_ObjectRenderPass,
					OpaqueVS, InObjectPassCreateInformation.ObjectPS,
					DepthStencilTestWriteNone
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
		}

		template<
			typename TransitionFunctor,
			typename PerPassFunctor,
			typename PerDrawFunctor
		>
		void ObjectPass::_RenderInternal(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunctor InTransitionFunction,
			_In_ PerPassFunctor InPerPassFunction,
			_In_ PerDrawFunctor InPerDrawFunction
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

						InPerDrawFunction(DrawInformation.PerDrawMaterial);

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

		template void ObjectPass::_RenderInternal<TransitionFunctorType, PerPassFunctorType, PerDrawFunctorType>(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunctorType InTransitionFunction,
			_In_ PerPassFunctorType InPerPassFunction,
			_In_ PerDrawFunctorType InPerDrawFunction
		);
	}
}
