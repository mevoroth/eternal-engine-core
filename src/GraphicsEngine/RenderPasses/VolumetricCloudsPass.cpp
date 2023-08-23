#include "GraphicsEngine/RenderPasses/VolumetricCloudsPass.hpp"
#include "imgui.h"
#include "Core/System.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::HLSL;

		static constexpr int32_t ThreadGroupCountX = 32;
		static constexpr int32_t ThreadGroupCountY = 32;
		static constexpr int32_t ThreadGroupCountZ = 1;

		static float VolumetricCloudsBottomLayerMeters	= 300.0f;
		static float VolumetricCloudsTopLayerMeters		= 14000.0f;
		static int VolumetricCloudsMaxSteps				= 64;
		static float VolumetricCloudsDensity			= 0.1f;
		static float VolumetricCloudsNoiseScale			= 1.0f;

		VolumetricCloudsPass::VolumetricCloudsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _VolumetricCloudsConstantBuffer(InContext, "VolumetricCloudsBuffer")
		{
			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, "%d", ThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, "%d", ThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, "%d", ThreadGroupCountZ);

			ShaderCreateInformation VolumetricCloudsComputeCreateInformation(
				ShaderType::SHADER_TYPE_COMPUTE, "VolumetricClouds", "Volumetrics\\volumetricclouds.compute.hlsl",
				{
					"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
					"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
					"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString
				}
			);
			Shader* VolumetricCloudsCompute = InContext.GetShader(VolumetricCloudsComputeCreateInformation);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE)
					}
				)
			);

			_VolumetricCloudsDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					*_RootSignature,
					VolumetricCloudsCompute
				)
			);
		}

		VolumetricCloudsPass::~VolumetricCloudsPass()
		{
			DestroyDescriptorTable(_VolumetricCloudsDescriptorTable);
		}

		void VolumetricCloudsPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope VolumetricCloudsCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "VolumetricClouds");

			{
				MapRange VolumetricCloudsMapRange(sizeof(VolumetricCloudsConstants));
				MapScope<VolumetricCloudsConstants> VolumetricCloudsConstantsMapScope(*_VolumetricCloudsConstantBuffer.ResourceBuffer, VolumetricCloudsMapRange);
				VolumetricCloudsConstantsMapScope->CloudsBottomLayerRadiusMeters		= VolumetricCloudsBottomLayerMeters;
				VolumetricCloudsConstantsMapScope->CloudsTopLayerRadiusMeters			= VolumetricCloudsTopLayerMeters;
				VolumetricCloudsConstantsMapScope->CloudsBottomLayerRadiusMetersSquared	= VolumetricCloudsBottomLayerMeters * VolumetricCloudsBottomLayerMeters;
				VolumetricCloudsConstantsMapScope->CloudsTopLayerRadiusMetersSquared	= VolumetricCloudsTopLayerMeters * VolumetricCloudsTopLayerMeters;
				VolumetricCloudsConstantsMapScope->CloudsMaxStepsRcp					= 1.0f / static_cast<float>(VolumetricCloudsMaxSteps);
				VolumetricCloudsConstantsMapScope->CloudsMaxSteps						= VolumetricCloudsMaxSteps;

				VolumetricCloudsConstantsMapScope->CloudsDensity						= VolumetricCloudsDensity;
				VolumetricCloudsConstantsMapScope->CloudsNoiseScale						= VolumetricCloudsNoiseScale;
			}

			_VolumetricCloudsDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_VolumetricCloudsDescriptorTable->SetDescriptor(1, _VolumetricCloudsConstantBuffer.GetView());
			_VolumetricCloudsDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_VolumetricCloudsDescriptorTable->SetDescriptor(3, InSystem.GetTextureFactory().GetTextureCache("noise_curl_3d_xyzw").CachedTexture ? InSystem.GetTextureFactory().GetTextureCache("noise_curl_3d_xyzw").CachedTexture->GetShaderResourceView() : nullptr);
			_VolumetricCloudsDescriptorTable->SetDescriptor(4, InContext.GetBilinearWrapSampler());
			_VolumetricCloudsDescriptorTable->SetDescriptor(5, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());

			{
				
				ResourceTransition Transitions[] =
				{
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView(),	TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(),		TransitionState::TRANSITION_SHADER_WRITE)
				};
				ResourceTransitionScope VolumetricCloudsTransitionScope(*VolumetricCloudsCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				VolumetricCloudsCommandList->SetComputePipeline(*_Pipeline);
				VolumetricCloudsCommandList->SetComputeDescriptorTable(InContext, *_VolumetricCloudsDescriptorTable);
				VolumetricCloudsCommandList->Dispatch(
					Math::DivideRoundUp(InContext.GetWindow().GetWidth(), ThreadGroupCountX),
					Math::DivideRoundUp(InContext.GetWindow().GetHeight(), ThreadGroupCountY),
					1
				);
			}
		}

		void VolumetricCloudsPass::RenderDebug()
		{
			if (ImGui::TreeNode("Volumetric Clouds"))
			{
				ImGui::SliderFloat("Clouds bottom layer (m)", &VolumetricCloudsBottomLayerMeters, 0.0f, 10000.0f);
				ImGui::SliderFloat("Clouds top layer (m)", &VolumetricCloudsTopLayerMeters, 10000.0f, 20000.0f);
				ImGui::SliderFloat("Clouds Density", &VolumetricCloudsDensity, 0.0f, 0.1f);
				ImGui::SliderFloat("Clouds Noise scale", &VolumetricCloudsNoiseScale, 0.0f, 10.0f);
				ImGui::TreePop();
			}
		}
	}
}
