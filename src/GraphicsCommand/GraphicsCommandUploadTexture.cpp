#include "GraphicsCommand/GraphicsCommandUploadTexture.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "Resources/Payload.hpp"
#include "Resources/TextureFactory.hpp"
#include "GraphicData/Texture.hpp"
#include "Material/Material.hpp"

namespace Eternal
{
	namespace GraphicsCommands
	{
		GraphicsCommandUploadTexture::GraphicsCommandUploadTexture(_In_ TexturePayload& InPayload, _In_ TextureFactory& InTextureFactory)
			: _Payload(InPayload)
			, _Factory(InTextureFactory)
		{
		}

		void GraphicsCommandUploadTexture::Execute(_In_ GraphicsContext& InContext)
		{
			const RawTextureData& InTextureData = _Payload.TextureData;
			TextureCache& Cache = _Factory.GetTextureCache(_Payload.MaterialToUpdate.Key);

			if (!Cache.CachedTexture)
			{
				CommandListScope UploadTextureCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "GraphicsCommandUploadTexture");

				//////////////////////////////////////////////////////////////////////////
				// CPU Buffer
				std::string UploadBufferName = "AnonymousTextureBuffer";
				const uint32_t UploadBufferSize = InTextureData.Width * InTextureData.Height * InTextureData.DepthOrArraySize;
				const uint32_t UploadBufferSizeBytes = UploadBufferSize * InTextureData.Stride;
				//TextureResourceCreateInformation UploadBufferTextureInformation(
				BufferResourceCreateInformation UploadBufferTextureInformation(
					InContext.GetDevice(),
					UploadBufferName,
					BufferCreateInformation(
						Format::FORMAT_BGRA8888_UNORM,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_COPY_READ,
						InTextureData.Stride,
						UploadBufferSize
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				);

				Resource* UploadTexture = CreateBuffer(UploadBufferTextureInformation);

				//////////////////////////////////////////////////////////////////////////
				// Map
				MapRange UploadTextureMapRange(UploadBufferSizeBytes);
				void* UploadTextureDataPtr = UploadTexture->Map<>(UploadTextureMapRange);
				memcpy(UploadTextureDataPtr, InTextureData.TextureData, UploadBufferSizeBytes);
				UploadTexture->Unmap(UploadTextureMapRange);
				InContext.DelayedDelete(UploadTexture);

				//////////////////////////////////////////////////////////////////////////
				// GPU Texture
				TextureResourceCreateInformation GPUTextureCreateInformation(
					InContext.GetDevice(),
					_Payload.MaterialToUpdate.Key,
					TextureCreateInformation(
						ResourceDimension::RESOURCE_DIMENSION_TEXTURE_2D,
						Format::FORMAT_BGRA8888_UNORM,
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
				// Upload
				UploadTextureCommandList->CopyResource(
					Cache.CachedTexture->GetTexture(),
					*UploadTexture,
					CopyRegion(
						TextureFromBufferRegion(
							Extent3D(InTextureData.Width, InTextureData.Height, InTextureData.DepthOrArraySize),
							UploadBufferSizeBytes
						)
					)
				);

				//////////////////////////////////////////////////////////////////////////
				// Transition
				ResourceTransition TextureCopyWriteToShaderResource(
					Cache.CachedTexture->GetShaderResourceView(),
					TransitionState::TRANSITION_PIXEL_SHADER_READ
				);
				UploadTextureCommandList->Transition(
					&TextureCopyWriteToShaderResource, 1
				);
			}

			if (_Payload.MaterialToUpdate.MaterialToUpdate)
				_Payload.MaterialToUpdate.MaterialToUpdate->SetTexture(_Payload.MaterialToUpdate.Slot, Cache.CachedTexture);
		}
	}
}
