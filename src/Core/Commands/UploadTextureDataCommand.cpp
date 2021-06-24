#include "Task/Core/Commands/UploadTextureDataCommand.hpp"
#include "Graphics/Types/DeviceType.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Resource.hpp"
#include "Container/FreeList.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/UploadBuffers.hpp"

using namespace Eternal::Core;
using namespace Eternal::GraphicData;

void UploadTextureDataCommand::Execute(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ CommandList& CommandListObj)
{
//	switch (DeviceObj.GetDeviceType())
//	{
//#ifdef ETERNAL_ENABLE_D3D12
//	case DeviceType::D3D12:
//	{
//		Resource* UploadBuffer = Resources->GetUploadBuffers()->Get(UPLOAD_BUFFER_TEXTURE)->Pop();
//		void* UploadBufferData = UploadBuffer->Map(DeviceObj);
//		memcpy(UploadBufferData, TextureData, sizeof(uint8_t) * 4 * Width * Height);
//		UploadBuffer->Unmap(DeviceObj);
//
//		ResourceTransition TextureTransitionBefore(DestinationTexture, TRANSITION_UNDEFINED, TRANSITION_COPY_WRITE);
//		CommandListObj.Transition(nullptr, 0u, &TextureTransitionBefore, 1u);
//
//		Position3D Pos = { 0, 0, 0 };
//		Extent Ext = { Width, Height, 1 };
//		CommandListObj.CopyBufferToTexture(*UploadBuffer, *DestinationTexture, 0, sizeof(uint32_t) * Width * Height, Pos, Ext);
//
//		ResourceTransition TextureTransitionAfter(DestinationTexture, TRANSITION_COPY_WRITE, TRANSITION_SHADER_READ);
//		CommandListObj.Transition(nullptr, 0u, &TextureTransitionAfter, 1u);
//	} break;
//#endif
//	case DeviceType::VULKAN:
//	{
//		Resource* UploadBuffer = Resources->GetUploadBuffers()->Get(UPLOAD_BUFFER_TEXTURE)->Pop();
//		void* UploadBufferData = UploadBuffer->Map(DeviceObj);
//		memcpy(UploadBufferData, TextureData, sizeof(uint8_t) * 4 * Width * Height);
//		UploadBuffer->Unmap(DeviceObj);
//
//		ResourceTransition TextureTransitionsBefore[] =
//		{
//			ResourceTransition(DestinationTexture,	TRANSITION_UNDEFINED, TRANSITION_COPY_WRITE),
//			ResourceTransition(UploadBuffer,		TRANSITION_UNDEFINED, TRANSITION_COPY_READ)
//		};
//		//ResourceTransition TextureTransitionBefore(DestinationTexture, TRANSITION_UNDEFINED, TRANSITION_COPY_WRITE);
//		CommandListObj.Transition(nullptr, 0u, TextureTransitionsBefore, ETERNAL_ARRAYSIZE(TextureTransitionsBefore));
//
//		Position3D Pos = { 0, 0, 0 };
//		Extent Ext = { Width, Height, 1 };
//		//CommandListObj.CopyBufferToTexture(*UploadBuffer, *DestinationTexture, 0, sizeof(uint32_t) * Width * Height, Pos, Ext);
//		CommandListObj.CopyTexture(*UploadBuffer, *DestinationTexture, Pos, Pos, Ext);
//
//		ResourceTransition TextureTransitionAfter(DestinationTexture, TRANSITION_COPY_WRITE, TRANSITION_SHADER_READ);
//		CommandListObj.Transition(nullptr, 0u, &TextureTransitionAfter, 1u);
//	} break;
//
//	default:
//		ETERNAL_BREAK();
//		break;
//	}
}
