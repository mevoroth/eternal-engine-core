#include "GraphicsEngine/RenderPasses/PostProcesses/BloomPass.hpp"
#include "Configuration/ConfigurationSetting.hpp"
#include "Core/System.hpp"
#include "Math/Math.hpp"
#include "Memory/StackMemory.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Core;

		static constexpr int32_t BloomThreadGroupCountX = 8;
		static constexpr int32_t BloomThreadGroupCountY = 8;
		static constexpr int32_t BloomThreadGroupCountZ = 1;

		static ConfigurationSetting<float> BloomThreshold("BloomThreshold", 1.0f);
		static ConfigurationSetting<float> BloomIntensity("BloomIntensity", 0.25f);
		static ConfigurationSetting<float> BloomSaturation("BloomSaturation", 1.0f);
		static ConfigurationSetting<float> BloomUpsampleRadius("BloomUpsampleRadius", 0.5f);

		BloomPass::BloomPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			using namespace Eternal::Memory;

			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, 4, "%d", BloomThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, 4, "%d", BloomThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, 4, "%d", BloomThreadGroupCountZ);

			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			{
				Shader* BloomDownsampleCompute = InContext.GetShader(
					ShaderCreateInformation(
						ShaderType::SHADER_TYPE_COMPUTE,
						"BloomDownsample",
						"PostProcesses/bloomdownsample.compute.hlsl",
						{
							"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
							"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
							"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString,
							"BLOOM_PASS_DOWNSAMPLE", "1"
						}
					)
				);

				_BloomDownsampleRootSignature = CreateRootSignature(
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
				_BloomDownsampleDescriptorTable	= _BloomDownsampleRootSignature->CreateRootDescriptorTable(InContext);
				_BloomDownsamplePipeline		= CreatePipeline(InContext, ComputePipelineCreateInformation(_BloomDownsampleRootSignature, BloomDownsampleCompute));
			}

			{
				Shader* BloomBlurHorizontalCompute = InContext.GetShader(
					ShaderCreateInformation(
						ShaderType::SHADER_TYPE_COMPUTE,
						"BloomBlurHorizontal",
						"PostProcesses/bloomblur.compute.hlsl",
						{
							"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
							"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
							"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString,
							"BLOOM_BLUR_HORIZONTAL", "1"
						}
					)
				);

				_BloomBlurHorizontalRootSignature = CreateRootSignature(
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
				_BloomBlurHorizontalDescriptorTable	= _BloomBlurHorizontalRootSignature->CreateRootDescriptorTable(InContext);
				_BloomBlurHorizontalPipeline		= CreatePipeline(InContext, ComputePipelineCreateInformation(_BloomBlurHorizontalRootSignature, BloomBlurHorizontalCompute));
			}

			{
				Shader* BloomBlurVerticalCompute = InContext.GetShader(
					ShaderCreateInformation(
						ShaderType::SHADER_TYPE_COMPUTE,
						"BloomBlurVertical",
						"PostProcesses/bloomblur.compute.hlsl",
						{
							"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
							"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
							"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString,
							"BLOOM_BLUR_VERTICAL", "1"
						}
					)
				);

				_BloomBlurVerticalRootSignature = CreateRootSignature(
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
				_BloomBlurVerticalDescriptorTable	= _BloomBlurVerticalRootSignature->CreateRootDescriptorTable(InContext);
				_BloomBlurVerticalPipeline			= CreatePipeline(InContext, ComputePipelineCreateInformation(_BloomBlurVerticalRootSignature, BloomBlurVerticalCompute));
			}

			{
				Shader* BloomCompositeCompute = InContext.GetShader(
					ShaderCreateInformation(
						ShaderType::SHADER_TYPE_COMPUTE,
						"BloomComposite",
						"PostProcesses/bloomcomposite.compute.hlsl",
						{
							"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
							"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
							"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString,
							"BLOOM_PASS_COMPOSITE", "1"
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
				_BloomCompositeDescriptorTable	= _RootSignature->CreateRootDescriptorTable(InContext);
				_Pipeline						= CreatePipeline(InContext, ComputePipelineCreateInformation(_RootSignature, BloomCompositeCompute));
			}

			_BloomConstantBuffer = CreateMultiBufferedBuffer(
				InContext,
				BufferResourceCreateInformation(
					InContext.GetDevice(),
					"BloomConstantBuffer",
					BufferCreateInformation(
						Format::FORMAT_UNKNOWN,
						BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
						sizeof(BloomConstants),
						1 + InRenderer.GetGlobalResources().GetGBufferLuminanceMipShaderResourceViews().size() * (2 + 1)
					),
					ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
				)
			);
		}

		BloomPass::~BloomPass()
		{
			DestroyMultiBufferedResource(_BloomConstantBuffer);

			DestroyDescriptorTable(_BloomCompositeDescriptorTable);

			DestroyDescriptorTable(_BloomBlurVerticalDescriptorTable);
			DestroyPipeline(_BloomBlurVerticalPipeline);
			DestroyRootSignature(_BloomBlurVerticalRootSignature);

			DestroyDescriptorTable(_BloomBlurHorizontalDescriptorTable);
			DestroyPipeline(_BloomBlurHorizontalPipeline);
			DestroyRootSignature(_BloomBlurHorizontalRootSignature);

			DestroyDescriptorTable(_BloomDownsampleDescriptorTable);
			DestroyPipeline(_BloomDownsamplePipeline);
			DestroyRootSignature(_BloomDownsampleRootSignature);
		}

		void BloomPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			(void)InSystem;

			CommandListScope BloomCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "BloomPass");

			const uint32_t FullWidth  = InContext.GetOutputDevice().GetWidth();
			const uint32_t FullHeight = InContext.GetOutputDevice().GetHeight();

			const vector<View*>& GBufferLuminanceMipShaderResourceViews = InRenderer.GetGlobalResources().GetGBufferLuminanceMipShaderResourceViews();
			const vector<View*>& GBufferLuminanceMipUnorderedAccessViews = InRenderer.GetGlobalResources().GetGBufferLuminanceMipUnorderedAccessViews();
			
			const vector<View*>& GBufferLuminanceIntermediateMipShaderResourceViews = InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediateMipShaderResourceViews();
			const vector<View*>& GBufferLuminanceIntermediateMipUnorderedAccessViews = InRenderer.GetGlobalResources().GetGBufferLuminanceIntermediateMipUnorderedAccessViews();

			const uint32_t BloomMipLevels = GBufferLuminanceMipShaderResourceViews.size();

			StackAllocation<View> BloomConstantsBufferViews(ETERNAL_STACK_VIEWS_PARAMETERS(BloomMipLevels * 3 + 1));

			MapScope<BloomConstants> BloomConstantsBufferMapScope(*_BloomConstantBuffer);
			uint32_t BloomConstantBufferIndex = 0;

			auto CreateBloomConstantBufferView = [this, &InContext, &BloomConstantsBufferViews, &BloomConstantsBufferMapScope, &BloomConstantBufferIndex](_In_ uint32_t MipWidth, _In_ uint32_t MipHeight, _In_ uint32_t InBloomMipLevel) mutable -> View*
			{
				{
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].ViewSizeAndInverseSize	= Vector4(MipWidth, MipHeight, 1.0f / MipWidth, 1.0f / MipHeight);
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomTexelSize			= Vector2(1.0f / static_cast<float>(MipWidth), 1.0f / static_cast<float>(MipHeight));
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomThreshold			= static_cast<float>(BloomThreshold);
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomIntensity			= static_cast<float>(BloomIntensity);
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomSaturation			= static_cast<float>(BloomSaturation);
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomDirtMaskIntensity	= 0.0f;
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomMipLevel			= InBloomMipLevel;
					BloomConstantsBufferMapScope[BloomConstantBufferIndex].BloomUpsampleRadius		= BloomUpsampleRadius;
				}
				
				ViewMetaData BloomConstantsMetaData;
				BloomConstantsMetaData.ConstantBufferView.BufferElementOffset	= BloomConstantBufferIndex;
				BloomConstantsMetaData.ConstantBufferView.BufferSize			= (*_BloomConstantBuffer)->GetBufferStride();

				++BloomConstantBufferIndex;

				return CreateConstantBufferView(
					ConstantBufferViewCreateInformation(
						InContext,
						&(*_BloomConstantBuffer),
						BloomConstantsMetaData
					),
					BloomConstantsBufferViews.SubAllocate()
				);
			};

			{
				CommandListEventScope BloomDownSample(BloomCommandList, InContext, "BloomDownSample");

				const uint32_t Mip0Width  = Math::Max(FullWidth  >> 1u, 1u);
				const uint32_t Mip0Height = Math::Max(FullHeight >> 1u, 1u);

				ResourceTransition DownsampleTransitions[] =
				{
					ResourceTransition({ ResourceSubResource(0), GBufferLuminanceMipShaderResourceViews[0] },	TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(1), GBufferLuminanceMipUnorderedAccessViews[1] },	TransitionState::TRANSITION_SHADER_WRITE)
				};
				BloomCommandList->Transition(DownsampleTransitions, ETERNAL_ARRAYSIZE(DownsampleTransitions));

				_BloomDownsampleDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
				_BloomDownsampleDescriptorTable->SetDescriptor(1, CreateBloomConstantBufferView(Mip0Width, Mip0Height, 0));
				_BloomDownsampleDescriptorTable->SetDescriptor(2, GBufferLuminanceMipShaderResourceViews[0]);
				_BloomDownsampleDescriptorTable->SetDescriptor(3, InContext.GetBilinearClampSampler());
				_BloomDownsampleDescriptorTable->SetDescriptor(4, GBufferLuminanceMipUnorderedAccessViews[1]);

				BloomCommandList->SetComputePipeline(*_BloomDownsamplePipeline);
				BloomCommandList->SetComputeDescriptorTable(InContext, *_BloomDownsampleDescriptorTable);
				BloomCommandList->Dispatch(
					Math::DivideRoundUp(Mip0Width,  static_cast<uint32_t>(BloomThreadGroupCountX)),
					Math::DivideRoundUp(Mip0Height, static_cast<uint32_t>(BloomThreadGroupCountY)),
					1u
				);

				for (uint32_t MipIndex = 1; MipIndex < BloomMipLevels - 1; ++MipIndex)
				{
					const uint32_t SourceWidth  = Math::Max(FullWidth  >> MipIndex, 1u);
					const uint32_t SourceHeight = Math::Max(FullHeight >> MipIndex, 1u);
					const uint32_t DestinationWidth  = Math::Max(SourceWidth  >> 1u, 1u);
					const uint32_t DestinationHeight = Math::Max(SourceHeight >> 1u, 1u);

					ResourceTransition MipTransitions[] =
					{
						ResourceTransition({ ResourceSubResource(MipIndex), GBufferLuminanceMipShaderResourceViews[MipIndex - 1] },	TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
						ResourceTransition({ ResourceSubResource(MipIndex + 1), GBufferLuminanceMipUnorderedAccessViews[MipIndex] },		TransitionState::TRANSITION_SHADER_WRITE)
					};
					BloomCommandList->Transition(MipTransitions, ETERNAL_ARRAYSIZE(MipTransitions));

					_BloomDownsampleDescriptorTable->SetDescriptor(1, CreateBloomConstantBufferView(DestinationWidth, DestinationHeight, MipIndex));
					_BloomDownsampleDescriptorTable->SetDescriptor(2, GBufferLuminanceMipShaderResourceViews[MipIndex]);
					_BloomDownsampleDescriptorTable->SetDescriptor(4, GBufferLuminanceMipUnorderedAccessViews[MipIndex + 1]);

					BloomCommandList->SetComputeDescriptorTable(InContext, *_BloomDownsampleDescriptorTable);
					BloomCommandList->Dispatch(
						Math::DivideRoundUp(DestinationWidth,  static_cast<uint32_t>(BloomThreadGroupCountX)),
						Math::DivideRoundUp(DestinationHeight, static_cast<uint32_t>(BloomThreadGroupCountY)),
						1u
					);
				}
			}

			{
				CommandListEventScope BloomSeparableBlur(BloomCommandList, InContext, "BloomSeparableBlur");

				for (int32_t MipIndex = static_cast<int32_t>(BloomMipLevels) - 1; MipIndex >= 0; --MipIndex)
				{
					const uint32_t MipWidth  = Math::Max(FullWidth  >> (static_cast<uint32_t>(MipIndex)), 1u);
					const uint32_t MipHeight = Math::Max(FullHeight >> (static_cast<uint32_t>(MipIndex)), 1u);

					{
						ResourceTransition HorizontalBlurTransitions[] =
						{
							ResourceTransition({ ResourceSubResource(MipIndex), GBufferLuminanceMipShaderResourceViews[MipIndex] },					TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
							ResourceTransition({ ResourceSubResource(MipIndex), GBufferLuminanceIntermediateMipUnorderedAccessViews[MipIndex] },	TransitionState::TRANSITION_SHADER_WRITE)
						};
						BloomCommandList->Transition(HorizontalBlurTransitions, ETERNAL_ARRAYSIZE(HorizontalBlurTransitions));

						_BloomBlurHorizontalDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
						_BloomBlurHorizontalDescriptorTable->SetDescriptor(1, CreateBloomConstantBufferView(MipWidth, MipHeight, static_cast<uint32_t>(MipIndex)));
						_BloomBlurHorizontalDescriptorTable->SetDescriptor(2, GBufferLuminanceMipShaderResourceViews[MipIndex]);
						_BloomBlurHorizontalDescriptorTable->SetDescriptor(3, InContext.GetBilinearClampSampler());
						_BloomBlurHorizontalDescriptorTable->SetDescriptor(4, GBufferLuminanceIntermediateMipUnorderedAccessViews[MipIndex]);

						BloomCommandList->SetComputePipeline(*_BloomBlurHorizontalPipeline);
						BloomCommandList->SetComputeDescriptorTable(InContext, *_BloomBlurHorizontalDescriptorTable);
						BloomCommandList->Dispatch(
							Math::DivideRoundUp(MipWidth,  static_cast<uint32_t>(BloomThreadGroupCountX)),
							Math::DivideRoundUp(MipHeight, static_cast<uint32_t>(BloomThreadGroupCountY)),
							1u
						);
					}

					{
						ResourceTransition VerticalBlurTransitions[] =
						{
							ResourceTransition({ ResourceSubResource(MipIndex), GBufferLuminanceIntermediateMipShaderResourceViews[MipIndex] },	TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
							ResourceTransition({ ResourceSubResource(MipIndex), GBufferLuminanceMipUnorderedAccessViews[MipIndex] },			TransitionState::TRANSITION_SHADER_WRITE),
							{}
						};

						uint32_t VerticalBlurTransitionsCount = 2;

						if (MipIndex < static_cast<int32_t>(BloomMipLevels) - 1)
						{
							VerticalBlurTransitions[VerticalBlurTransitionsCount++] = ResourceTransition(
								{
									ResourceSubResource(MipIndex + 1),
									GBufferLuminanceMipShaderResourceViews[MipIndex + 1]
								},
								TransitionState::TRANSITION_NON_PIXEL_SHADER_READ
							);
						}

						BloomCommandList->Transition(VerticalBlurTransitions, VerticalBlurTransitionsCount);

						_BloomBlurVerticalDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
						_BloomBlurVerticalDescriptorTable->SetDescriptor(1, CreateBloomConstantBufferView(MipWidth, MipHeight, static_cast<uint32_t>(MipIndex)));
						_BloomBlurVerticalDescriptorTable->SetDescriptor(2, GBufferLuminanceIntermediateMipShaderResourceViews[MipIndex]);
						_BloomBlurVerticalDescriptorTable->SetDescriptor(3, MipIndex < static_cast<int32_t>(BloomMipLevels) - 1 ? GBufferLuminanceMipShaderResourceViews[MipIndex + 1] : InSystem.GetTextureFactory().GetTextureCache("black").CachedTexture->GetShaderResourceView());
						_BloomBlurVerticalDescriptorTable->SetDescriptor(4, InContext.GetBilinearClampSampler());
						_BloomBlurVerticalDescriptorTable->SetDescriptor(5, GBufferLuminanceMipUnorderedAccessViews[MipIndex]);

						BloomCommandList->SetComputePipeline(*_BloomBlurVerticalPipeline);
						BloomCommandList->SetComputeDescriptorTable(InContext, *_BloomBlurVerticalDescriptorTable);
						BloomCommandList->Dispatch(
							Math::DivideRoundUp(MipWidth,  static_cast<uint32_t>(BloomThreadGroupCountX)),
							Math::DivideRoundUp(MipHeight, static_cast<uint32_t>(BloomThreadGroupCountY)),
							1u
						);
					}
				}
			}

			{
				CommandListEventScope BloomComposite(BloomCommandList, InContext, "BloomComposite");

				ResourceTransition CompositeTransitions[] =
				{
					ResourceTransition({ ResourceSubResource(0), GBufferLuminanceMipShaderResourceViews[0] },				TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(0), GBufferLuminanceIntermediateMipUnorderedAccessViews[0] },	TransitionState::TRANSITION_SHADER_WRITE)
				};
				BloomCommandList->Transition(CompositeTransitions, ETERNAL_ARRAYSIZE(CompositeTransitions));

				_BloomCompositeDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
				_BloomCompositeDescriptorTable->SetDescriptor(1, CreateBloomConstantBufferView(FullWidth, FullHeight, 0u));
				_BloomCompositeDescriptorTable->SetDescriptor(2, GBufferLuminanceMipShaderResourceViews[0]);
				_BloomCompositeDescriptorTable->SetDescriptor(3, InContext.GetBilinearClampSampler());
				_BloomCompositeDescriptorTable->SetDescriptor(4, GBufferLuminanceIntermediateMipUnorderedAccessViews[0]);

				BloomCommandList->SetComputePipeline(*_Pipeline);
				BloomCommandList->SetComputeDescriptorTable(InContext, *_BloomCompositeDescriptorTable);
				BloomCommandList->Dispatch(
					Math::DivideRoundUp(FullWidth,  static_cast<uint32_t>(BloomThreadGroupCountX)),
					Math::DivideRoundUp(FullHeight, static_cast<uint32_t>(BloomThreadGroupCountY)),
					1u
				);

				InRenderer.GetGlobalResources().SwapIntermediateLuminanceTexture();
			}
		}

		void BloomPass::RenderDebug()
		{
			if (ImGui::TreeNode("Bloom"))
			{
				float Threshold  = static_cast<float>(BloomThreshold);
				float Intensity  = static_cast<float>(BloomIntensity);
				float Saturation = static_cast<float>(BloomSaturation);

				if (ImGui::SliderFloat("Threshold",       &Threshold,  0.0f, 5.0f))
					static_cast<float&>(BloomThreshold)  = Threshold;
				if (ImGui::SliderFloat("Intensity",       &Intensity,  0.0f, 2.0f))
					static_cast<float&>(BloomIntensity)  = Intensity;
				if (ImGui::SliderFloat("Saturation",      &Saturation, 0.0f, 1.0f))
					static_cast<float&>(BloomSaturation) = Saturation;
				ImGui::SliderFloat("Upsample Radius",     &BloomUpsampleRadius, 0.1f, 2.0f);

				ImGui::TreePop();
			}
		}

		void BloomPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			OutInputs.InputViews.push_back(StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetShaderResourceView());
		}

		void BloomPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(),
				TransitionState::TRANSITION_SHADER_WRITE
			};
		}
	}
}
