#include "Task/ControlsTask.hpp"

#include "Macros/Macros.hpp"

#include "Input/Input.hpp"

using namespace Eternal::Task;
using namespace Eternal::Input;

void ControlsTask::DoSetup()
{
}

void ControlsTask::DoReset()
{
}

void ControlsTask::DoExecute()
{
	for (int InputIndex = 0; InputIndex < _Inputs.size(); ++InputIndex)
	{
		_Inputs[InputIndex]->Update();
	}
}

void ControlsTask::RegisterInput(_In_ Eternal::Input::Input* InputObj)
{
	_Inputs.push_back(InputObj);
}
