#include "GraphicsEngine/RenderPasses/PresentPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		PresentPass::PresentPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			Shader* ScreenVertex		= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_VERTEX, "ScreenVertex", "screen.vertex.hlsl"));
			Shader* SampleTexturePixel	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "SampleTexturePixel", "sampletexture.pixel.hlsl"));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER, RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL)
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
					ScreenVertex, SampleTexturePixel
				)
			);
		}

		PresentPass::~PresentPass()
		{
			DestroyDescriptorTable(_PresentDescriptorTable);
		}

		void PresentPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			//GraphicsCommandListScope PresentCommandList = InContext.CreateNewGraphicsCommandList(InContext.GetCurrentFrameBackBufferRenderPass(), "Present");
			CommandListScope PresentCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "Present");

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
