#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		const string OpaquePass::OpaquePassName = "OpaquePass";

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			vector<string> Defines =
			{
				"OBJECT_NEEDS_NORMAL",		"1",
				"OBJECT_NEEDS_TANGENT",		"1",
				"OBJECT_NEEDS_BINORMAL",	"1",
				"OBJECT_NEEDS_UV",			"1"
			};

			ShaderCreateInformation OpaquePSCreateInformation(ShaderType::PS, "OpaquePS", "opaque.ps.hlsl", Defines);

			vector<RootSignatureParameter> ParametersVSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
			};

			vector<RootSignatureParameter> ParametersMSPS =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS)
			};

			ObjectPassCreateInformation ObjectPassInformation =
			{
				Defines,
				UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateAdditive, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferAlbedo().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferNormals().GetRenderTargetDepthStencilView()),
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Clear_Store
				),
				InContext.GetShader(OpaquePSCreateInformation)
			};
			_InitializeObjectPass(InContext, ObjectPassInformation);
		}

		void OpaquePass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			TransitionFunctorType TransitionFunctionOpaque(
				[](_In_ CommandList* InObjectCommandList, _In_ Renderer& InRenderer) -> void
				{
					ResourceTransition Transitions[] =
					{
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferAlbedo().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_RENDER_TARGET),
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferNormals().GetRenderTargetDepthStencilView(),					TransitionState::TRANSITION_RENDER_TARGET),
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView(),	TransitionState::TRANSITION_RENDER_TARGET),
						ResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),				TransitionState::TRANSITION_DEPTH_STENCIL_WRITE)
					};
					InObjectCommandList->Transition(Transitions, ETERNAL_ARRAYSIZE(Transitions));
				}
			);

			PerPassFunctorType PerPassFunctionOpaque(
				[this](_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer) -> void
				{
					_ObjectDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetViewConstantBufferView());
					_ObjectDescriptorTable->SetDescriptor(5, InContext.GetBilinearClampSampler());
				}
			);

			PerDrawFunctorType PerDrawFunctionOpaque(
				[this](_In_ Material* InPerDrawMaterial)
				{
					InPerDrawMaterial->CommitMaterial(*_ObjectDescriptorTable);
				}
			);

			_RenderInternal<TransitionFunctorType, PerPassFunctorType, PerDrawFunctorType>(
				InContext,
				InSystem,
				InRenderer,
				TransitionFunctionOpaque,
				PerPassFunctionOpaque,
				PerDrawFunctionOpaque
			);
		}

		const string& OpaquePass::_GetPassName() const
		{
			return OpaquePassName;
		}
	}
}
