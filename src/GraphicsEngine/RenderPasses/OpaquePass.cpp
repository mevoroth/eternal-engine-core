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

		static constexpr bool UseMeshPipeline = false;

		OpaquePass::OpaquePass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: ObjectPass(InContext, InRenderer)
		{
			//vector<string> Defines =
			//{
			//	"OBJECT_NEEDS_NORMAL",		"1",
			//	"OBJECT_NEEDS_TANGENT",		"1",
			//	"OBJECT_NEEDS_BINORMAL",	"1",
			//	"OBJECT_NEEDS_UV",			"1"
			//};

			//ShaderCreateInformation OpaquePSCreateInformation(ShaderType::PS, "OpaquePS", "opaque.ps.hlsl", Defines);
			//Shader* OpaquePS = InContext.GetShader(OpaquePSCreateInformation);

			//vector<RootSignatureParameter> ParametersVSPS =
			//{
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS)
			//};

			//vector<RootSignatureParameter> ParametersMSPS =
			//{
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_SAMPLER,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_PS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS),
			//	RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_MS)
			//};

			//_RootSignature = CreateRootSignature(
			//	InContext,
			//	RootSignatureCreateInformation(
			//		UseMeshPipeline ? ParametersMSPS : ParametersVSPS,
			//		{}, {},
			//		/*InHasInputAssembler=*/ true
			//	)
			//);
			//_OpaqueDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);
			//
			//_OpaqueInputLayout = CreateInputLayout(
			//	InContext,
			//	{
			//		VertexStream<PositionNormalTangentBinormalUVVertex>({
			//			{ Format::FORMAT_RGBA32323232_FLOAT,	VertexDataType::VERTEX_DATA_TYPE_POSITION },
			//			{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_NORMAL },
			//			{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_TANGENT },
			//			{ Format::FORMAT_RGB323232_FLOAT,		VertexDataType::VERTEX_DATA_TYPE_BINORMAL },
			//			{ Format::FORMAT_RG3232_FLOAT,			VertexDataType::VERTEX_DATA_TYPE_UV }
			//		})
			//	}
			//);

			//GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();
			//_OpaqueRenderPass = CreateRenderPass(
			//	InContext,
			//	RenderPassCreateInformation(
			//		InContext.GetMainViewport(),
			//		{
			//			RenderTargetInformation(BlendStateAdditive, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferLuminance().GetRenderTargetDepthStencilView()),
			//			RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferAlbedo().GetRenderTargetDepthStencilView()),
			//			RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferNormals().GetRenderTargetDepthStencilView()),
			//			RenderTargetInformation(BlendStateNone, RenderTargetOperator::Clear_Store, InGlobalResources.GetGBufferRoughnessMetallicSpecular().GetRenderTargetDepthStencilView())
			//		},
			//		InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Clear_Store
			//	)
			//);

			//if (UseMeshPipeline)
			//{
			//	ShaderCreateInformation OpaqueMSCreateInformation(ShaderType::MS, "ObjectMS", "object.ms.hlsl", Defines);
			//	Shader* OpaqueMS = InContext.GetShader(OpaqueMSCreateInformation);
			//	
			//	MeshPipelineCreateInformation OpaquePipelineCreateInformation(
			//		*_RootSignature,
			//		_OpaqueRenderPass,
			//		OpaqueMS, OpaquePS,
			//		DepthStencilTestWriteNone
			//	);
			//	_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			//}
			//else
			//{
			//	ShaderCreateInformation OpaqueVSCreateInformation(ShaderType::VS, "ObjectVS", "object.vs.hlsl", Defines);
			//	Shader* OpaqueVS = InContext.GetShader(OpaqueVSCreateInformation);

			//	GraphicsPipelineCreateInformation OpaquePipelineCreateInformation(
			//		*_RootSignature,
			//		_OpaqueInputLayout,
			//		_OpaqueRenderPass,
			//		OpaqueVS, OpaquePS,
			//		DepthStencilTestWriteNone
			//	);
			//	_Pipeline = CreatePipeline(InContext, OpaquePipelineCreateInformation);
			//}
		}

		OpaquePass::~OpaquePass()
		{
			//DestroyRenderPass(_OpaqueRenderPass);
			//DestroyInputLayout(_OpaqueInputLayout);
			//DestroyDescriptorTable(_OpaqueDescriptorTable);
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

			_RenderInternal<TransitionFunctorType, PerPassFunctorType>(
				InContext,
				InSystem,
				InRenderer,
				TransitionFunctionOpaque,
				PerPassFunctionOpaque
			);
		}

		const string& OpaquePass::_GetPassName() const
		{
			return OpaquePassName;
		}
	}
}
