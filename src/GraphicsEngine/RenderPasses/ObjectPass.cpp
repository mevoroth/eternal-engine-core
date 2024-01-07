#include "GraphicsEngine/RenderPasses/ObjectPass.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
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

		ObjectPass::ObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ uint32_t InInstanceCount)
			: _ObjectPerInstanceBuffer(InContext, "ObjectPerInstanceBuffer", InInstanceCount)
		{
			(void)InRenderer;
		}

		void ObjectPass::_InitializeObjectPass(_In_ GraphicsContext& InContext, _In_ const ObjectPassCreateInformation& InObjectPassCreateInformation)
		{
			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					InObjectPassCreateInformation.RootSignatureParameters,
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
				sprintf_s(ShaderName, "%s%s", _GetPassName().c_str(), "Mesh");

				Shader* OpaqueMesh = InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_MESH, ShaderName, "object.mesh.hlsl", InObjectPassCreateInformation.Defines));
				
				MeshPipelineCreateInformation OpaquePipelineCreateInformation(
					_RootSignature,
					_ObjectRenderPass,
					OpaqueMesh, InObjectPassCreateInformation.ObjectPixel,
					InObjectPassCreateInformation.ObjectDepthStencil,
					InObjectPassCreateInformation.ObjectRasterizer
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
			else
			{
				sprintf_s(ShaderName, "%s%s", _GetPassName().c_str(), "Vertex");

				Shader* OpaqueVertex = InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_VERTEX, ShaderName, "object.vertex.hlsl", InObjectPassCreateInformation.Defines));

				GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
					_RootSignature,
					_ObjectInputLayout,
					_ObjectRenderPass,
					OpaqueVertex, InObjectPassCreateInformation.ObjectPixel,
					InObjectPassCreateInformation.ObjectDepthStencil,
					InObjectPassCreateInformation.ObjectRasterizer
				);
				_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			}
		}

		template<
			typename TransitionFunction,
			typename PerPassFunction,
			typename PerDrawFunction,
			typename IsVisibleFunction
		>
		void ObjectPass::_RenderInternal(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunction InTransitionFunctor,
			_In_ PerPassFunction InPerPassFunctor,
			_In_ PerDrawFunction InPerDrawFunctor,
			_In_ IsVisibleFunction InIsVisibleFunctor
		)
		{
			ETERNAL_PROFILER(BASIC)();

			const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = InSystem.GetRenderFrame().MeshCollections;
			
			if (MeshCollections.size() == 0 || !Material::DefaultMaterial->IsValid(TextureType::TEXTURE_TYPE_DIFFUSE))
				return;

			CommandListScope ObjectCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, _GetPassName());

			InTransitionFunctor(ObjectCommandList, InRenderer);

			ObjectCommandList->BeginRenderPass(*_ObjectRenderPass);
			ObjectCommandList->SetGraphicsPipeline(*_Pipeline);

			InPerPassFunctor(InContext, InRenderer);

			MapRange PerInstanceBufferMapRange(sizeof(PerInstanceInformation) * (*_ObjectPerInstanceBuffer.ResourceBuffer)->GetElementCount());
			MapScope<PerInstanceInformation> PerInstanceBufferMapScope(*_ObjectPerInstanceBuffer.ResourceBuffer, PerInstanceBufferMapRange);

			MapRange PerDrawInstanceBufferMapRange(sizeof(PerDrawInstanceConstants) * (*_ObjectPerDrawInstanceBuffer)->GetElementCount());
			MapScope<PerDrawInstanceConstants> PerDrawInstanceBufferMapScope(*_ObjectPerDrawInstanceBuffer, PerDrawInstanceBufferMapRange);

			uint32_t PerDrawCount = 0;
			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
				PerDrawCount += static_cast<uint32_t>(MeshCollections[CollectionIndex].Instances.size());

			StackAllocation<View> ObjectPerDrawInstanceBufferViews(alloca(GetViewSize(InContext) * PerDrawCount), GetViewSize(InContext), PerDrawCount);

			uint32_t DrawInstanceCount	= 0;

			_ObjectDescriptorTable->SetDescriptor(3, _ObjectPerInstanceBuffer.GetView());

			uint32_t VisibilityKey = 0;

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				Mesh*& Meshes = MeshCollections[CollectionIndex].Object->Meshes;
				
				PerDrawInstanceBufferMapScope[CollectionIndex].InstanceStart		= DrawInstanceCount;

				uint32_t InstancesCount = static_cast<uint32_t>(MeshCollections[CollectionIndex].Instances.size());
				//for (uint32_t InstanceIndex = 0; InstanceIndex < MeshCollections[CollectionIndex].Instances.size(); ++InstanceIndex)
				//	PerInstanceBufferMapScope[DrawInstanceCount + InstanceIndex].InstanceWorldToWorld	= MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();
				//DrawInstanceCount += InstancesCount;

				for (uint32_t InstanceIndex = 0; InstanceIndex < InstancesCount; ++InstanceIndex)
				{
					PerInstanceBufferMapScope[DrawInstanceCount + InstanceIndex].InstanceWorldToWorld = MeshCollections[CollectionIndex].Instances[InstanceIndex]->GetTransform().GetLocalToWorld();

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

					GPUMesh& CurrentGPUMesh = Meshes->GetGPUMesh();
					if (!CurrentGPUMesh.MeshIndexBuffer)
						continue;

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

						if (!InIsVisibleFunctor(VisibilityKey++))
							continue;

						InPerDrawFunctor(DrawInformation.PerDrawMaterial, InRenderer);

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
								1,
								DrawInformation.IndicesOffset,
								DrawInformation.VerticesOffset
							);
						}
					}
				}
				DrawInstanceCount += InstancesCount;
			}
			ObjectCommandList->EndRenderPass();
		}

		template void ObjectPass::_RenderInternal<TransitionFunctionType, PerPassFunctionType, PerDrawFunctionType, IsVisibleFunctionType>(
			_In_ GraphicsContext& InContext,
			_In_ System& InSystem,
			_In_ Renderer& InRenderer,
			_In_ TransitionFunctionType InTransitionFunctor,
			_In_ PerPassFunctionType InPerPassFunctor,
			_In_ PerDrawFunctionType InPerDrawFunctor,
			_In_ IsVisibleFunctionType InIsVisibleFunctor
		);
	}
}
