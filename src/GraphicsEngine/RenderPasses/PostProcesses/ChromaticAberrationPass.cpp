#include "GraphicsEngine/RenderPasses/PostProcesses/ChromaticAberrationPass.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Core;

		static constexpr int32_t ThreadGroupCountX = 32;
		static constexpr int32_t ThreadGroupCountY = 32;
		static constexpr int32_t ThreadGroupCountZ = 1;

		static ConfigurationSetting<Vector3> ChromaticAberrationOctaveColorMask("ChromaticAberrationOctaveColorMask", Vector3::One(), CHROMATIC_ABERRATION_OCTAVES_COUNT);
		static ConfigurationSetting<float> ChromaticAberrationOctaveDirection("ChromaticAberrationOctaveDirection", 0.0f, CHROMATIC_ABERRATION_OCTAVES_COUNT);
		static ConfigurationSetting<float> ChromaticAberrationOctaveMagnitude("ChromaticAberrationOctaveMagnitude", 1.0f, CHROMATIC_ABERRATION_OCTAVES_COUNT);
		static ConfigurationSetting<int> ChromaticAberrationOctavesCount("ChromaticAberrationOctavesCount", CHROMATIC_ABERRATION_OCTAVES_COUNT);
		static ConfigurationSetting<float> ChromaticAberrationOffsetDirection("ChromaticAberrationOffsetDirection", 0.0f);
		static ConfigurationSetting<float> ChromaticAberrationOffsetMagnitude("ChromaticAberrationOffsetMagnitude", 0.0f);
		static ConfigurationSetting<float> ChromaticAberrationStrength("ChromaticAberrationStrength", 0.0f);

		ChromaticAberrationPass::ChromaticAberrationPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _ChromaticAberrationConstantBuffer(InContext, "ChromaticAberrationBuffer")
		{
			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, "%d", ThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, "%d", ThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, "%d", ThreadGroupCountZ);


			Shader* ChromaticAberrationCompute = InContext.GetShader(
				ShaderCreateInformation(
					ShaderType::SHADER_TYPE_COMPUTE,
					"ChromaticAberration",
					"PostProcesses/chromaticaberration.compute.hlsl",
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

			_ChromaticAberrationDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					_RootSignature,
					ChromaticAberrationCompute
				)
			);
		}

		ChromaticAberrationPass::~ChromaticAberrationPass()
		{
		}

		void ChromaticAberrationPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope ChromaticAberrationCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "ChromaticAberration");

			_ChromaticAberrationDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_ChromaticAberrationDescriptorTable->SetDescriptor(1, _ChromaticAberrationConstantBuffer.GetView());
			_ChromaticAberrationDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView());
			_ChromaticAberrationDescriptorTable->SetDescriptor(3, InContext.GetBilinearWrapSampler());
			_ChromaticAberrationDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediate().GetUnorderedAccessView());

			{
				ResourceTransition Transitions[] =
				{
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView(),				TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
				};
				ResourceTransitionScope VolumetricCloudsTransitionScope(*ChromaticAberrationCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				{
					MapScope<ChromaticAberrationConstants> ChromaticAberrationConstantsMapScope(*_ChromaticAberrationConstantBuffer.ResourceBuffer);
					
					const Vector2 AspectRatio(
						1.0f / static_cast<float>(InContext.GetBackBufferViewport().GetWidth()),
						1.0f / static_cast<float>(InContext.GetBackBufferViewport().GetHeight())
					);
					
					const Vector2 PrecomputedOffsetDirection = Vector2(
						Math::Sin(Math::FMod(ChromaticAberrationOffsetDirection * Math::PI_2, Math::PI_2)),
						Math::Cos(Math::FMod(ChromaticAberrationOffsetDirection * Math::PI_2 + Math::PI_2, Math::PI_2))
					) * AspectRatio * ChromaticAberrationOffsetMagnitude;

					float ChromaticAberrationOctavesCountRcp = 1.0f / static_cast<float>(ChromaticAberrationOctavesCount);

					for (uint32_t OctaveIndex = 0; OctaveIndex < CHROMATIC_ABERRATION_OCTAVES_COUNT; ++OctaveIndex)
					{
						ChromaticAberrationConstantsMapScope->ChromaticAberrationOctaves[OctaveIndex].OctaveMask = ChromaticAberrationOctaveColorMask[OctaveIndex] * ChromaticAberrationOctavesCountRcp;
						ChromaticAberrationConstantsMapScope->ChromaticAberrationOctaves[OctaveIndex].OctaveDirection = Vector2(
							Math::Sin(Math::FMod(ChromaticAberrationOctaveDirection[OctaveIndex] * Math::PI_2, Math::PI_2)),
							Math::Cos(Math::FMod(ChromaticAberrationOctaveDirection[OctaveIndex] * Math::PI_2 + Math::PI_2, Math::PI_2))
							
						) * AspectRatio * ChromaticAberrationOctaveMagnitude[OctaveIndex] + PrecomputedOffsetDirection;
						ChromaticAberrationConstantsMapScope->ChromaticAberrationOctaves[OctaveIndex].OctaveStrength = ChromaticAberrationOctaveMagnitude[OctaveIndex];
					}

					ChromaticAberrationConstantsMapScope->ChromaticAberrationStrength = ChromaticAberrationStrength;

					ChromaticAberrationConstantsMapScope->ChromaticAberrationOctavesCount = static_cast<uint32_t>(ChromaticAberrationOctavesCount);
				}


				ChromaticAberrationCommandList->Transition(ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediate().GetUnorderedAccessView(), TransitionState::TRANSITION_SHADER_WRITE));

				ChromaticAberrationCommandList->SetComputePipeline(*_Pipeline);
				ChromaticAberrationCommandList->SetComputeDescriptorTable(InContext, *_ChromaticAberrationDescriptorTable);
				ChromaticAberrationCommandList->Dispatch(
					Math::DivideRoundUp(InContext.GetOutputDevice().GetWidth(), ThreadGroupCountX),
					Math::DivideRoundUp(InContext.GetOutputDevice().GetHeight(), ThreadGroupCountY),
					1
				);
			}

			InRenderer.GetGlobalResources().SwapIntermediateLuminanceTexture();
		}

		void ChromaticAberrationPass::RenderDebug()
		{
			if (ImGui::TreeNode("Chromatic aberration"))
			{
				for (uint32_t OctaveIndex = 0; OctaveIndex < CHROMATIC_ABERRATION_OCTAVES_COUNT; ++OctaveIndex)
				{
					char ChromaticAberrationOctaveColorMaskLabel[256];
					char ChromaticAberrationOctaveDirectionLabel[256];
					char ChromaticAberrationOctaveMagnitudeLabel[256];

					sprintf_s(ChromaticAberrationOctaveColorMaskLabel, "Chromatic Aberration Octave Color Mask %d", OctaveIndex);
					sprintf_s(ChromaticAberrationOctaveDirectionLabel, "Chromatic Aberration Octave Direction %d", OctaveIndex);
					sprintf_s(ChromaticAberrationOctaveMagnitudeLabel, "Chromatic Aberration Octave Magnitude %d", OctaveIndex);

					ImGui::ColorEdit3(ChromaticAberrationOctaveColorMaskLabel, &ChromaticAberrationOctaveColorMask[OctaveIndex].x);
					ImGui::SliderFloat(ChromaticAberrationOctaveDirectionLabel, &ChromaticAberrationOctaveDirection[OctaveIndex], 0.0f, 1.0f);
					ImGui::SliderFloat(ChromaticAberrationOctaveMagnitudeLabel, &ChromaticAberrationOctaveMagnitude[OctaveIndex], 0.0f, 100.0f);
				}
				ImGui::SliderInt("Chromatic Aberration Octave", &ChromaticAberrationOctavesCount, 1, CHROMATIC_ABERRATION_OCTAVES_COUNT);
				ImGui::SliderFloat("Chromatic Aberration Direction", &ChromaticAberrationOffsetDirection, 0.0f, 1.0f);
				ImGui::SliderFloat("Chromatic Aberration Offset Magnitude", &ChromaticAberrationOffsetMagnitude, 0.0f, 100.0f);
				ImGui::SliderFloat("Chromatic Aberration Strength", &ChromaticAberrationStrength, 0.0f, 10.0f);
				
				ImGui::TreePop();
			}
		}

		void ChromaticAberrationPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
		}

		void ChromaticAberrationPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
		}
	}
}
