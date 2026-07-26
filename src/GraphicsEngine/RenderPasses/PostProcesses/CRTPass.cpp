#include "GraphicsEngine/RenderPasses/PostProcesses/CRTPass.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Core;

		static constexpr int32_t ThreadGroupCountX = 32;
		static constexpr int32_t ThreadGroupCountY = 32;
		static constexpr int32_t ThreadGroupCountZ = 1;

		static ConfigurationSetting<float> CRTStrength("CRTStrength", 0.25f);
		static ConfigurationSetting<float> CRTRadius("CRTRadius", 1.0f);

		CRTPass::CRTPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _CRTConstantBuffer(InContext, "CRTBuffer")
		{
			_IsPassEnabled = false;

			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, "%d", ThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, "%d", ThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, "%d", ThreadGroupCountZ);

			Shader* CRTCompute = InContext.GetShader(
				ShaderCreateInformation(
					ShaderType::SHADER_TYPE_COMPUTE,
					"CRT",
					"PostProcesses/crt.compute.hlsl",
					{
						"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
						"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
						"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString
					}
				)
			);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_COMPUTE)
					}
				)
			);

			_CRTDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					_RootSignature,
					CRTCompute
				)
			);
		}

		CRTPass::~CRTPass()
		{
		}

		void CRTPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope CRTCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "CRT");

			_CRTDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_CRTDescriptorTable->SetDescriptor(1, _CRTConstantBuffer.GetView());
			_CRTDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView());
			_CRTDescriptorTable->SetDescriptor(3, InContext.GetBilinearClampSampler());
			_CRTDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediate().GetUnorderedAccessView());

			{
				MapScope<CRTConstants> CRTConstantsMapScope(*_CRTConstantBuffer.ResourceBuffer);

				CRTConstantsMapScope->Strength = static_cast<float>(CRTStrength);
				CRTConstantsMapScope->Radius = static_cast<float>(CRTRadius);
				CRTConstantsMapScope->Padding = 0.0f;
			}

			{
				ResourceTransition Transitions[] =
				{
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView(), TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
				};
				ResourceTransitionScope TransitionScope(*CRTCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				CRTCommandList->Transition(ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediate().GetUnorderedAccessView(), TransitionState::TRANSITION_SHADER_WRITE));

				CRTCommandList->SetComputePipeline(*_Pipeline);
				CRTCommandList->SetComputeDescriptorTable(InContext, *_CRTDescriptorTable);
				CRTCommandList->Dispatch(
					Math::DivideRoundUp(InContext.GetOutputDevice().GetWidth(), ThreadGroupCountX),
					Math::DivideRoundUp(InContext.GetOutputDevice().GetHeight(), ThreadGroupCountY),
					1
				);
			}

			InRenderer.GetGlobalResources().SwapIntermediateLuminanceTexture();
		}

		void CRTPass::RenderDebug()
		{
			if (ImGui::TreeNode("Fish Eye"))
			{
				float Strength = static_cast<float>(CRTStrength);
				float Radius = static_cast<float>(CRTRadius);

				if (ImGui::SliderFloat("Strength", &Strength, 0.0f, 2.0f))
					static_cast<float&>(CRTStrength) = Strength;
				if (ImGui::SliderFloat("Radius", &Radius, 0.0f, 5.0f))
					static_cast<float&>(CRTRadius) = Radius;

				ImGui::TreePop();
			}
		}

		void CRTPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			OutInputs.InputViews.push_back(StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetShaderResourceView());
		}

		void CRTPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(),
				TransitionState::TRANSITION_SHADER_WRITE
			};
		}
	}
}

