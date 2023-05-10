#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
#include "ShadersReflection/HLSLPerDrawConstants.hpp"
#include "ShadersReflection/HLSLPerInstanceInformation.hpp"
#include "Memory/StackMemory.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::HLSL;

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
						{ Format::FORMAT_RGB323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_POSITION },
						{ Format::FORMAT_RGB323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_NORMAL },
						{ Format::FORMAT_RGB323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_TANGENT },
						{ Format::FORMAT_RGB323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_BINORMAL },
						{ Format::FORMAT_RG3232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_UV }
					})
				}
			);

			_ObjectRenderPass = CreateRenderPass(
				InContext,
				InObjectPassCreateInformation.RenderPassInformation
			);

			_ObjectPerInstanceBuffer = CreateMultiBufferedBuffer(
				InContext,
				BufferResourceCreateInformation(
					InContext.GetDevice(),
					"ObjectPerInstanceBuffer",
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_STRUCTURED_BUFFER,
						sizeof(PerInstanceInformation),
						4096
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				)
			);

			ViewMetaData MetaData;
			MetaData.ShaderResourceViewBuffer.NumElements = 4096;
			MetaData.ShaderResourceViewBuffer.StructureByteStride = sizeof(PerInstanceInformation);
			_ObjectPerInstanceBufferView = CreateMultiBufferedShaderResourceView(
				*_ObjectPerInstanceBuffer,
				ShaderResourceViewStructuredBufferCreateInformation(
					InContext,
					*_ObjectPerInstanceBuffer,
					MetaData
				)
			);

			_ObjectPerDrawInstanceBuffer = CreateMultiBufferedBuffer(
				InContext,
				BufferResourceCreateInformation(
					InContext.GetDevice(),
					"ObjectPerDrawInstanceBuffer",
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
						sizeof(PerDrawInstanceConstants),
						4096
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				)
			);

			char ShaderName[256] = {};

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

			const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = InSystem.GetRenderFrame().MeshCollections;
			if (MeshCollections.size() == 0)
				return;

			CommandListScope ObjectCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, _GetPassName());

			InTransitionFunction(ObjectCommandList, InRenderer);

			ObjectCommandList->BeginRenderPass(*_ObjectRenderPass);
			ObjectCommandList->SetGraphicsPipeline(*_Pipeline);

			InPerPassFunction(InContext, InRenderer);

			MapRange PerInstanceBufferMapRange(sizeof(PerInstanceInformation) * 4096);
			MapScope<PerInstanceInformation> PerInstanceBufferMapScope(*_ObjectPerInstanceBuffer, PerInstanceBufferMapRange);

			MapRange PerDrawInstanceBufferMapRange(sizeof(PerDrawInstanceConstants) * 4096);
			MapScope<PerDrawInstanceConstants> PerDrawInstanceBufferMapScope(*_ObjectPerDrawInstanceBuffer, PerDrawInstanceBufferMapRange);

			StackAllocation<View> ObjectPerDrawInstanceBufferViews(alloca(GetViewSize(InContext) * MeshCollections.size()), GetViewSize(InContext), static_cast<uint32_t>(MeshCollections.size()));

			uint32_t DrawInstanceCount	= 0;

			View* ObjectPerInstanceBufferView = *_ObjectPerInstanceBufferView;
			_ObjectDescriptorTable->SetDescriptor(3, ObjectPerInstanceBufferView);

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				vector<Mesh*>& Meshes = MeshCollections[CollectionIndex].Object->Meshes;
					
				PerDrawInstanceBufferMapScope[CollectionIndex].InstanceStart		= DrawInstanceCount;

				uint32_t InstancesCount = static_cast<uint32_t>(MeshCollections[CollectionIndex].Instances.size());
				for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
					PerInstanceBufferMapScope[DrawInstanceCount + InstanceIndex].InstanceWorldToWorld	= MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetViewToWorld();
				DrawInstanceCount += InstancesCount;

				ViewMetaData ObjectPerDrawInstanceConstantsMetaData;
				ObjectPerDrawInstanceConstantsMetaData.ConstantBufferView.BufferElementOffset	= CollectionIndex;
				ObjectPerDrawInstanceConstantsMetaData.ConstantBufferView.BufferSize			= sizeof(PerDrawInstanceConstants);

				View* ObjectPerDrawInstanceBufferView = CreateConstantBufferView(
					ConstantBufferViewCreateInformation(
						InContext,
						&(*_ObjectPerDrawInstanceBuffer),
						ObjectPerDrawInstanceConstantsMetaData
					),
					ObjectPerDrawInstanceBufferViews.SubAllocate()
				);
				_ObjectDescriptorTable->SetDescriptor(1, ObjectPerDrawInstanceBufferView);

				for (uint32_t MeshIndex = 0; MeshIndex < Meshes.size(); ++MeshIndex)
				{
					GPUMesh& CurrentGPUMesh = Meshes[MeshIndex]->GetGPUMesh();
					const Resource* MeshVertexBuffer = CurrentGPUMesh.MeshVertexBuffer;
					if (UseMeshPipeline)
					{
						_ObjectDescriptorTable->SetDescriptor(7, CurrentGPUMesh.MeshVertexStructuredBufferView);
						_ObjectDescriptorTable->SetDescriptor(8, CurrentGPUMesh.MeshIndexStructuredBufferView);
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
								DrawInformation.IndicesCount,
								InstancesCount,
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
