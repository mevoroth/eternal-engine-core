#include "Core/StateSharedData.hpp"

#include "Parallel/AtomicS32Factory.hpp"
#include "Parallel/CpuCoreCount.hpp"

using namespace Eternal::Core;

StateSharedData::StateSharedData()
{
	for (uint32_t CommandListIndex = 0; CommandListIndex < ETERNAL_ARRAYSIZE(CommandListsCount); ++CommandListIndex)
	{
		CommandListsCount[CommandListIndex] = CreateAtomicS32();
	}

	for (uint32_t FrameIndex = 0; FrameIndex < ETERNAL_ARRAYSIZE(EngineCommands); ++FrameIndex)
	{
		EngineCommands[FrameIndex].resize(CpuCoreCount());
	}
}

StateSharedData::~StateSharedData()
{
	for (uint32_t CommandListIndex = 0; CommandListIndex < ETERNAL_ARRAYSIZE(CommandListsCount); ++CommandListIndex)
	{
		delete CommandListsCount[CommandListIndex];
		CommandListsCount[CommandListIndex] = nullptr;
	}
}

void StateSharedData::RecordCommandList(_In_ CommandList* NewCommandList)
{
	int NewEntry;
	
	do
	{
		NewEntry = CommandListsCount[CurrentFrame]->Load();
	}
	while (!CommandListsCount[CurrentFrame]->CompareAndSwap(NewEntry, NewEntry + 1));

	ETERNAL_ASSERT(NewEntry < ETERNAL_ARRAYSIZE(RecordedCommandLists[CurrentFrame]));
	RecordedCommandLists[CurrentFrame][NewEntry] = NewCommandList;
}

void StateSharedData::Reset()
{
	CommandListsCount[CurrentFrame]->Store(0);
}

vector<GraphicGameObject*>& StateSharedData::GetPendingQueue()
{
	return LoadingQueues[(CurrentFrame + FRAME_LAG - 1) % FRAME_LAG];
}

vector<GraphicGameObject*>& StateSharedData::GetActiveQueue()
{
	return LoadingQueues[CurrentFrame];
}
