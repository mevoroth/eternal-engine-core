#include "Task/TimeTask.hpp"

#include "Time/Time.hpp"

using namespace Eternal::Time;
using namespace Eternal::Task;

TimeTask::TimeTask(_In_ Eternal::Time::Time* TimeObj)
	: _Time(TimeObj)
{
}

void TimeTask::Setup()
{
	SetState(SETUP);
}

void TimeTask::Execute()
{
	SetState(EXECUTING);
	_Time->Update();
	SetState(DONE);
}

void TimeTask::Reset()
{
	SetState(IDLE);
}
