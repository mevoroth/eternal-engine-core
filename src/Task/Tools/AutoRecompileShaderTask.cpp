#include "Task/Tools/AutoRecompileShaderTask.hpp"

#include "Macros/Macros.hpp"
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>
#include "Graphics/ShaderFactory.hpp"

using namespace Eternal::Task;
using namespace Eternal::Graphics;

AutoRecompileShaderTask::AutoRecompileShaderTask()
{
	vector<string>& ShaderPaths = ShaderFactory::Get()->GetShaderPaths();
	_FilesWatchHandlesCount = ShaderPaths.size();
	_FilesWatchHandles = new void*[_FilesWatchHandlesCount];
	_DirectoryHandles = new void*[_FilesWatchHandlesCount];

	for (int ShaderPathIndex = 0; ShaderPathIndex < ShaderPaths.size(); ++ShaderPathIndex)
	{
		_FilesWatchHandles[ShaderPathIndex] = FindFirstChangeNotification(
			(ShaderPaths[ShaderPathIndex]).c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_LAST_WRITE
		);
		ETERNAL_ASSERT(_FilesWatchHandles[ShaderPathIndex] != INVALID_HANDLE_VALUE);
		_DirectoryHandles[ShaderPathIndex] = CreateFile((ShaderPaths[ShaderPathIndex]).c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		ETERNAL_ASSERT(_DirectoryHandles[ShaderPathIndex] != INVALID_HANDLE_VALUE);
		DWORD LastError = GetLastError();

		char ErrorMessage[1024];
		sprintf_s(ErrorMessage, "Error: %d\n", LastError);
	}
}

AutoRecompileShaderTask::~AutoRecompileShaderTask()
{
	delete[] _FilesWatchHandles;
	_FilesWatchHandles = nullptr;
	delete[] _DirectoryHandles;
	_DirectoryHandles = nullptr;
}

void AutoRecompileShaderTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void AutoRecompileShaderTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	//vector<string>& ShaderPaths = ShaderFactory::Get()->GetShaderPaths();
	//DWORD ObjectIndex = WaitForMultipleObjects(_FilesWatchHandlesCount, _FilesWatchHandles, FALSE, INFINITE);

	//ShaderFactory::Get()->Recompile();

	//if (FindNextChangeNotification(_FilesWatchHandles[ObjectIndex]))
	//{
	//	OutputDebugString("processed\n");
	//}
	//else
	//{
	//	// BUG?
	//}

	SetState(DONE);
}

void AutoRecompileShaderTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}
