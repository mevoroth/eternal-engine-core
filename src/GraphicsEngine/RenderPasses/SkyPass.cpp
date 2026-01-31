#include "GraphicsEngine/RenderPasses/SkyPass.hpp"
#include "Core/System.hpp"
#include "Light/Light.hpp"
#include "Math/Math.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		float SkyPass::AtmospherePhaseG[]	= { 0.0f, 0.0f };
		float SkyPass::AtmospherePhaseBlend	= 0.5f;

		ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(SkyPass::AtmospherePhaseG) == VOLUMETRIC_PHASE_COUNT, "AtmospherePhaseG not matching count");

		SkyPass::SkyPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _MipMapConstantBuffer(InContext, "SkyMipMapConstantBuffer")
		{
			std::vector<string> Defines =
			{
				"USE_FULLSCREEN_MODE",					"FULLSCREEN_MODE_TRIANGLE",
				"USE_MULTIPLE_LAYER_RENDER_TARGETS",	"MULTIPLE_LAYER_RENDER_TARGETS_CUBEMAP"
			};

			Shader* CubeMapVertex = InContext.GetShader(ShaderCreateInformation(
				ShaderType::SHADER_TYPE_VERTEX,
				"CubeMapVertex",
				"screen.vertex.hlsl",
				Defines
			));
			Shader* AtmosphereCubeMapPixel = InContext.GetShader(ShaderCreateInformation(
				ShaderType::SHADER_TYPE_PIXEL,
				"AtmosphereCubeMapPixel",
				"Volumetrics/atmosphere.pixel.hlsl",
				Defines
			));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL)
					}
				)
			);

			_SkyDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_SkyRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InRenderer.GetGlobalResources().GetSkyViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InRenderer.GetGlobalResources().GetSky().GetRenderTargetDepthStencilView())
					}
				)
			);

			_Pipeline = CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					_RootSignature,
					InContext.GetEmptyInputLayout(),
					_SkyRenderPass,
					CubeMapVertex,
					AtmosphereCubeMapPixel
				)
			);

			_SkyMipGenerationDescriptorTable = InRenderer.GetMipMapGeneration().CreateDescriptorTable(InContext);
		}

		SkyPass::~SkyPass()
		{
			DestroyRenderPass(_SkyRenderPass);
			DestroyDescriptorTable(_SkyDescriptorTable);
		}

		void SkyPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope SkyCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "SkyPass");
			{
				ResourceTransition SkyTransition(InRenderer.GetGlobalResources().GetSky().GetRenderTargetDepthStencilView(), TransitionState::TRANSITION_RENDER_TARGET);
				SkyCommandList->Transition(SkyTransition);
			}
			{
				const vector<ObjectsList<Light>::InstancedObjects>& Lights = InSystem.GetRenderFrame().Lights;

				MapRange AtmosphereMapRange(sizeof(AtmosphereConstants));
				MapScope<AtmosphereConstants> AtmosphereConstantsMapScope(InRenderer.GetGlobalResources().GetAtmosphereConstantBuffer(), AtmosphereMapRange);
				for (int PhaseGIndex = 0; PhaseGIndex < VOLUMETRIC_PHASE_COUNT; ++PhaseGIndex)
					AtmosphereConstantsMapScope->AtmospherePhaseG[PhaseGIndex]		= 1.0f - AtmospherePhaseG[PhaseGIndex];
				AtmosphereConstantsMapScope->AtmospherePhaseBlend					= AtmospherePhaseBlend;
				AtmosphereConstantsMapScope->AtmosphereDirectionalLightDirection	= Lights[0].Object->GetDirection();
				AtmosphereConstantsMapScope->AtmosphereDirectionalLightIlluminance	= Lights[0].Object->GetColor() * Lights[0].Object->GetIntensity();
			}
			{
				_SkyDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetAtmosphereConstantBufferView());
				_SkyDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetSkyViewCubeMapConstantBufferView());

				CommandListEventScope SkyCubemapGeneration(SkyCommandList, InContext, "SkyCubemapGeneration");
				SkyCommandList->BeginRenderPass(*_SkyRenderPass);
				SkyCommandList->SetGraphicsPipeline(*_Pipeline);
				SkyCommandList->SetGraphicsDescriptorTable(InContext, *_SkyDescriptorTable);
				SkyCommandList->DrawInstanced(3, 6);
				SkyCommandList->EndRenderPass();
			}

			//////////////////////////////////////////////////////////////////////////

			ResourceTransition SkyTransition(InRenderer.GetGlobalResources().GetSky().GetShaderResourceView(), TransitionState::TRANSITION_NON_PIXEL_SHADER_READ);
			SkyCommandList->Transition(SkyTransition);

			{
				MapRange MipMapBufferMapRange(sizeof(MipMapConstants));
				MapScope<MipMapConstants> MipMapMapScope(*_MipMapConstantBuffer.ResourceBuffer, MipMapBufferMapRange);

				View* SkyMipView = InRenderer.GetGlobalResources().GetSkyMipShaderResourceViews()[0];
				MipMapMapScope->TextureSize = Uint4(SkyMipView->GetResource().GetWidth(), SkyMipView->GetResource().GetHeight(), SkyMipView->GetResource().GetDepthOrArraySize(), 0);
			}

			const vector<View*>& SkyMipUnorderedAccessViews = InRenderer.GetGlobalResources().GetSkyMipUnorderedAccessViews();
			for (uint32_t SkyMip = 0, SkyMipCount = static_cast<uint32_t>(SkyMipUnorderedAccessViews.size()) - 1; SkyMip < SkyMipCount; ++SkyMip)
			{
				View* CurrentMip0 = InRenderer.GetGlobalResources().GetSkyMipShaderResourceViews()[SkyMip];
				View* CurrentMip1 = SkyMipUnorderedAccessViews[SkyMip + 1];

				uint8_t Mip0 = static_cast<uint8_t>(SkyMip);
				uint8_t Mip1 = Mip0 + 1;

				ResourceTransition Transitions[] =
				{
					ResourceTransition({ ResourceSubResource(0, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(1, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(2, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(3, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(4, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(5, Mip0), CurrentMip0 }, TransitionState::TRANSITION_NON_PIXEL_SHADER_READ),
					ResourceTransition({ ResourceSubResource(0, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE),
					ResourceTransition({ ResourceSubResource(1, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE),
					ResourceTransition({ ResourceSubResource(2, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE),
					ResourceTransition({ ResourceSubResource(3, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE),
					ResourceTransition({ ResourceSubResource(4, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE),
					ResourceTransition({ ResourceSubResource(5, Mip1), CurrentMip1 }, TransitionState::TRANSITION_SHADER_WRITE)
				};
				ResourceTransitionScope SkyToShaderWriteScope(*SkyCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				_SkyMipGenerationDescriptorTable->SetDescriptor(0, CurrentMip0);
				_SkyMipGenerationDescriptorTable->SetDescriptor(1, InContext.GetBilinearClampSampler());
				_SkyMipGenerationDescriptorTable->SetDescriptor(2, SkyMipUnorderedAccessViews[SkyMip + 1]);
				_SkyMipGenerationDescriptorTable->SetDescriptor(3, static_cast<View*>(*_MipMapConstantBuffer.ResourceView));

				uint32_t ThreadGroupCountLog2 = Math::Min(SkyMipCount - SkyMip, static_cast<uint32_t>(MipMapThreadGroupCount::MIPMAP_THREAD_GROUP_COUNT_8));
				uint32_t ThreadGroupCount = 1 << (SkyMip + 4);

				CommandListEventScope SkyCubemapMipGeneration(SkyCommandList, InContext, "SkyCubemapMipGeneration");
				SkyCommandList->SetComputePipeline(InRenderer.GetMipMapGeneration().GetPipeline(
					MipMapTextureType::MIPMAP_TEXTURE_TYPE_2D_ARRAY,
					MipMapTextureFormat::MIPMAP_TEXTURE_FORMAT_RGB111110_FLOAT,
					static_cast<MipMapThreadGroupCount>(ThreadGroupCountLog2)
				));
				SkyCommandList->SetComputeDescriptorTable(InContext, *_SkyMipGenerationDescriptorTable);
				SkyCommandList->Dispatch(
					Math::DivideRoundUp(CurrentMip0->GetResource().GetWidth(), ThreadGroupCount),
					Math::DivideRoundUp(CurrentMip0->GetResource().GetHeight(), ThreadGroupCount),
					6
				);
			}
		}

		void SkyPass::RenderDebug()
		{
			if (ImGui::TreeNode("Atmosphere"))
			{
				for (int PhaseGIndex = 0; PhaseGIndex < VOLUMETRIC_PHASE_COUNT; ++PhaseGIndex)
				{
					char PhaseLabel[32];
					sprintf_s(PhaseLabel, "Atmosphere Phase G %d", PhaseGIndex);
					ImGui::SliderFloat(PhaseLabel, &AtmospherePhaseG[PhaseGIndex], -1.0f, 1.0f);
				}
				ImGui::SliderFloat("Atmosphere Phase Blend", &AtmospherePhaseBlend, 0.0f, 1.0f);
				ImGui::TreePop();
			}
		}

		void SkyPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			(void)OutInputs;
		}

		void SkyPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetSky().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetSky().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_SHADER_WRITE
			};
		}
	}
}
