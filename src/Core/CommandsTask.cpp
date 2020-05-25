#include "Task/Core/CommandsTask.hpp"

#include <vector>
#include "Graphics/Device.hpp"
#include "Graphics/CommandQueue.hpp"
#include "Graphics/CommandList.hpp"
#include "Graphics/CommandListFactory.hpp"
#include "NextGenGraphics/Resource.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/GraphicGameObject.hpp"
#include "Core/MeshComponent.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/UploadBuffers.hpp"
#include "GraphicData/CommandQueues.hpp"
#include "GraphicData/GraphicBuffers.hpp"
//#include "d3d12/D3D12CommandQueue.hpp"
//#include "d3d12/D3D12CommandList.hpp"
#include "Graphics/Fence.hpp"
#include "Task/Core/Commands/EngineCommand.hpp"

using namespace std;
using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class CommandsTaskData
		{
		public:
			CommandsTaskData(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
				: _Device(DeviceObj)
				, _GraphicResources(Resources)
				, _SharedData(SharedData)
			{
				_CommandLists.resize(FRAME_LAG);
				for (uint32_t CommandListIndex = 0; CommandListIndex < _CommandLists.size(); ++CommandListIndex)
					_CommandLists[CommandListIndex] = CreateCommandList(DeviceObj, COMMAND_LIST_TYPE_GRAPHIC);
			}

			Device& GetDevice()
			{
				return _Device;
			}

			GraphicResources* GetGraphicResources()
			{
				return _GraphicResources;
			}

			StateSharedData* GetSharedData()
			{
				return _SharedData;
			}

			CommandList* GetCommandList()
			{
				return _CommandLists[_SharedData->CurrentFrame];
			}

		private:
			Device&					_Device;
			GraphicResources*		_GraphicResources	= nullptr;
			StateSharedData*		_SharedData			= nullptr;
			vector<CommandList*>	_CommandLists;
		};
	}
}

CommandsTask::CommandsTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData)
	: _CommandsTaskData(new CommandsTaskData(DeviceObj, Resources, SharedData))
{
}

void CommandsTask::DoSetup()
{
}

void CommandsTask::DoExecute()
{
	GraphicResources*					Resources			= _CommandsTaskData->GetGraphicResources();
	StateSharedData*					SharedData			= _CommandsTaskData->GetSharedData();
	vector<GraphicGameObject*>&			PendingQueue		= SharedData->GetPendingQueue();
	vector< vector<EngineCommand*> >&	Commands			= SharedData->EngineCommands[SharedData->CurrentFrame];
	Device&								DeviceObj			= _CommandsTaskData->GetDevice();
	//GCommandQueue*				GCommandQueueObj	= _CommandsTaskData->GetGraphicResources()->GetCommandQueues()->Get(COMMAND_QUEUE_GRAPHIC);
	CommandList*						LoadingCommandList	= _CommandsTaskData->GetCommandList();
	UploadBuffers*						UploadBuffersObj	= Resources->GetUploadBuffers();
	//CommandAllocator*			CommandAllocatorObj	= GCommandQueueObj->Queue->GetCommandAllocator(SharedData->CurrentFrame);

	static bool once = false;

	size_t PendingCommandsCount = 0u;
	for (uint32_t ThreadIndex = 0; ThreadIndex < Commands.size(); ++ThreadIndex)
		PendingCommandsCount += Commands[ThreadIndex].size();

	if (once && !PendingQueue.size() && !PendingCommandsCount)
		return;

	//GCommandQueueObj->Queue->Reset(SharedData->CurrentFrame);

	LoadingCommandList->Begin();
	for (uint32_t GraphicGameObjectIndex = 0; GraphicGameObjectIndex < PendingQueue.size(); ++GraphicGameObjectIndex)
	{
		GraphicGameObject*& CurrentGameObject = PendingQueue[GraphicGameObjectIndex];
		CurrentGameObject->GetMeshComponent()->GetMesh()->InitializeBuffers(DeviceObj, *LoadingCommandList, UploadBuffersObj);
		CurrentGameObject->GetMeshComponent()->GetBoundingBox()->InitializeBuffers(DeviceObj, *LoadingCommandList, UploadBuffersObj);

		//Resource* UploadBuffer = ->Pop();
		SharedData->GraphicGameObjects.push_back(CurrentGameObject);
	}

	for (uint32_t ThreadIndex = 0; ThreadIndex < Commands.size(); ++ThreadIndex)
	{
		for (uint32_t CommandIndex = 0; CommandIndex < Commands[ThreadIndex].size(); ++CommandIndex)
		{
			EngineCommand* CurrentCommand = Commands[ThreadIndex][CommandIndex];
			CurrentCommand->Execute(DeviceObj, Resources, *LoadingCommandList);
		}
		Commands[ThreadIndex].clear();
	}

	if (!once)
	{
		GraphicBuffers* GBuffers = Resources->GetGraphicBuffers();
		ResourceTransition ResourceTransitions[GRAPHIC_BUFFER_COUNT];
		for (uint32_t TransitionIndex = 0; TransitionIndex < GRAPHIC_BUFFER_NO_DEPTH_COUNT; ++TransitionIndex)
		{
			ResourceTransitions[TransitionIndex].ResourceObj	= GBuffers->GetResource((GraphicBufferKey)TransitionIndex);
			ResourceTransitions[TransitionIndex].Before			= TRANSITION_UNDEFINED;
			ResourceTransitions[TransitionIndex].After			= TRANSITION_RENDER_TARGET_WRITE;
		}
		ResourceTransitions[GRAPHIC_BUFFER_DEPTH].ResourceObj	= GBuffers->GetResource(GRAPHIC_BUFFER_DEPTH);
		ResourceTransitions[GRAPHIC_BUFFER_DEPTH].Before		= TRANSITION_UNDEFINED;
		ResourceTransitions[GRAPHIC_BUFFER_DEPTH].After			= TransitionState(/*TRANSITION_DEPTH_STENCIL_READ |*/ TRANSITION_DEPTH_STENCIL_WRITE);
		LoadingCommandList->Transition(nullptr, 0u, ResourceTransitions, GRAPHIC_BUFFER_COUNT);
		once = true;
	}

	LoadingCommandList->End();

	SharedData->RecordCommandList(LoadingCommandList);

	PendingQueue.clear();
}

void CommandsTask::DoReset()
{

}
