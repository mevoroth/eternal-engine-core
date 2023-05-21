#include "GraphicsEngine/RenderPasses/SkyPass.hpp"
#include "Core/System.hpp"
#include "Light/Light.hpp"
#include "imgui.h"

namespace Eternal
{
	namespace GraphicsEngine
	{
		float SkyPass::AtmospherePhaseG[]	= { 0.0f, 0.0f };
		float SkyPass::AtmospherePhaseBlend	= 0.5f;

		ETERNAL_STATIC_ASSERT(ETERNAL_ARRAYSIZE(SkyPass::AtmospherePhaseG) == VOLUMETRIC_PHASE_COUNT, "AtmospherePhaseG not matching count");

		SkyPass::SkyPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			std::vector<string> Defines =
			{
				"USE_FULLSCREEN_MODE",					"FULLSCREEN_MODE_TRIANGLE",
				"USE_MULTIPLE_LAYER_RENDER_TARGETS",	"MULTIPLE_LAYER_RENDER_TARGETS_CUBEMAP"
			};

			Shader* CubeMapVS = InContext.GetShader(ShaderCreateInformation(
				ShaderType::VS,
				"CubeMapVS",
				"screen.vs.hlsl",
				Defines
			));
			Shader* AtmosphereCubeMapPS = InContext.GetShader(ShaderCreateInformation(
				ShaderType::PS,
				"AtmosphereCubeMapPS",
				"Volumetrics\\atmosphere.ps.hlsl",
				Defines
			));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
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
					*_RootSignature,
					InContext.GetEmptyInputLayout(),
					_SkyRenderPass,
					CubeMapVS,
					AtmosphereCubeMapPS
				)
			);
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
				const vector<ObjectsList<Light>::InstancedObjects>& Lights = InSystem.GetRenderFrame().Lights;

				MapRange AtmosphereMapRange(sizeof(AtmosphereConstants));
				MapScope<AtmosphereConstants> AtmosphereConstantsMapScope(InRenderer.GetGlobalResources().GetAtmosphereConstantBuffer(), AtmosphereMapRange);
				for (int PhaseGIndex = 0; PhaseGIndex < VOLUMETRIC_PHASE_COUNT; ++PhaseGIndex)
					AtmosphereConstantsMapScope->AtmospherePhaseG[PhaseGIndex]		= 1.0f - AtmospherePhaseG[PhaseGIndex];
				AtmosphereConstantsMapScope->AtmospherePhaseBlend					= AtmospherePhaseBlend;
				AtmosphereConstantsMapScope->AtmosphereDirectionalLightDirection	= Lights[0].Object->GetDirection();
				AtmosphereConstantsMapScope->AtmosphereDirectionalLightIlluminance	= Lights[0].Object->GetColor() * Lights[0].Object->GetIntensity();
			}

			_SkyDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetAtmosphereConstantBufferView());
			_SkyDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetSkyViewCubeMapConstantBufferView());

			SkyCommandList->BeginRenderPass(*_SkyRenderPass);
			SkyCommandList->SetGraphicsPipeline(*_Pipeline);
			SkyCommandList->SetGraphicsDescriptorTable(InContext, *_SkyDescriptorTable);
			SkyCommandList->DrawInstanced(3, 6);
			SkyCommandList->EndRenderPass();
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
			}
		}
	}
}