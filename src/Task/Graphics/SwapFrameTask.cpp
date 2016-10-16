#include "Task/Graphics/SwapFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Context.hpp"
#include "GraphicData/ContextCollection.hpp"

using namespace Eternal::Task;
using namespace Eternal::GraphicData;

SwapFrameTask::SwapFrameTask(_In_ Renderer& RendererObj, _In_ Context& MainContext, _In_ ContextCollection& DeferredContexts)
	: _Renderer(RendererObj)
	, _MainContext(MainContext)
	, _DeferredContexts(DeferredContexts)
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

	_DeferredContexts.Flush(_MainContext);
	_Renderer.Flush();

	SetState(DONE);
}

void SwapFrameTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}
