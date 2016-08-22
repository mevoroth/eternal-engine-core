#include "Task/Core/GameStateTask.hpp"

#include "Macros/Macros.hpp"
#include "Core/GameState.hpp"

using namespace Eternal::Task;

GameStateTask::GameStateTask(_In_ GameState* GameStateObj)
	: _GameState(GameStateObj)
	, _ScheduledToBegin(GameStateObj)
{
	ETERNAL_ASSERT(GameStateObj);
}

void GameStateTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETTINGUP);
	if (_ScheduledToBegin)
	{
		_ScheduledToBegin->Begin();
		_ScheduledToBegin = nullptr;
	}
	SetState(SETUP);
}

void GameStateTask::Execute()
{
	SetState(EXECUTING);
	_GameState->Update();
	SetState(DONE);
}

void GameStateTask::Reset()
{
	GameState* NextState = _GameState->NextState();

	if (!NextState						// No next state
		|| NextState != _GameState)		// Changing state
	{
		_GameState->End();
		_GameState = NextState;
		_ScheduledToBegin = _GameState;
	}

	SetState(IDLE);
}

bool GameStateTask::GetRemainingState()
{
	return _GameState != nullptr;
}
