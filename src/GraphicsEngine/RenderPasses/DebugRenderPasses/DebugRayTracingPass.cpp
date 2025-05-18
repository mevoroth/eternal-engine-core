#include "GraphicsEngine/RenderPasses/DebugRenderPasses/DebugRayTracingPass.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::Types;

		const string DebugRayTracingPass::DebugRayTracingPassName = "DebugRayTracingPass";

		DebugRayTracingPass::DebugRayTracingPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _DebugRayTracingRayGenerationConstantBuffer(InContext, "DebugRayTracingRayGenerationBuffer")
		{
			_IsPassEnabled = false;
			return;

			Shader* DebugRayTracingRayGeneration	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_RAY_GENERATION, "DebugRayTracingRayGeneration", "RayTracing/Debug/debugraytracing.raygeneration.hlsl"));
			Shader* DebugRayTracingClosestHit		= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_CLOSEST_HIT, "DebugRayTracingClosestHit", "RayTracing/Debug/debugraytracing.closesthit.hlsl"));
			Shader* DebugRayTracingMiss				= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_MISS, "DebugRayTracingMiss", "RayTracing/Debug/debugraytracing.miss.hlsl"));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RAYTRACING_ACCELERATION_STRUCTURE,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,							RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,					RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING)
					}
				)
			);

			_DebugRayTracingDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				RayTracingPipelineCreateInformation(
					_RootSignature,
					DebugRayTracingRayGeneration,
					DebugRayTracingClosestHit,
					DebugRayTracingMiss,
					nullptr
				)
			);

			_DebugRayTracingShaderTable = CreateShaderTable(InContext.GetDevice(), *_Pipeline);
		}

		void DebugRayTracingPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			using namespace Eternal::Core;

			SystemFrame& CurrentRenderFrame = InSystem.GetRenderFrame();

			CommandListScope DebugRayTracingCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "DebugRayTracingPass");

			{
				float AspectRatio = static_cast<float>(InContext.GetMainViewport().GetHeight()) / static_cast<float>(InContext.GetMainViewport().GetWidth());
				float Border = 0.1f;

				MapRange RayGenerationBufferMapRange(sizeof(RayGenerationConstants));
				MapScope<RayGenerationConstants> RayGenerationBufferMapScope(*_DebugRayTracingRayGenerationConstantBuffer.ResourceBuffer, RayGenerationBufferMapRange);
				RayGenerationBufferMapScope->Viewport	= { -1.0f, -1.0f, 1.0f, 1.0f };
				RayGenerationBufferMapScope->Stencil	= {
					-1.0f + Border / AspectRatio, -1.0f + Border,
					 1.0f - Border / AspectRatio,  1.0f - Border
				};
			}

			_DebugRayTracingDescriptorTable->SetDescriptor(0, CurrentRenderFrame.MeshCollectionsAccelerationStructure->GetView());
			_DebugRayTracingDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());
			_DebugRayTracingDescriptorTable->SetDescriptor(2, _DebugRayTracingRayGenerationConstantBuffer.GetView());

			DebugRayTracingCommandList->SetRayTracingPipeline(*_Pipeline);
			DebugRayTracingCommandList->SetRayTracingDescriptorTable(InContext, *_DebugRayTracingDescriptorTable);
			DebugRayTracingCommandList->DispatchRays(*_DebugRayTracingShaderTable, InContext.GetOutputDevice().GetWidth(), InContext.GetOutputDevice().GetHeight());
		}

		void DebugRayTracingPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			(void)OutInputs;
		}

		void DebugRayTracingPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView(),
				TransitionState::TRANSITION_SHADER_WRITE
			};
		}
	}
}
