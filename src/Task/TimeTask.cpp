#include "Task/TimeTask.hpp"

#include "Macros/Macros.hpp"
#include "Time/Time.hpp"

using namespace Eternal::Time;
using namespace Eternal::Task;

TimeTask::TimeTask(_In_ Eternal::Time::Time* TimeObj)
	: _Time(TimeObj)
{
}

void TimeTask::DoSetup()
{
}

void TimeTask::DoExecute()
{
	_Time->Update();
}

void TimeTask::DoReset()
{
}
