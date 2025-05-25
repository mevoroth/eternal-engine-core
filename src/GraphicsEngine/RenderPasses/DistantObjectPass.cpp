#include "GraphicsEngine/RenderPasses/DistantObjectPass.hpp"
#include "GraphicsEngine/StencilAllocation.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		constexpr uint32_t DistantObjectPass::DistantObjectInstancesCount;
		const string DistantObjectPass::DistantObjectPassName = "DistantObjectPass";

		DistantObjectPass::DistantObjectPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: ObjectPass(InContext, InRenderer, DistantObjectInstancesCount)
		{
			_ObjectBucket = MaterialType::MATERIAL_TYPE_DISTANT;

			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			vector<string> Defines =
			{
				"OBJECT_HAS_INSTANCES",		"1",
				"OBJECT_NEEDS_NORMAL",		"1"
			};

			ShaderCreateInformation DistantPixelCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "DistantObjectPixel", "distantobject.pixel.hlsl", Defines);

			vector<RootSignatureParameter> ParametersVSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL)
			};

			vector<RootSignatureParameter> ParametersMSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH)
			};

			StencilTest::FaceOperator FrontFace;
			FrontFace.Comparison = ComparisonFunction::COMPARISON_FUNCTION_NEVER;

			StencilTest::FaceOperator BackFace;
			BackFace.Comparison = ComparisonFunction::COMPARISON_FUNCTION_NOT_EQUAL;

			DepthStencil DepthStencilTestNoneReadOpaqueStencil(
				DepthStencilTestNone.GetDepthTest(),
				StencilTest(
					FrontFace,
					BackFace,
					StencilAllocation::OpaqueStencil,
					0x0,
					StencilAllocation::OpaqueStencil
				)
			);

			ObjectPassCreateInformation ObjectPassInformation(
				Defines,
				UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Load_Store
				),
				InContext.GetShader(DistantPixelCreateInformation),
				RasterizerCullFront,
				DepthStencilTestNoneReadOpaqueStencil
			);
			_InitializeObjectPass(InContext, ObjectPassInformation);
		}

		void DistantObjectPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			TransitionFunctionType TransitionFunctorOpaque(
				[](_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer) -> void
				{
					(void)InRenderer;
					ResourceTransition Transitions[] =
					{
						ResourceTransition(InRenderer.GetGlobalResources().GetSky().GetShaderResourceView(),							TransitionState::TRANSITION_PIXEL_SHADER_READ),
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView(),		TransitionState::TRANSITION_RENDER_TARGET),
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),	TransitionState::TRANSITION_DEPTH_STENCIL_WRITE)
					};
					InObjectCommandList->Transition(Transitions, ETERNAL_ARRAYSIZE(Transitions));
				}
			);

			PerPassFunctionType PerPassFunctorOpaque(
				[this](_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer) -> void
				{
					_ObjectDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetViewConstantBufferView());
					_ObjectDescriptorTable->SetDescriptor(5, InContext.GetBilinearWrapSampler());
				}
			);

			PerDrawFunctionType PerDrawFunctorOpaque(
				[this](_In_ Material* InPerDrawMaterial, _In_ Renderer& InRenderer)
				{
					(void)InPerDrawMaterial;
					_ObjectDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetSky().GetShaderResourceView());
				}
			);

			IsVisibleFunctionType IsVisibleFunctorOpaque(
				[&InSystem](_In_ uint32_t InKey) -> bool
				{
					(void)InKey;
					(void)InSystem;
					return true;
				}
			);

			_RenderInternal<TransitionFunctionType, PerPassFunctionType, PerDrawFunctionType>(
				InContext,
				InSystem,
				InRenderer,
				TransitionFunctorOpaque,
				PerPassFunctorOpaque,
				PerDrawFunctorOpaque,
				IsVisibleFunctorOpaque
			);
		}

		void DistantObjectPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			OutInputs.InputViews.push_back(StaticRenderer->GetGlobalResources().GetSky().GetShaderResourceView());
		}

		void DistantObjectPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_RENDER_TARGET
			};
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferDepthStencil().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_DEPTH_STENCIL_WRITE
			};
		}

		const string& DistantObjectPass::_GetPassName() const
		{
			return DistantObjectPassName;
		}
	}
}
