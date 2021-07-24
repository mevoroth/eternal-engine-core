#include "GraphicsCommand/UploadMeshGraphicsCommand.hpp"
#include "Mesh/Mesh.hpp"
#include "Resources/Payload.hpp"
#include "Graphics/GraphicsInclude.hpp"

namespace Eternal
{
	namespace GraphicsCommands
	{
		using namespace Eternal::Graphics;

		UploadMeshGraphicsCommand::UploadMeshGraphicsCommand(_In_ MeshPayload& InPayload)
			: _Payload(InPayload)
		{
		}

		void UploadMeshGraphicsCommand::Execute(_In_ GraphicsContext& InContext, _In_ CommandList& InCommandList)
		{
			for (uint32_t MeshIndex = 0, MeshCount = _Payload.LoadedMesh->Meshes.size(); MeshIndex < MeshCount; ++MeshIndex)
			{
				Mesh* CurrentMesh = _Payload.LoadedMesh->Meshes[MeshIndex];

				// Vertex buffer
				uint32_t VertexBufferSize = CurrentMesh->GetVertexStride() * CurrentMesh->GetVerticesCount();

				BufferResourceCreateInformation MeshVerticesResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					VertexBufferCreateInformation(
						VertexBufferSize,
						CurrentMesh->GetVertexStride()
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshVertexBuffer = CreateBuffer(MeshVerticesResourceCreateInformation);
				// Upload
				{
					MapRange MeshVertexBufferRange(VertexBufferSize);
					MapScope<> MeshVertexBufferMapScope(*MeshVertexBuffer, MeshVertexBufferRange);
					memcpy(MeshVertexBufferMapScope.GetDataPointer(), CurrentMesh->GetVerticesData(), VertexBufferSize);
				}
				CurrentMesh->SetMeshVertexBuffer(MeshVertexBuffer);

				// Index buffer
				uint32_t IndexBufferSize = CurrentMesh->GetIndexStride() * CurrentMesh->GetIndicesCount();

				BufferResourceCreateInformation MeshIndicesResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					IndexBufferCreateInformation(
						IndexBufferSize,
						CurrentMesh->GetIndexStride()
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshIndexBuffer = CreateBuffer(MeshIndicesResourceCreateInformation);
				// Upload
				{
					MapRange MeshIndexBufferRange(IndexBufferSize);
					MapScope<> MeshIndexBufferMapScope(*MeshIndexBuffer, MeshIndexBufferRange);
					memcpy(MeshIndexBufferMapScope.GetDataPointer(), CurrentMesh->GetIndicesData(), IndexBufferSize);
				}
				CurrentMesh->SetMeshIndexBuffer(MeshIndexBuffer);

				// Constant buffer
				uint32_t ConstantBufferStride = CurrentMesh->GetConstantBufferStride();
				uint32_t ConstantBufferCount = CurrentMesh->GetConstantBufferCount();
				uint32_t ConstantBufferSize = ConstantBufferStride * ConstantBufferCount;

				BufferResourceCreateInformation MeshConstantBufferResourceCreateInformation(
					InContext.GetDevice(),
					CurrentMesh->GetName(),
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
						ConstantBufferSize,
						CurrentMesh->GetConstantBufferStride()
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);
				Resource* MeshConstantBuffer = CreateBuffer(MeshConstantBufferResourceCreateInformation);
				// Upload
				{
					MapRange MeshConstanBufferRange(ConstantBufferSize);
					MapScope<> MeshConstantBufferMapScope(*MeshConstantBuffer, MeshConstanBufferRange);
					memcpy(MeshConstantBufferMapScope.GetDataPointer(), CurrentMesh->GetConstantBufferData(), ConstantBufferSize);
				}
				CurrentMesh->SetMeshConstantBuffer(MeshConstantBuffer);

				for (uint32_t ConstantBufferViewIndex = 0; ConstantBufferViewIndex < ConstantBufferCount; ++ConstantBufferViewIndex)
				{
					ViewMetaData MetaData;
					MetaData.ConstantBufferView.BufferOffset	= ConstantBufferViewIndex * ConstantBufferStride;
					MetaData.ConstantBufferView.BufferSize		= ConstantBufferStride;
					ConstantBufferViewCreateInformation PerDrawConstantBufferViewCreateInformation(
						InContext,
						MeshConstantBuffer,
						MetaData
					);
					View* PerDrawConstantBufferView = CreateConstantBufferView(PerDrawConstantBufferViewCreateInformation);
					CurrentMesh->SetPerDrawConstantBufferViewVS(ConstantBufferViewIndex, PerDrawConstantBufferView);
				}
			}
		}
	}
}
