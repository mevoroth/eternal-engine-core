#include "GraphicsCommand/GraphicsCommandUploadTexture.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "Core/System.hpp"
#include "Resources/Payload.hpp"
#include "Resources/TextureFactory.hpp"
#include "Material/Material.hpp"

namespace Eternal
{
	namespace GraphicsCommands
	{
		GraphicsCommandUploadTexture::GraphicsCommandUploadTexture(_In_ TexturePayload& InPayload, _In_ TextureFactory& InTextureFactory, _In_ System& InSystem)
			: _Payload(InPayload)
			, _Factory(InTextureFactory)
			, _System(InSystem)
		{
		}

		void GraphicsCommandUploadTexture::Execute(_In_ GraphicsContext& InContext)
		{
			TextureCache& Cache							= _Factory.GetTextureCache(_Payload.Key);
			MaterialUpdateBatch& CurrentMaterialBatch	= _System.GetMaterialUpdateBatcher().MaterialUpdates.find(_Payload.Key)->second;
			const RawTextureData& InTextureData			= CurrentMaterialBatch.TextureData;

			if (!Cache.CachedTexture)
			{
				ETERNAL_ASSERT(InTextureData.TextureData);

				CommandListScope UploadTextureCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "GraphicsCommandUploadTexture");

				//////////////////////////////////////////////////////////////////////////
				// GPU Texture
				TextureResourceCreateInformation GPUTextureCreateInformation(
					InContext.GetDevice(),
					_Payload.Key,
					TextureCreateInformation(
						InTextureData.Dimension,
						InTextureData.TextureFormat,
						TextureResourceUsage::TEXTURE_RESOURCE_USAGE_SHADER_RESOURCE | TextureResourceUsage::TEXTURE_RESOURCE_USAGE_COPY_WRITE,
						InTextureData.Width,
						InTextureData.Height,
						InTextureData.DepthOrArraySize,
						1
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_MEMORY,
					TransitionState::TRANSITION_COPY_WRITE
				);
				ETERNAL_ASSERT(!Cache.CachedTexture);
				Cache.CachedTexture = new Texture(InContext, GPUTextureCreateInformation);
				
				//////////////////////////////////////////////////////////////////////////
				// CPU Buffer
				std::string UploadBufferName = "AnonymousTextureBuffer";
				//const uint32_t UploadBufferSize = InTextureData.Width * InTextureData.Height * InTextureData.DepthOrArraySize;
				//const uint32_t UploadBufferSizeBytes = UploadBufferSize * InTextureData.Stride;
				TextureToBufferMemoryFootprint Footprint	= Cache.CachedTexture->GetTexture().GetTextureToBufferMemoryFootprint(InContext.GetDevice());
				const uint32_t UploadBufferAlignedSizeBytes	= Footprint.TotalBytes;
				const uint32_t UploadBufferAlignedSize		= UploadBufferAlignedSizeBytes / InTextureData.Stride;
				
				Resource* UploadTexture = CreateBuffer(
					BufferResourceCreateInformation(
						InContext.GetDevice(),
						UploadBufferName,
						BufferCreateInformation(
							InTextureData.TextureFormat,
							BufferResourceUsage::BUFFER_RESOURCE_USAGE_COPY_READ,
							InTextureData.Stride,
							UploadBufferAlignedSize
						),
						ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
					)
				);

				//////////////////////////////////////////////////////////////////////////
				// Map
				MapRange UploadTextureMapRange(UploadBufferAlignedSizeBytes);
				uint8_t* UploadTextureDataPtr = UploadTexture->Map<uint8_t>(UploadTextureMapRange);
				uint8_t* SourceTextureDataPtr = reinterpret_cast<uint8_t*>(InTextureData.TextureData);
				for (int Row = 0, RowCount = InTextureData.Height * InTextureData.DepthOrArraySize; Row < RowCount; ++Row)
				{
					memcpy(UploadTextureDataPtr, SourceTextureDataPtr, InTextureData.Width * InTextureData.Stride);
					UploadTextureDataPtr += Footprint.RowPitch;
					SourceTextureDataPtr += InTextureData.Width * InTextureData.Stride;
				}
				UploadTexture->Unmap(UploadTextureMapRange);
				InContext.DelayedDelete(UploadTexture);

				//////////////////////////////////////////////////////////////////////////
				// Transition
				ResourceTransition TextureToCopyWrite(
					Cache.CachedTexture->GetShaderResourceView(),
					TransitionState::TRANSITION_COPY_WRITE
				);
				UploadTextureCommandList->Transition(
					&TextureToCopyWrite, 1
				);

				//////////////////////////////////////////////////////////////////////////
				// Upload
				UploadTextureCommandList->CopyResource(
					Cache.CachedTexture->GetTexture(),
					*UploadTexture,
					CopyRegion(
						TextureFromBufferRegion(
							Extent3D(InTextureData.Width, InTextureData.Height, InTextureData.DepthOrArraySize),
							UploadBufferAlignedSizeBytes
						)
					)
				);

				//////////////////////////////////////////////////////////////////////////
				// Transition
				ResourceTransition TextureCopyWriteToShaderResource(
					Cache.CachedTexture->GetShaderResourceView(),
					TransitionState::TRANSITION_PIXEL_SHADER_READ | TransitionState::TRANSITION_NON_PIXEL_SHADER_READ
				);
				UploadTextureCommandList->Transition(
					&TextureCopyWriteToShaderResource, 1
				);
			}

			for (uint32_t MaterialIndex = 0; MaterialIndex < CurrentMaterialBatch.Materials.size(); ++MaterialIndex)
			{
				if (CurrentMaterialBatch.Materials[MaterialIndex].MaterialToUpdate)
					CurrentMaterialBatch.Materials[MaterialIndex].MaterialToUpdate->SetTexture(CurrentMaterialBatch.Materials[MaterialIndex].Slot, Cache.CachedTexture);
			}
			CurrentMaterialBatch.Materials.clear();
			CurrentMaterialBatch.TextureData.ReleaseTextureData();

			_Payload.MarkProcessed();
		}
	}
}
