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

			ShaderCreateInformation TonemappingCSCreateInformation(
				ShaderType::CS, "TonemappingCS", "tonemapping.cs.hlsl",
				{
					"THREAD_GROUP_COUNT_X", ThreadGroupCountXString,
					"THREAD_GROUP_COUNT_Y", ThreadGroupCountYString,
					"THREAD_GROUP_COUNT_Z", ThreadGroupCountZString
				}
			);
			Shader* TonemappingCS = InContext.GetShader(TonemappingCSCreateInformation);

			_TonemappingRootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,			RootSignatureAccess::ROOT_SIGNATURE_ACCESS_CS)
					}
				)
			);

			_TonemappingDescriptorTable = _TonemappingRootSignature->CreateRootDescriptorTable(InContext);

			_TonemappingPipeline = CreatePipeline(
				InContext,
				ComputePipelineCreateInformation(
					*_TonemappingRootSignature,
					TonemappingCS
				)
			);
		}

		TonemappingPass::~TonemappingPass()
		{
			DestroyPipeline(_TonemappingPipeline);
			DestroyDescriptorTable(_TonemappingDescriptorTable);
			DestroyRootSignature(_TonemappingRootSignature);
		}

		void TonemappingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			CommandList* TonemappingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHIC, "Tonemapping");

			TonemappingCommandList->Begin(InContext);
			{
				ResourceTransition LuminanceToUnorderedAccess(InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(), TransitionState::TRANSITION_SHADER_WRITE);
				ResourceTransitionScope TonemappingTransitionScope(*TonemappingCommandList, &LuminanceToUnorderedAccess, 1);

				_TonemappingDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
				_TonemappingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());

				TonemappingCommandList->SetComputePipeline(*_TonemappingPipeline);
				TonemappingCommandList->SetComputeDescriptorTable(InContext, *_TonemappingDescriptorTable);
				TonemappingCommandList->Dispatch(
					Math::DivideRoundUp(InContext.GetWindow().GetWidth(), ThreadGroupCountX),
					Math::DivideRoundUp(InContext.GetWindow().GetHeight(), ThreadGroupCountY),
					1
				);
			}
			TonemappingCommandList->End();
		}
	}
}
