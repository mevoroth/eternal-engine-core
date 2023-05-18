#include "GraphicsEngine/RenderPasses/DepthOnlyPass.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		const string DepthOnlyPass::DepthOnlyPassName = "DepthOnlyPass";

		DepthOnlyPass::DepthOnlyPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer, _In_ const Viewport& InDepthOnlyViewport, _In_ View* InDepthStencilView, _In_ View* InDepthOnlyViewConstantBufferView)
			: ObjectPass(InContext, InRenderer)
			, _DepthOnlyViewport(InDepthOnlyViewport)
			, _DepthStencilView(InDepthStencilView)
			, _DepthOnlyViewConstantBufferView(InDepthOnlyViewConstantBufferView)
		{
			vector<string> Defines;

			vector<RootSignatureParameter> ParametersVSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS)
			};

			vector<RootSignatureParameter> ParametersMSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS)
			};

			ObjectPassCreateInformation ObjectPassInformation =
			{
				Defines,
				UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
				RenderPassCreateInformation(
					InDepthOnlyViewport,
					_DepthStencilView, RenderTargetOperator::Clear_Store
				)
			};
			_InitializeObjectPass(InContext, ObjectPassInformation);
		}

		void DepthOnlyPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			TransitionFunctionType TransitionFunctorDepthOnly(
				[this](_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer) -> void
				{
					ResourceTransition Transitions[] =
					{
						ResourceTransition(_DepthStencilView, TransitionState::TRANSITION_DEPTH_STENCIL_WRITE)
					};
					InObjectCommandList->Transition(Transitions, ETERNAL_ARRAYSIZE(Transitions));
				}
			);

			PerPassFunctionType PerPassFunctorDepthOnly(
				[this](_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer) -> void
				{
					_ObjectDescriptorTable->SetDescriptor(2, _DepthOnlyViewConstantBufferView);
				}
			);

			PerDrawFunctionType PerDrawFunctorDepthOnly(
				[](_In_ Material* InPerDrawMaterial) {}
			);

			IsVisibleFunctionType IsVisibleFunctorDepthonly(
				[](_In_ uint32_t InKey) -> bool { return false; }
			);

			_BeginRender(InSystem, InRenderer);
			_RenderInternal<TransitionFunctionType, PerPassFunctionType, PerDrawFunctionType, IsVisibleFunctionType>(
				InContext,
				InSystem,
				InRenderer,
				TransitionFunctorDepthOnly,
				PerPassFunctorDepthOnly,
				PerDrawFunctorDepthOnly,
				IsVisibleFunctorDepthonly
			);
		}

		const string& DepthOnlyPass::_GetPassName() const
		{
			return DepthOnlyPassName;
		}
	}
}
