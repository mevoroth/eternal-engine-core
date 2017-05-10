#include "Task/NextGenGraphics/RenderObjectsTask.hpp"

#include <vector>
#include "Core/StateSharedData.hpp"
#include "Core/GraphicGameObject.hpp"
#include "Core/MeshComponent.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/RootSignature.hpp"
#include "Graphics/RootSignatureFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/InputLayoutFactory.hpp"
#include "Graphics/RenderPassFactory.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/PipelineFactory.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/Viewports.hpp"
#include "GraphicData/GraphicBuffers.hpp"
#include "GraphicData/Shaders.hpp"
#include "GraphicData/StaticSamplers.hpp"
#include "Mesh/Mesh.hpp"

#include "Graphics/DescriptorHeapFactory.hpp"
#include "Graphics/RootSignature.hpp"
//#include "Graphics"

using namespace Eternal::Task;
using namespace Eternal::Components;
using namespace Eternal::GraphicData;

namespace Eternal
{
	namespace Task
	{
		class RenderObjectsTaskData
		{
		public:
			RenderObjectsTaskData(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
				: _SharedData(SharedData)
			{
				//_RootSignature = CreateRootSignature(DeviceObj, )
				InputLayout::VertexDataType DataTypes[] = {
					InputLayout::POSITION_T,
					InputLayout::NORMAL_T,
					InputLayout::TANGENT_T,
					InputLayout::BINORMAL_T,
					InputLayout::UV_T
				};

				vector<View*> RenderTargets;
				RenderTargets.resize(GRAPHIC_BUFFER_NO_DEPTH_COUNT);
				for (uint32_t GraphicBufferIndex = 0; GraphicBufferIndex < GRAPHIC_BUFFER_NO_DEPTH_COUNT; ++GraphicBufferIndex)
				{
					RenderTargets[GraphicBufferIndex] = Resources->GetGraphicBuffers()->Get((GraphicBufferKey)GraphicBufferIndex);
				}

				Sampler* BilinearSampler = Resources->GetStaticSamplers()->Get(SAMPLER_BILINEAR);

				DescriptorHeap* FrameConstants		= CreateDescriptorHeap(DeviceObj, 0, ROOT_SIGNATURE_PARAMETER_DYNAMIC_BUFFER, 2, (RootSignatureAccess)(ROOT_SIGNATURE_IA | ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
				DescriptorHeap* ObjectInstanceData	= CreateDescriptorHeap(DeviceObj, 0, ROOT_SIGNATURE_PARAMETER_STRUCTURED_BUFFER, 1, (RootSignatureAccess)(ROOT_SIGNATURE_IA | ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
				DescriptorHeap* ObjectTextureSet	= CreateDescriptorHeap(DeviceObj, 1, ROOT_SIGNATURE_PARAMETER_TEXTURE, 4, (RootSignatureAccess)(ROOT_SIGNATURE_IA | ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));

				DescriptorHeap* DescriptorHeaps[] =
				{
					FrameConstants,
					ObjectInstanceData,
					ObjectTextureSet
				};

				_Viewport		= Resources->GetViewports()->Get(VIEWPORT_FULLSCREEN);
				_InputLayout	= CreateInputLayout(DeviceObj, DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_RootSignature	= CreateRootSignature(DeviceObj, &BilinearSampler, 1, DescriptorHeaps, ETERNAL_ARRAYSIZE(DescriptorHeaps), (RootSignatureAccess)(ROOT_SIGNATURE_IA | ROOT_SIGNATURE_VS | ROOT_SIGNATURE_PS));
				_RenderPass		= CreateRenderPass(DeviceObj, RenderTargets, Resources->GetGraphicBuffers()->Get(GRAPHIC_BUFFER_DEPTH));
				_VS				= Resources->GetShaders()->Get(SHADER_OPAQUE_VS);
				_PS				= Resources->GetShaders()->Get(SHADER_OPAQUE_PS);
				_Pipeline		= CreatePipeline(DeviceObj, *_RootSignature, *_InputLayout, *_RenderPass, *GetVS(), *GetPS(), *GetViewport());
			}

			Viewport* GetViewport() { return _Viewport; }
			Pipeline* GetPipeline() { return _Pipeline; }
			RenderPass* GetRenderPass() { return _RenderPass; }
			RenderTarget* GetRenderTargets() { return _RenderTargets; }
			StateSharedData* GetSharedData() { return _SharedData; }
			Shader* GetVS() { return _VS; }
			Shader* GetPS() { return _PS; }

		private:
			StateSharedData*	_SharedData		= nullptr;
			RootSignature*		_RootSignature	= nullptr;
			Viewport*			_Viewport		= nullptr;
			Pipeline*			_Pipeline		= nullptr;
			RenderPass*			_RenderPass		= nullptr;
			RenderTarget*		_RenderTargets	= nullptr;
			InputLayout*		_InputLayout	= nullptr;
			Shader*				_VS				= nullptr;
			Shader*				_PS				= nullptr;
		};
	}
}

RenderObjectsTask::RenderObjectsTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: GraphicTask(DeviceObj, Resources)
	, _RenderObjectsTaskData(new RenderObjectsTaskData(DeviceObj, Resources, SharedData))
{
}

void RenderObjectsTask::Render(_Inout_ CommandList* CommandListObj)
{
	vector<GraphicGameObject*>& GameObjects = *_RenderObjectsTaskData->GetSharedData()->GraphicGameObjects;

	for (uint32_t GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex)
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		Resource* VertexBufferObj = CurrentMesh->GetVerticesBuffer();

		CommandListObj->SetIndicesBuffer(CurrentMesh->GetIndicesBuffer());
		CommandListObj->SetVerticesBuffers(0, 1, &VertexBufferObj);
		//CommandListObj->BindPipelineInput()
		CommandListObj->DrawIndexed(CurrentMesh->GetIndicesCount(), 0, 0);
	}
}

Viewport* RenderObjectsTask::GetViewport()
{
	return _RenderObjectsTaskData->GetViewport();
}

Pipeline* RenderObjectsTask::GetPipeline()
{
	return _RenderObjectsTaskData->GetPipeline();
}

RenderPass* RenderObjectsTask::GetRenderPass()
{
	return _RenderObjectsTaskData->GetRenderPass();
}

RenderTarget* RenderObjectsTask::GetRenderTargets()
{
	return _RenderObjectsTaskData->GetRenderTargets();
}
