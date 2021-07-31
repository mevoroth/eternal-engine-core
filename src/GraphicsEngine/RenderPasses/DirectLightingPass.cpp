#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "GraphicsEngine/Renderer.hpp"
#include "GraphicData/GlobalResources.hpp"
#include "GraphicData/RenderTargetTexture.hpp"
#include "Graphics/GraphicsInclude.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		DirectLightingPass::DirectLightingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			ShaderCreateInformation ScreenVSCreateInformation(ShaderType::VS, "ScreenVS", "screen.vs.hlsl");
			Shader& ScreenVS = *InContext.GetShader(ScreenVSCreateInformation);
			ShaderCreateInformation DirectLightingPSCreateInformation(ShaderType::PS, "DirectLightingPS", "directlighting.ps.hlsl");
			Shader& DirectLightingPS = *InContext.GetShader(DirectLightingPSCreateInformation);

			_DirectLightingRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
					}
				)
			);

			_DirectLightingDescriptorTable = _DirectLightingRootSignature->CreateRootDescriptorTable(InContext);

			_DirectLightingRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InRenderer.GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView())
					}
				)
			);

			_DirectLightingPipeline = CreatePipeline(
				InContext,
				PipelineCreateInformation(
					*_DirectLightingRootSignature,
					InContext.GetEmptyInputLayout(),
					*_DirectLightingRenderPass,
					ScreenVS,
					DirectLightingPS
				)
			);
		}

		DirectLightingPass::~DirectLightingPass()
		{
			DestroyPipeline(_DirectLightingPipeline);
			DestroyRenderPass(_DirectLightingRenderPass);
			DestroyDescriptorTable(_DirectLightingDescriptorTable);
			DestroyRootSignature(_DirectLightingRootSignature);
		}

		void DirectLightingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandList* DirectLightingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "DirectLighting");

			_DirectLightingDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_DirectLightingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferAlbedo().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(3, InRenderer.GetGlobalResources().GetGBufferNormals().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(5, InContext.GetPointClampSampler());

			DirectLightingCommandList->Begin(InContext);
			{
				ResourceTransition Transitions[] =
				{
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferAlbedo().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferNormals().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView(),	TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),				TransitionState::TRANSITION_PIXEL_SHADER_READ)
				};
				ResourceTransitionScope GBufferToShaderReadScope(*DirectLightingCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));

				DirectLightingCommandList->BeginRenderPass(*_DirectLightingRenderPass);
				DirectLightingCommandList->SetGraphicsPipeline(*_DirectLightingPipeline);
				DirectLightingCommandList->SetGraphicsDescriptorTable(InContext, *_DirectLightingDescriptorTable);
				DirectLightingCommandList->DrawInstanced(6);
				DirectLightingCommandList->EndRenderPass();
			}
			DirectLightingCommandList->End();
		}
	}
}
