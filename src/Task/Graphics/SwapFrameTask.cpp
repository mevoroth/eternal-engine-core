#include "Task/Graphics/SwapFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Renderer.hpp"

using namespace Eternal::Task;

SwapFrameTask::SwapFrameTask(_In_ Renderer& RendererObj)
	: _Renderer(RendererObj)
{
}

void SwapFrameTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void SwapFrameTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(EXECUTING);

	_Renderer.Flush();

	SetState(DONE);
}

void SwapFrameTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}
