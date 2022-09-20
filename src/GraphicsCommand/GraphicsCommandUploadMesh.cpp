#include "GraphicsCommand/GraphicsCommandUploadMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Resources/Payload.hpp"
#include "Graphics/GraphicsInclude.hpp"

namespace Eternal
{
	namespace GraphicsCommands
	{
		using namespace Eternal::Graphics;

		GraphicsCommandUploadMesh::GraphicsCommandUploadMesh(_In_ MeshPayload& InPayload)
			: _Payload(InPayload)
		{
		}

		void GraphicsCommandUploadMesh::Execute(_In_ GraphicsContext& InContext)
		{
			for (uint32_t MeshIndex = 0, MeshCount = static_cast<uint32_t>(_Payload.LoadedMesh->Meshes.size()); MeshIndex < MeshCount; ++MeshIndex)
			{
				Mesh* CurrentMesh = _Payload.LoadedMesh->Meshes[MeshIndex];

				// Vertex buffer
				uint32_t VerticesCount = CurrentMesh->GetVerticesCount();
				uint32_t VertexStride = CurrentMesh->GetVertexStride();
				uint32_t VertexBufferSize = CurrentMesh->GetVertexStride() * VerticesCount;

				BufferResourceCreateInformation MeshVerticesResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					//VertexBufferCreateInformation(
					//	VertexStride,
					//	VerticesCount
					//),
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_VERTEX_BUFFER | BufferResourceUsage::BUFFER_RESOURCE_USAGE_STRUCTURED_BUFFER,
						VertexStride,
						VerticesCount
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshVertexBuffer = CreateBuffer(MeshVerticesResourceCreateInformation);
				ViewMetaData MeshVertexStructuredBufferViewMetaData;
				MeshVertexStructuredBufferViewMetaData.ShaderResourceViewBuffer.FirstElement		= 0;
				MeshVertexStructuredBufferViewMetaData.ShaderResourceViewBuffer.NumElements			= VerticesCount;
				MeshVertexStructuredBufferViewMetaData.ShaderResourceViewBuffer.StructureByteStride	= VertexStride;

				View* MeshVertexStructuredBufferView = CreateShaderResourceView(
					ShaderResourceViewStructuredBufferCreateInformation(
						InContext,
						MeshVertexBuffer,
						MeshVertexStructuredBufferViewMetaData
					)
				);
				// Upload
				{
					uint32_t ActualVertexStride = MeshVertexBuffer->GetBufferStride();
					MapRange MeshVertexBufferRange(VertexBufferSize);
					MapScope<uint8_t> MeshVertexBufferMapScope(*MeshVertexBuffer, MeshVertexBufferRange);

					for (uint32_t VertexIndex = 0; VertexIndex < VerticesCount; ++VertexIndex)
					{
						uint8_t* DestinationOffset = MeshVertexBufferMapScope.GetDataPointer() + ActualVertexStride * VertexIndex;
						const uint8_t* SourceOffset = static_cast<const uint8_t*>(CurrentMesh->GetVerticesData()) + VertexStride * VertexIndex;

						memcpy(DestinationOffset, SourceOffset, VertexStride);
					}
				}
				CurrentMesh->SetMeshVertexStructuredBufferView(MeshVertexStructuredBufferView);
				CurrentMesh->SetMeshVertexBuffer(MeshVertexBuffer);

				// Index buffer
				uint32_t IndicesCount = CurrentMesh->GetIndicesCount();
				uint32_t IndexStride = CurrentMesh->GetIndexStride();
				uint32_t IndexBufferSize = IndexStride * IndicesCount;

				BufferResourceCreateInformation MeshIndicesResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					//IndexBufferCreateInformation(
					//	IndexStride,
					//	IndicesCount
					//),
					BufferCreateInformation(
						CurrentMesh->GetIndexStride() == sizeof(uint16_t) ? Format::FORMAT_R16_UINT : Format::FORMAT_R32_UINT,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_INDEX_BUFFER | BufferResourceUsage::BUFFER_RESOURCE_USAGE_STRUCTURED_BUFFER,
						IndexStride,
						IndicesCount
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshIndexBuffer = CreateBuffer(MeshIndicesResourceCreateInformation);
				ViewMetaData MeshIndexStructuredBufferViewMetaData;
				MeshIndexStructuredBufferViewMetaData.ShaderResourceViewBuffer.FirstElement			= 0;
				MeshIndexStructuredBufferViewMetaData.ShaderResourceViewBuffer.NumElements			= IndicesCount / 3;
				MeshIndexStructuredBufferViewMetaData.ShaderResourceViewBuffer.StructureByteStride	= IndexStride * 3;

				View* MeshIndexStructuredBufferView = CreateShaderResourceView(
					ShaderResourceViewStructuredBufferCreateInformation(
						InContext,
						MeshIndexBuffer,
						MeshIndexStructuredBufferViewMetaData
					)
				);
				// Upload
				{
					MapRange MeshIndexBufferRange(IndexBufferSize);
					MapScope<> MeshIndexBufferMapScope(*MeshIndexBuffer, MeshIndexBufferRange);
					memcpy(MeshIndexBufferMapScope.GetDataPointer(), CurrentMesh->GetIndicesData(), IndexBufferSize);
				}
				CurrentMesh->SetMeshIndexBuffer(MeshIndexBuffer);
				CurrentMesh->SetMeshIndexStructuredBufferView(MeshIndexStructuredBufferView);

				// Constant buffer
				uint32_t ConstantBufferStride = CurrentMesh->GetConstantBufferStride();
				uint32_t ConstantBufferCount = CurrentMesh->GetConstantBufferCount();

				BufferResourceCreateInformation MeshConstantBufferResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
						CurrentMesh->GetConstantBufferStride(),
						ConstantBufferCount
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshConstantBuffer = CreateBuffer(MeshConstantBufferResourceCreateInformation);
				// Upload
				{
					uint32_t ActualConstantBufferStride	= MeshConstantBuffer->GetBufferStride();
					MapRange MeshConstanBufferRange(ActualConstantBufferStride * ConstantBufferCount);
					MapScope<uint8_t> MeshConstantBufferMapScope(*MeshConstantBuffer, MeshConstanBufferRange);

					for (uint32_t ElementIndex = 0; ElementIndex < ConstantBufferCount; ++ElementIndex)
					{
						uint8_t* DestinationOffset	= MeshConstantBufferMapScope.GetDataPointer() + ActualConstantBufferStride * ElementIndex;
						const uint8_t* SourceOffset	= static_cast<const uint8_t*>(CurrentMesh->GetConstantBufferData()) + ConstantBufferStride * ElementIndex;

						memcpy(DestinationOffset, SourceOffset, ConstantBufferStride);
					}
				}
				CurrentMesh->SetMeshConstantBuffer(MeshConstantBuffer);

				for (uint32_t ConstantBufferViewIndex = 0; ConstantBufferViewIndex < ConstantBufferCount; ++ConstantBufferViewIndex)
				{
					ViewMetaData MetaData;
					MetaData.ConstantBufferView.BufferElementOffset	= ConstantBufferViewIndex;
					MetaData.ConstantBufferView.BufferSize			= ConstantBufferStride;
					ConstantBufferViewCreateInformation PerDrawConstantBufferViewCreateInformation(
						InContext,
						MeshConstantBuffer,
						MetaData
					);
					View* PerDrawConstantBufferView = CreateConstantBufferView(PerDrawConstantBufferViewCreateInformation);
					CurrentMesh->SetPerDrawConstantBufferViewMSVS(ConstantBufferViewIndex, PerDrawConstantBufferView);
				}
			}
		}
	}
}
