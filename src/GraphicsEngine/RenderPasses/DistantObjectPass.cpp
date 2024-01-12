#include "GraphicsEngine/RenderPasses/DistantObjectPass.hpp"

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

			ObjectPassCreateInformation ObjectPassInformation(
				Defines,
				UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateAdditive, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::NoLoad_NoStore
				),
				InContext.GetShader(DistantPixelCreateInformation),
				Rasterizer(CullMode::CULL_MODE_FRONT)
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

		const string& DistantObjectPass::_GetPassName() const
		{
			return DistantObjectPassName;
		}
	}
}
