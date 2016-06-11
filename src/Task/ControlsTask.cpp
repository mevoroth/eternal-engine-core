#include "Task/ControlsTask.hpp"

#include "Macros/Macros.hpp"

#include "Input/Input.hpp"

using namespace Eternal::Task;
using namespace Eternal::Input;

void ControlsTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void ControlsTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}

void ControlsTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	for (int InputIndex = 0; InputIndex < _Inputs.size(); ++InputIndex)
	{
		_Inputs[InputIndex]->Update();
	}

	SetState(DONE);
}

void ControlsTask::RegisterInput(_In_ Input* InputObj)
{
	_Inputs.push_back(InputObj);
}
