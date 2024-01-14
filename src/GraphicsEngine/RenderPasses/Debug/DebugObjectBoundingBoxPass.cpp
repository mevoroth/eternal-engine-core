#include "GraphicsEngine/RenderPasses/Debug/DebugObjectBoundingBoxPass.hpp"
#include "Core/System.hpp"
#include "Mesh/Mesh.hpp"
#include "Components/TransformComponent.hpp"
#include "ShadersReflection/HLSLCubePerInstanceInformation.hpp"

namespace Eternal
{
	namespace GraphicsEngine
	{
		const string DebugObjectBoundingBoxPass::DebugObjectBoundingBoxPassName = "DebugObjectBoundingBoxPass";

		DebugObjectBoundingBoxPass::DebugObjectBoundingBoxPass(_In_ GraphicsContext& InContext, _In_ Renderer& InRenderer)
			: _DebugObjectBoundingBoxPerInstanceBuffer(InContext, "DebugObjectBoundingBoxBoxPerInstanceBuffer", 4096)
		{
			_IsPassEnabled = false;

			using namespace Eternal::HLSL;

			GlobalResources& InGlobalResources = InRenderer.GetGlobalResources();

			Shader* BoundingBoxWireframeVertex	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_VERTEX, "BoundingBoxWireframeVertex", "cube.vertex.hlsl", { "PRIMITIVE_INDICES", "Lines"}));
			Shader* BoundingBoxWireframePixel	= InContext.GetShader(ShaderCreateInformation(ShaderType::SHADER_TYPE_PIXEL, "BoundingBoxWireframePixel", "fixedcolor.pixel.hlsl"));

			_RootSignature = CreateRootSignature(
				InContext,
				RootSignatureCreateInformation(
					{
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_CONSTANT_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX),
						RootSignatureParameter(RootSignatureParameterType::ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER,	RootSignatureAccess::ROOT_SIGNATURE_ACCESS_VERTEX)
					}
				)
			);

			_DebugObjectBoundingBoxInputLayout = CreateInputLayout(InContext);
			_DebugObjectBoundingBoxRenderPass = CreateRenderPass(
				InContext,
				RenderPassCreateInformation(
					InContext.GetMainViewport(),
					{
						RenderTargetInformation(BlendStateNone, RenderTargetOperator::Load_Store, InRenderer.GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView())
					},
					InGlobalResources.GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), RenderTargetOperator::Load_Store
				)
			);

			_Pipeline = CreatePipeline(
				InContext,
				GraphicsPipelineCreateInformation(
					_RootSignature,
					_DebugObjectBoundingBoxInputLayout,
					_DebugObjectBoundingBoxRenderPass,
					BoundingBoxWireframeVertex,
					BoundingBoxWireframePixel,
					DepthStencilTestWriteNone,
					RasterizerLine,
					PrimitiveTopology::PRIMITIVE_TOPOLOGY_LINE_LIST
				)
			);

			_DebugObjectBoundingBoxDescriptorTable = _RootSignature->CreateRootDescriptorTable(InContext);
		}

		void DebugObjectBoundingBoxPass::Render(_In_ GraphicsContext& InContext, _In_ System& InSystem, _In_ Renderer& InRenderer)
		{
			SystemFrame& CurrentFrame = InSystem.GetRenderFrame();
			const vector<ObjectsList<MeshCollection>::InstancedObjects>& MeshCollections = CurrentFrame.MeshCollections;

			if (MeshCollections.size() == 0 || CurrentFrame.MeshCollectionsVisibility.GetSize() == 0)
				return;

			CommandListScope DebugObjectBoundingBoxCommandList = InContext.CreateNewCommandList(CommandType::COMMAND_TYPE_GRAPHICS, "DebugObjectBoundingBoxPass");

			MapRange DebugObjectBoundingBoxPerInstanceBufferMapRange(sizeof(CubePerInstanceInformation) * 4096);
			MapScope<CubePerInstanceInformation> DebugObjectBoundingBoxPerInstanceBufferMapScope(*_DebugObjectBoundingBoxPerInstanceBuffer.ResourceBuffer, DebugObjectBoundingBoxPerInstanceBufferMapRange);

			ResourceTransition DebugObjectBoundingBoxResourceTransition(InRenderer.GetGlobalResources().GetGBufferDepthStencil().GetRenderTargetDepthStencilView(), TransitionState::TRANSITION_DEPTH_STENCIL_WRITE);
			DebugObjectBoundingBoxCommandList->Transition(DebugObjectBoundingBoxResourceTransition);

			DebugObjectBoundingBoxCommandList->BeginRenderPass(*_DebugObjectBoundingBoxRenderPass);
			DebugObjectBoundingBoxCommandList->SetGraphicsPipeline(*_Pipeline);

			_DebugObjectBoundingBoxDescriptorTable->SetDescriptor(0, InRenderer.GetGlobalResources().GetViewConstantBufferView());
			_DebugObjectBoundingBoxDescriptorTable->SetDescriptor(1, _DebugObjectBoundingBoxPerInstanceBuffer.GetView());
			DebugObjectBoundingBoxCommandList->SetGraphicsDescriptorTable(InContext, *_DebugObjectBoundingBoxDescriptorTable);

			uint32_t VisibilityCount				= 0;
			uint32_t BoundingBoxPerInstanceCount	= 0;

			for (uint32_t CollectionIndex = 0; CollectionIndex < MeshCollections.size(); ++CollectionIndex)
			{
				const ObjectsList<MeshCollection>::InstancedObjects& CurrentInstancedObject = MeshCollections[CollectionIndex];
				Mesh* Meshes = CurrentInstancedObject.Object->Meshes;

				uint32_t InstancesCount = static_cast<uint32_t>(CurrentInstancedObject.Instances.size());
				for (uint32_t InstanceIndex = 0; InstanceIndex < InstancesCount; ++InstanceIndex)
				{
					Matrix4x4 LocalToWorld = CurrentInstancedObject.Instances[InstanceIndex]->GetTransform().GetLocalToWorld();

					GPUMesh& CurrentGPUMesh = Meshes->GetGPUMesh();
					vector<AxisAlignedBoundingBox>& CurrentBoundingBoxes = CurrentGPUMesh.BoundingBoxes;

					for (uint32_t BoundingBoxIndex = 0; BoundingBoxIndex < CurrentBoundingBoxes.size(); ++BoundingBoxIndex)
					{
						if (CurrentFrame.MeshCollectionsVisibility.IsSet(VisibilityCount))
						{
							AxisAlignedBoundingBox CurrentBoundingBox = CurrentBoundingBoxes[BoundingBoxIndex];

							DebugObjectBoundingBoxPerInstanceBufferMapScope[BoundingBoxPerInstanceCount].InstanceWorldToWorld	= LocalToWorld;
							DebugObjectBoundingBoxPerInstanceBufferMapScope[BoundingBoxPerInstanceCount].CubeOrigin				= CurrentBoundingBox.GetOrigin();
							DebugObjectBoundingBoxPerInstanceBufferMapScope[BoundingBoxPerInstanceCount].CubeExtent				= CurrentBoundingBox.GetExtent();

							++BoundingBoxPerInstanceCount;
						}

						++VisibilityCount;
					}
				}
			}

			DebugObjectBoundingBoxCommandList->DrawInstanced(24, BoundingBoxPerInstanceCount);
			DebugObjectBoundingBoxCommandList->EndRenderPass();
		}

		void DebugObjectBoundingBoxPass::GetInputs(_Out_ FrameGraphPassInputs& OutInputs) const
		{
			(void)OutInputs;
		}

		void DebugObjectBoundingBoxPass::GetOutputs(_Out_ FrameGraphPassOutputs& OutOutputs) const
		{
			OutOutputs.OutputViews[&StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetTexture()] = {
				StaticRenderer->GetGlobalResources().GetGBufferLuminance().GetRenderTargetDepthStencilView(),
				TransitionState::TRANSITION_DEPTH_STENCIL_WRITE
			};
		}
	}
}
