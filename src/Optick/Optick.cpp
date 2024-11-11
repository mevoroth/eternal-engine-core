#include "Optick/Optick.hpp"

#if ETERNAL_PLATFORM_WINDOWS

#define VK_USE_PLATFORM_WIN32_KHR
#ifdef VKAPI_PTR
#undef VKAPI_PTR
#endif
#include "optick.h"
#include <vulkan/vulkan.h>

#include "d3d12/D3D12Device.hpp"
#include "d3d12/D3D12CommandQueue.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanCommandQueue.hpp"

#endif

#include "Graphics/GraphicsInclude.hpp"

namespace Eternal
{
	namespace Optick
	{
		void OptickStart(_In_ GraphicsContext& InContext)
		{
			(void)InContext;
//			switch (InContext.GetDevice().GetDeviceType())
//			{
//#if ETERNAL_ENABLE_D3D12
//			case DeviceType::D3D12:
//			{
//				ID3D12CommandQueue* InCommandQueues[] =
//				{
//					static_cast<D3D12CommandQueue&>(InContext.GetGraphicsQueue()).GetD3D12CommandQueue(),
//					static_cast<D3D12CommandQueue&>(InContext.GetComputeQueue()).GetD3D12CommandQueue(),
//					static_cast<D3D12CommandQueue&>(InContext.GetCopyQueue()).GetD3D12CommandQueue()
//				};
//				ID3D12Device* InD3DDevice = static_cast<D3D12Device&>(InContext.GetDevice()).GetD3D12Device();
//				OPTICK_GPU_INIT_D3D12(InD3DDevice, InCommandQueues, ETERNAL_ARRAYSIZE(InCommandQueues));
//			} break;
//#endif
//#ifdef ETERNAL_ENABLE_VULKAN
//			case DeviceType::VULKAN:
//			{
//				VulkanDevice& InDevice					= static_cast<VulkanDevice&>(InContext.GetDevice());
//				VkDevice InVkDevice						= InDevice.GetVulkanDevice();
//				VkDevice InVkDevices[]					= { InVkDevice, InVkDevice, InVkDevice };
//				VkPhysicalDevice InVkPhysicalDevice		= InDevice.GetPhysicalDevice();
//				VkPhysicalDevice InVkPhysicalDevices[]	= { InVkPhysicalDevice, InVkPhysicalDevice, InVkPhysicalDevice };
//				VkQueue InCommandQueues[] =
//				{
//					static_cast<VulkanCommandQueue&>(InContext.GetGraphicsQueue()).GetVulkanCommandQueue(),
//					static_cast<VulkanCommandQueue&>(InContext.GetComputeQueue()).GetVulkanCommandQueue(),
//					static_cast<VulkanCommandQueue&>(InContext.GetCopyQueue()).GetVulkanCommandQueue()
//				};
//				QueueFamilyIndicesType QueueFamilies;
//				InDevice.GetQueueFamilyIndices(QueueFamilies);
//				
//				::Optick::VulkanFunctions VkFunctions;
//				VkFunctions.vkGetPhysicalDeviceProperties	= &vkGetPhysicalDeviceProperties;
//				VkFunctions.vkCreateQueryPool				= reinterpret_cast<PFN_vkCreateQueryPool_>(&vkCreateQueryPool);
//				VkFunctions.vkCreateCommandPool				= reinterpret_cast<PFN_vkCreateCommandPool_>(&vkCreateCommandPool);
//				VkFunctions.vkAllocateCommandBuffers		= reinterpret_cast<PFN_vkAllocateCommandBuffers_>(&vkAllocateCommandBuffers);
//				VkFunctions.vkCreateFence					= reinterpret_cast<PFN_vkCreateFence_>(&vkCreateFence);
//				VkFunctions.vkCmdResetQueryPool				= reinterpret_cast<PFN_vkCmdResetQueryPool_>(&vkCmdResetQueryPool);
//				VkFunctions.vkQueueSubmit					= reinterpret_cast<PFN_vkQueueSubmit_>(&vkQueueSubmit);
//				VkFunctions.vkWaitForFences					= reinterpret_cast<PFN_vkWaitForFences_>(&vkWaitForFences);
//				VkFunctions.vkResetCommandBuffer			= reinterpret_cast<PFN_vkResetCommandBuffer_>(&vkResetCommandBuffer);
//				VkFunctions.vkCmdWriteTimestamp				= reinterpret_cast<PFN_vkCmdWriteTimestamp_>(&vkCmdWriteTimestamp);
//				VkFunctions.vkGetQueryPoolResults			= reinterpret_cast<PFN_vkGetQueryPoolResults_>(&vkGetQueryPoolResults);
//				VkFunctions.vkBeginCommandBuffer			= reinterpret_cast<PFN_vkBeginCommandBuffer_>(&vkBeginCommandBuffer);
//				VkFunctions.vkEndCommandBuffer				= reinterpret_cast<PFN_vkEndCommandBuffer_>(&vkEndCommandBuffer);
//				VkFunctions.vkResetFences					= reinterpret_cast<PFN_vkResetFences_>(&vkResetFences);
//				VkFunctions.vkDestroyCommandPool			= &vkDestroyCommandPool;
//				VkFunctions.vkDestroyQueryPool				= &vkDestroyQueryPool;
//				VkFunctions.vkDestroyFence					= &vkDestroyFence;
//				VkFunctions.vkFreeCommandBuffers			= &vkFreeCommandBuffers;
//
//
//				OPTICK_GPU_INIT_VULKAN(InVkDevices, InVkPhysicalDevices, InCommandQueues, QueueFamilies.data(), ETERNAL_ARRAYSIZE(InCommandQueues), &VkFunctions);
//			} break;
//#endif
//			default:
//			{
//				ETERNAL_BREAK();
//			}
//			}

			//OPTICK_START_CAPTURE();
		}

		void OptickStop()
		{
			//OPTICK_STOP_CAPTURE();
			OPTICK_SHUTDOWN();
		}
	}
}
