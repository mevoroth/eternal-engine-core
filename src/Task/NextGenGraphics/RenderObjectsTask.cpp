#include "Task/NextGenGraphics/RenderObjectsTask.hpp"

#include "Core/StateSharedData.hpp"
#include "Core/GraphicGameObject.hpp"
#include "Core/MeshComponent.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/RootSignature.hpp"
#include "Graphics/RootSignatureFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/InputLayoutFactory.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/Viewports.hpp"
#include "Mesh/Mesh.hpp"

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

				_Viewport		= Resources->GetViewports()->Get(FULLSCREEN);
				_InputLayout	= CreateInputLayout(DeviceObj, DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_RootSignature	= CreateRootSignature(DeviceObj);
				//_Pipeline		= CreatePipeline(DeviceObj, *_RootSignature, *_InputLayout, *(RenderPass*)nullptr, *(Shader*)nullptr);
			}

			Viewport* GetViewport() { return _Viewport; }
			Pipeline* GetPipeline() { return _Pipeline; }
			RenderPass* GetRenderPass() { return _RenderPass; }
			RenderTarget* GetRenderTargets() { return _RenderTargets; }
			StateSharedData* GetSharedData() { return _SharedData; }

		private:
			StateSharedData*	_SharedData		= nullptr;
			RootSignature*		_RootSignature	= nullptr;
			Viewport*			_Viewport		= nullptr;
			Pipeline*			_Pipeline		= nullptr;
			RenderPass*			_RenderPass		= nullptr;
			RenderTarget*		_RenderTargets	= nullptr;
			InputLayout*		_InputLayout	= nullptr;
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
