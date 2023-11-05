#include "GraphicsEngine/RenderPasses/RayTracedReflectionsPass.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		using namespace Eternal::Types;

		const string RayTracedReflectionsPass::RayTracedReflectionsPassName = "RayTracedReflectionsPass";

		RayTracedReflectionsPass::RayTracedReflectionsPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
		{
			Shader* RayTracedReflectionsRayGeneration	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_RAY_GENERATION, "RayTracedReflectionsRayGeneration", "RayTracing/raytracedreflections.raygeneration.hlsl"));
			Shader* RayTracedReflectionsClosestHit		= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_CLOSEST_HIT, "RayTracedReflectionsClosestHit", "RayTracing/raytracedreflections.closesthit.hlsl"));
			Shader* RayTracedReflectionsMiss			= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_RAYTRACING_MISS, "RayTracedReflectionsMiss", "RayTracing/raytracedreflections.miss.hlsl"));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RAYTRACING_ACCELERATION_STRUCTURE,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,							RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_TEXTURE,							RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_RW_TEXTURE,							RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,					RootSignatureAccess::ROOT_SIGNATURE_ACCESS_RAYTRACING)
					}
				)
			);

			_RayTracedReflectionsDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);

			_Pipeline = CreatePipeline(
				InContext,
				RayTracingPipelineCreateInformation(
					*_RootSignature,
					RayTracedReflectionsRayGeneration,
					RayTracedReflectionsClosestHit,
					RayTracedReflectionsMiss
				)
			);

			_RayTracedReflectionsShaderTable = CreateShaderTable(InContext.GetDevice(), *_Pipeline);
		}

		void RayTracedReflectionsPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			SystemFrame& CurrentRenderFrame = InSystem.GetRenderFrame();

			CommandListScope RayTracedReflectionsCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "RayTracedReflectionsPass");

			_RayTracedReflectionsDescriptorTable->SetDescriptor(0, CurrentRenderFrame.MeshCollectionsAccelerationStructure->GetView());
			_RayTracedReflectionsDescriptorTable->SetDescriptor(1, InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetShaderResourceView());
			_RayTracedReflectionsDescriptorTable->SetDescriptor(2, InRenderer.GetGlobalResources().GetGBufferNormals().GetShaderResourceView());
			_RayTracedReflectionsDescriptorTable->SetDescriptor(3, InRenderer.GetGlobalResources().GetGBufferLuminance().GetUnorderedAccessView());
			_RayTracedReflectionsDescriptorTable->SetDescriptor(4, InRenderer.GetGlobalResources().GetViewConstantBufferView());

			RayTracedReflectionsCommandList->SetRayTracingPipeline(*_Pipeline);
			RayTracedReflectionsCommandList->SetRayTracingDescriptorTable(InContext, *_RayTracedReflectionsDescriptorTable);
			RayTracedReflectionsCommandList->DispatchRays(*_RayTracedReflectionsShaderTable, InContext.GetOutputDevice().GetWidth(), InContext.GetOutputDevice().GetHeight());
		}
	}
}
