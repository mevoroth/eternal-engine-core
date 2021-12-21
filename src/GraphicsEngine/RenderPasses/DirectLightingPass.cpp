#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "Light/Light.hpp"
#include "Core/System.hpp"
#include "Types/Types.hpp"
#include "HLSLDirectLighting.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::Types;
		using namespace Eternal::Components;
		using namespace Eternal::HLSL;

		DirectLightingPass::DirectLightingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			ShaderCreateInformation ScreenVSCreateInformation(ShaderType::VS, "ScreenVS", "screen.vs.hlsl");
			Shader* ScreenVS = InContext.GetShader(ScreenVSCreateInformation);
			ShaderCreateInformation DirectLightingPSCreateInformation(ShaderType::PS, "DirectLightingPS", "directlighting.ps.hlsl");
			Shader* DirectLightingPS = InContext.GetShader(DirectLightingPSCreateInformation);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
					}
				)
			);

			_DirectLightingDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_DirectLightingRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InRenderer.GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView())
					}
				)
			);

			_Pipeline = CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					*_RootSignature,
					InContext.GetEmptyInputLayout(),
					_DirectLightingRenderPass,
					ScreenVS,
					DirectLightingPS
				)
			);

			{
				_DirectLightingConstantBuffer = CreateMultiBufferedBuffer(
					InContext,
					BufferResourceCreateInformation(
						InContext.GetDevice(),
						"DirectLightingBuffer",
						BufferCreateInformation(
							Format::FORMAT_UNKNOWN,
							BufferResourceUsage::BUFFER_RESOURCE_USAGE_CONSTANT_BUFFER,
							sizeof(DirectLightingConstants)
						),
						ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
					)
				);

				ViewMetaData MetaData;
				MetaData.ConstantBufferView.BufferSize = sizeof(DirectLightingConstants);
				_DirectLightingConstantBufferView = CreateMultiBufferedConstantBufferView(
					*_DirectLightingConstantBuffer,
					ConstantBufferViewCreateInformation(
						InContext,
						*_DirectLightingConstantBuffer,
						MetaData
					)
				);
			}

			{
				_DirectLightingLightsBuffer = CreateMultiBufferedBuffer(
					InContext,
					BufferResourceCreateInformation(
						InContext.GetDevice(),
						"DirectLightingLightsBuffer",
						BufferCreateInformation(
							Format::FORMAT_UNKNOWN,
							BufferResourceUsage::BUFFER_RESOURCE_USAGE_STRUCTURED_BUFFER,
							sizeof(LightInformation),
							1024
						),
						ResourceMemoryType::RESOURCE_MEMORY_TYPE_GPU_UPLOAD
					)
				);

				ViewMetaData MetaData;
				MetaData.ShaderResourceViewBuffer.NumElements			= 1024;
				MetaData.ShaderResourceViewBuffer.StructureByteStride	= sizeof(LightInformation);
				_DirectLightingLightsBufferView = CreateMultiBufferedShaderResourceView(
					*_DirectLightingLightsBuffer,
					ShaderResourceViewStructuredBufferCreateInformation(
						InContext,
						*_DirectLightingLightsBuffer,
						MetaData
					)
				);
			}
		}

		DirectLightingPass::~DirectLightingPass()
		{
			DestroyRenderPass(_DirectLightingRenderPass);
			DestroyDescriptorTable(_DirectLightingDescriptorTable);
		}

		void DirectLightingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandList* DirectLightingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "DirectLighting");

			const vector<Light*>& Lights = InSystem.GetRenderFrame().Lights;
			{
				MapRange LightsBufferMapRange(sizeof(LightInformation) * 1024);
				MapScope<LightInformation> LightsBufferMapScope(*_DirectLightingLightsBuffer, LightsBufferMapRange);

				for (uint32_t LightIndex = 0; LightIndex < Lights.size(); ++LightIndex)
				{
					LightsBufferMapScope.GetDataPointer()[LightIndex] =
					{
						Vector4(Lights[LightIndex]->GetPosition(), 1.0f),
						Lights[LightIndex]->GetColor() * Lights[LightIndex]->GetIntensity(), 0.0f,
						Lights[LightIndex]->GetDirection(), 0.0f
					};
				}
			}

			{
				MapRange DirectLightingMapRange(sizeof(DirectLightingConstants));
				MapScope<DirectLightingConstants> DirectLightingConstantsMapScope(*_DirectLightingConstantBuffer, DirectLightingMapRange);
				DirectLightingConstantsMapScope.GetDataPointer()->LightsCount = 1;// static_cast<uint32_t>(Lights.size());
			}

			View* DirectLightingConstantBufferView = *_DirectLightingConstantBufferView;
			View* DirectLightingLightsBufferView = *_DirectLightingLightsBufferView;
			_DirectLightingDescriptorTable->SetDescriptor(0, DirectLightingConstantBufferView);
			_DirectLightingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_DirectLightingDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(3, InRenderer.GetGlobalResources().GetGBufferAlbedo().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetGBufferNormals().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(5, InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(6, DirectLightingLightsBufferView);
			_DirectLightingDescriptorTable->SetDescriptor(7, InContext.GetPointClampSampler());

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
				DirectLightingCommandList->SetGraphicsPipeline(*_Pipeline);
				DirectLightingCommandList->SetGraphicsDescriptorTable(InContext, *_DirectLightingDescriptorTable);
				DirectLightingCommandList->DrawInstanced(6);
				DirectLightingCommandList->EndRenderPass();
			}
			DirectLightingCommandList->End();
		}
	}
}
