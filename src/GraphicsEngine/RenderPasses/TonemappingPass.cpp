#include "GraphicsEngine/RenderPasses/TonemappingPass.hpp"
#include "Math/Math.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		static constexpr int32_t ThreadGroupCountX = 32;
		static constexpr int32_t ThreadGroupCountY = 32;
		static constexpr int32_t ThreadGroupCountZ = 1;

		TonemappingPass::TonemappingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			char ThreadGroupCountXString[4];
			char ThreadGroupCountYString[4];
			char ThreadGroupCountZString[4];

			sprintf_s(ThreadGroupCountXString, "%d", ThreadGroupCountX);
			sprintf_s(ThreadGroupCountYString, "%d", ThreadGroupCountY);
			sprintf_s(ThreadGroupCountZString, "%d", ThreadGroupCountZ);

			ShaderCreateInformation TonemappingComputeCreateInformation(
				ShaderType::SHADER_TYPE_COMPUTE, "TonemappingCompute", "tonemapping.compute.hlsl",
				{
					"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
					"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
					"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString
				}
			);
			Shader* TonemappingCompute = InContext.GetShader(TonemappingComputeCreateInformation);

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS)
					}
				)
			);

			_TonemappingDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					*_RootSignature,
					TonemappingCompute
				)
			);
		}

		TonemappingPass::~TonemappingPass()
		{
			DestroyDescriptorTable(_TonemappingDescriptorTable);
		}

		void TonemappingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandListScope TonemappingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "Tonemapping");

			_TonemappingDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_TonemappingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());

			{
				ResourceTransition LuminanceToUnorderedAccess(InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(), TransitionState::TRANSITION_SHADER_WRITE);
				ResourceTransitionScope TonemappingTransitionScope(*TonemappingCommandList, &LuminanceToUnorderedAccess, 1);

				TonemappingCommandList->SetComputePipeline(*_Pipeline);
				TonemappingCommandList->SetComputeDescriptorTable(InContext, *_TonemappingDescriptorTable);
				TonemappingCommandList->Dispatch(
					Math::DivideRoundUp(InContext.GetWindow().GetWidth(), ThreadGroupCountX),
					Math::DivideRoundUp(InContext.GetWindow().GetHeight(), ThreadGroupCountY),
					1
				);
			}
		}
	}
}
