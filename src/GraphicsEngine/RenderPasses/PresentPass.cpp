#include "GraphicsEngine/RenderPasses/PresentPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		PresentPass::PresentPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			ShaderCreateInformation ScreenVSCreateInformation(ShaderType::VS, "ScreenVS", "screen.vs.hlsl");
			Shader* ScreenVS = InContext.GetShader(ScreenVSCreateInformation);
			ShaderCreateInformation SampleTexturePSCreateInformation(ShaderType::PS, "SampleTexturePS", "sampletexture.ps.hlsl");
			Shader* SampleTexturePS = InContext.GetShader(SampleTexturePSCreateInformation);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
					}
				)
			);

			_PresentDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					*_RootSignature,
					InContext.GetEmptyInputLayout(),
					InContext.GetBackBufferRenderPass(),
					ScreenVS, SampleTexturePS
				)
			);
		}

		PresentPass::~PresentPass()
		{
			DestroyDescriptorTable(_PresentDescriptorTable);
		}

		void PresentPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope PresentCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "Present");

			_PresentDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView());
			_PresentDescriptorTable->SetDescriptor(1, InContext.GetPointClampSampler());

			{
				ResourceTransition LuminanceToCopySource(InRenderer.GetGlobalResources().GetGBufferLuminance().GetShaderResourceView(), TransitionState::TRANSITION_PIXEL_SHADER_READ);
				ResourceTransitionScope LuminanceToCopySourceScope(*PresentCommandList, &LuminanceToCopySource, 1);

				PresentCommandList->BeginRenderPass(InContext.GetCurrentFrameBackBufferRenderPass());
				PresentCommandList->SetGraphicsPipeline(*_Pipeline);
				PresentCommandList->SetGraphicsDescriptorTable(InContext, *_PresentDescriptorTable);
				PresentCommandList->DrawInstanced(6);
				PresentCommandList->EndRenderPass();
			}
		}
	}
}
