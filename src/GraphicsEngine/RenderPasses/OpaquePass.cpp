#include "GraphicsEngine/RenderPasses/OpaquePass.hpp"
#include "GraphicsEngine/StencilAllocation.hpp"
#include "Core/System.hpp"
#include "GraphicData/MeshVertexFormat.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::GraphicData;

		constexpr uint32_t OpaquePass::OpaquePassInstancesCount;
		const string OpaquePass::OpaquePassName = "OpaquePass";

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: ObjectPass(InContext, InRenderer, OpaquePassInstancesCount)
		{
			_ObjectBucket = MaterialType::MATERIAL_TYPE_OPAQUE;

			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			vector<string> Defines =
			{
				"OBJECT_NEEDS_NORMAL",		"1",
				"OBJECT_NEEDS_TANGENT",		"1",
				"OBJECT_NEEDS_BINORMAL",	"1",
				"OBJECT_NEEDS_UV",			"1"
			};

			ShaderCreateInformation OpaquePixelCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "OpaquePixel", "opaque.pixel.hlsl", Defines);

			vector<RootSignatureParameter> ParametersVertexPixel =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL)
			};

			vector<RootSignatureParameter> ParametersMeshPixel =
			{
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PIXEL),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH),
				RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MESH)
			};

			StencilTest::FaceOperator FaceStencil;
			FaceStencil.Pass = StencilTest::StencilOperator::STENCIL_OPERATOR_REPLACE;

			DepthStencil DepthStencilTestWriteWriteOpaqueStencil(
				DepthStencilTestWriteNone.GetDepthTest(),
				StencilTest(
					FaceStencil,
					FaceStencil,
					0x0,
					StencilAllocation::OpaqueStencil,
					StencilAllocation::OpaqueStencil
				)
			);

			ObjectPassCreateInformation ObjectPassInformation =
			{
				Defines,
				UseMeshPipeline ? ParametersMeshPixel : ParametersVertexPixel,
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
				InContext.GetShader(OpaquePixelCreateInformation),
				RasterizerDefault,
				DepthStencilTestWriteWriteOpaqueStencil
			};
			_InitializeObjectPass(InContext, ObjectPassInformation);
		}

		void OpaquePass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			TransitionFunctionType TransitionFunctorOpaque(
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

			PerPassFunctionType PerPassFunctorOpaque(
				[this](_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer) -> void
				{
					_ObjectDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetViewConstantBufferView());
					_ObjectDescriptorTable->SetDescriptor(7, InContext.GetBilinearWrapSampler());
				}
			);

			PerDrawFunctionType PerDrawFunctorOpaque(
				[this](_In_ Material* InPerDrawMaterial, _In_ Renderer& InRenderer)
				{
					(void)InRenderer;
					InPerDrawMaterial->CommitMaterial(*_ObjectDescriptorTable);
				}
			);

			IsVisibleFunctionType IsVisibleFunctorOpaque(
				[&InSystem](_In_ uint32_t InKey) -> bool
				{
					DynamicBitField<>& MeshCollectionsVisibility = InSystem.GetRenderFrame().MeshCollectionsVisibility;
					return MeshCollectionsVisibility.IsSet(InKey);
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

		void OpaquePass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			(void)OutInputs;
		}

		void OpaquePass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			GlobalResources& Resources = StaticRenderer->GetGlobalResources();

			OutOutputs.OutputViews[&Resources.GetGBufferLuminance().GetTexture()] = {
				Resources.GetGBufferLuminance().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_RENDER_TARGET
			};
			OutOutputs.OutputViews[&Resources.GetGBufferNormals().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferNormals().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_RENDER_TARGET
			};
			OutOutputs.OutputViews[&Resources.GetGBufferRoughnessMetallicSpecular().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_RENDER_TARGET
			};
			OutOutputs.OutputViews[&Resources.GetGBufferDepthStencil().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_DEPTH_STENCIL_WRITE
			};
		}

		const string& OpaquePass::_GetPassName() const
		{
			return OpaquePassName;
		}
	}
}
