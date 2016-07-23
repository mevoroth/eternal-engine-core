#include "Task/Graphics/SolidGBufferTask.hpp"

using namespace Eternal::Task;

void SolidGBufferTask::Setup()
{
	SetState(SETUP);
}

void SolidGBufferTask::Execute()
{
	SetState(EXECUTING);



	SetState(DONE);
}

void SolidGBufferTask::Reset()
{
	SetState(IDLE);
}
