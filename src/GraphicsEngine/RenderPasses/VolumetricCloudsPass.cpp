#include "GraphicsEngine/RenderPasses/VolumetricCloudsPass.hpp"
#include "HLSLVolumetricClouds.hpp"
#include "imgui.h"

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
		static float VolumetricCloudsPhaseG[]			= { 0.0f, 0.0f };
		static float VolumetricCloudsBlend				= 0.5f;

		ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(VolumetricCloudsPhaseG) == VOLUMETRIC_PHASE_COUNT, "VolumeetricCloudsPhaseG not matching count");

		VolumetricCloudsPass::VolumetricCloudsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, "%d", ThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, "%d", ThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, "%d", ThreadGroupCountZ);

			ShaderCreateInformation VolumetricCloudsCSCreateInformation(
				ShaderType::CS, "VolumetricClouds", "volumetricclouds.cs.hlsl",
				{
					"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
					"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
					"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString
				}
			);
			Shader* VolumetricCloudsCS = InContext.GetShader(VolumetricCloudsCSCreateInformation);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS)
					}
				)
			);

			_VolumetricCloudsDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					*_RootSignature,
					VolumetricCloudsCS
				)
			);


			{
				_VolumetricCloudsConstantBuffer = CreateMultiBufferedBuffer(
					InContext,
					BufferResourceCreateInformation(
						InContext.GetDevice(),
						"VolumetricCloudsBuffer",
						BufferCreateInformation(
							Format::FORMAT_UNKNOWN,
							BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
							sizeof(VolumetricCloudsConstants)
						),
						ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
					)
				);

				ViewMetaData MetaData;
				MetaData.ConstantBufferView.BufferSize = sizeof(VolumetricCloudsConstants);
				_VolumetricCloudsConstantBufferView = CreateMultiBufferedConstantBufferView(
					*_VolumetricCloudsConstantBuffer,
					ConstantBufferViewCreateInformation(
						InContext,
						*_VolumetricCloudsConstantBuffer,
						MetaData
					)
				);
			}

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
				MapScope<VolumetricCloudsConstants> VolumetricCloudsConstantsMapScope(*_VolumetricCloudsConstantBuffer, VolumetricCloudsMapRange);
				VolumetricCloudsConstantsMapScope->BottomLayerRadiusMetersSquared	= VolumetricCloudsBottomLayerMeters * VolumetricCloudsBottomLayerMeters;
				VolumetricCloudsConstantsMapScope->TopLayerRadiusMetersSquared		= VolumetricCloudsTopLayerMeters * VolumetricCloudsTopLayerMeters;
				VolumetricCloudsConstantsMapScope->MaxStepsRcp						= 1.0f / static_cast<float>(VolumetricCloudsMaxSteps);
				VolumetricCloudsConstantsMapScope->MaxSteps							= VolumetricCloudsMaxSteps;

				for (int PhaseGIndex = 0; PhaseGIndex < VOLUMETRIC_PHASE_COUNT; ++PhaseGIndex)
					VolumetricCloudsConstantsMapScope->PhaseG[PhaseGIndex]	= VolumetricCloudsPhaseG[PhaseGIndex];
				VolumetricCloudsConstantsMapScope->PhaseBlend				= VolumetricCloudsBlend;
			}

			View* VolumetricCloudsConstantBufferView = *_VolumetricCloudsConstantBufferView;
			_VolumetricCloudsDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_VolumetricCloudsDescriptorTable->SetDescriptor(1, VolumetricCloudsConstantBufferView);
			_VolumetricCloudsDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_VolumetricCloudsDescriptorTable->SetDescriptor(3, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());

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

		void VolumetricCloudsPass::DebugRender()
		{
			ImGui::Begin("Volumetric Clouds");
			ImGui::SliderFloat("Clouds bottom layer (m)", &VolumetricCloudsBottomLayerMeters, 0.0f, 1000.0f);
			ImGui::SliderFloat("Clouds top layer (m)", &VolumetricCloudsTopLayerMeters, 10000.0f, 20000.0f);
			for (int PhaseGIndex = 0; PhaseGIndex < VOLUMETRIC_PHASE_COUNT; ++PhaseGIndex)
			{
				char PhaseLabel[32];
				sprintf_s(PhaseLabel, "Clouds Phase G %d", PhaseGIndex);
				ImGui::SliderFloat(PhaseLabel, &VolumetricCloudsPhaseG[PhaseGIndex], -1.0f, 1.0f);
			}
			ImGui::SliderFloat("Clouds Phase Blend", &VolumetricCloudsBlend, 0.0f, 1.0f);
			ImGui::End();
		}
	}
}
