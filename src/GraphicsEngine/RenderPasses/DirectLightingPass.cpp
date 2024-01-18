#include "GraphicsEngine/RenderPasses/DirectLightingPass.hpp"
#include "GraphicsEngine/StencilAllocation.hpp"
#include "Light/Light.hpp"
#include "Core/System.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::Types;
		using namespace Eternal::Components;
		using namespace Eternal::HLSL;

		DirectLightingPass::DirectLightingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _DirectLightingConstantBuffer(InContext, "DirectLightingBuffer")
			, _DirectLightingLightsBuffer(InContext, "DirectLightingLightsBuffer", 1024)
		{
			Shader* ScreenVertex		= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_VERTEX, "ScreenVertex", "screen.vertex.hlsl"));
			Shader* DirectLightingPixel	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "DirectLightingPixel", "directlighting.pixel.hlsl"));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL)
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
					},
					InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),
					RenderTargetOperator::Load_Store
				)
			);

			StencilTest::FaceOperator FrontFace;
			FrontFace.Comparison = ComparisonFunction::COMPARISON_FUNCTION_EQUAL;

			StencilTest::FaceOperator BackFace;
			BackFace.Comparison = ComparisonFunction::COMPARISON_FUNCTION_NEVER;

			DepthStencil DepthStencilTestNoneReadOpaqueStencil(
				DepthStencilNoneNone.GetDepthTest(),
				StencilTest(
					FrontFace,
					BackFace,
					StencilAllocation::OpaqueStencil,
					0x0,
					StencilAllocation::OpaqueStencil
				)
			);

			_Pipeline = CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					_RootSignature,
					InContext.GetEmptyInputLayout(),
					_DirectLightingRenderPass,
					ScreenVertex,
					DirectLightingPixel,
					DepthStencilTestNoneReadOpaqueStencil
				)
			);
		}

		DirectLightingPass::~DirectLightingPass()
		{
			DestroyRenderPass(_DirectLightingRenderPass);
			DestroyDescriptorTable(_DirectLightingDescriptorTable);
		}

		void DirectLightingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope DirectLightingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "DirectLighting");

			const vector<ObjectsList<Light>::InstancedObjects>& Lights = InSystem.GetRenderFrame().Lights;
			{
				MapRange LightsBufferMapRange(sizeof(LightInformation) * (*_DirectLightingLightsBuffer.ResourceBuffer)->GetElementCount());
				MapScope<LightInformation> LightsBufferMapScope(*_DirectLightingLightsBuffer.ResourceBuffer, LightsBufferMapRange);

				for (uint32_t LightIndex = 0; LightIndex < Lights.size(); ++LightIndex)
				{
					LightsBufferMapScope.GetDataPointer()[LightIndex] =
					{
						Vector4(Lights[LightIndex].Object->GetPosition(), 1.0f),
						Lights[LightIndex].Object->GetColor() * Lights[LightIndex].Object->GetIntensity(), static_cast<uint32_t>(Lights[LightIndex].Object->GetLightType()),
						Lights[LightIndex].Object->GetDirection(), 0.0f
					};
				}
			}

			{
				MapRange DirectLightingMapRange(sizeof(DirectLightingConstants));
				MapScope<DirectLightingConstants> DirectLightingConstantsMapScope(*_DirectLightingConstantBuffer.ResourceBuffer, DirectLightingMapRange);
				DirectLightingConstantsMapScope.GetDataPointer()->LightsCount = 1;// static_cast<uint32_t>(Lights.size());
			}

			_DirectLightingDescriptorTable->SetDescriptor(0, _DirectLightingConstantBuffer.GetView());
			_DirectLightingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_DirectLightingDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(3, InRenderer.GetGlobalResources().GetGBufferAlbedo().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetGBufferNormals().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(5, InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetShaderResourceView());
			_DirectLightingDescriptorTable->SetDescriptor(6, _DirectLightingLightsBuffer.GetView());
			_DirectLightingDescriptorTable->SetDescriptor(7, InContext.GetPointClampSampler());

			{
				ResourceTransition Transitions[] =
				{
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferAlbedo().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferNormals().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView(),	TransitionState::TRANSITION_PIXEL_SHADER_READ),
					ResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),				TransitionState::TRANSITION_PIXEL_SHADER_READ | TransitionState::TRANSITION_DEPTH_STENCIL_READ)
				};
				//ResourceTransitionScope GBufferToShaderReadScope(*DirectLightingCommandList, Transitions, ETERNAL_ARRAYSIZE(Transitions));
				DirectLightingCommandList->Transition(Transitions, ETERNAL_ARRAYSIZE(Transitions));

				DirectLightingCommandList->BeginRenderPass(*_DirectLightingRenderPass);
				DirectLightingCommandList->SetGraphicsPipeline(*_Pipeline);
				DirectLightingCommandList->SetGraphicsDescriptorTable(InContext, *_DirectLightingDescriptorTable);
				DirectLightingCommandList->DrawInstanced(6);
				DirectLightingCommandList->EndRenderPass();
			}

			InRenderer.GetStencilTracker().ReleaseStencil(StencilAllocation::OpaqueStencilBit);
		}

		void DirectLightingPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			OutInputs.InputViews = {
				StaticRenderer->GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView(),
				StaticRenderer->GetGlobalResources().GetGBufferAlbedo().GetShaderResourceView(),
				StaticRenderer->GetGlobalResources().GetGBufferNormals().GetShaderResourceView(),
				StaticRenderer->GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetShaderResourceView()
			};
		}

		void DirectLightingPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_DEPTH_STENCIL_WRITE
			};
		}
	}
}
