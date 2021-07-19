#include "Task/Tools/AutoRecompileShaderTask.hpp"

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "Graphics/ShaderFactory.hpp"

using namespace Eternal::Task;
using namespace Eternal::Graphics;

//AutoRecompileShaderTask::AutoRecompileShaderTask()
//{
//	return;
//	//vector<string>& ShaderPaths = ShaderFactory::Get()->GetShaderPaths();
//	//_FilesWatchHandlesCount = ShaderPaths.size();
//	//_FilesWatchHandles = new void*[_FilesWatchHandlesCount];
//	//_DirectoryHandles = new void*[_FilesWatchHandlesCount];
//
//	//for (int ShaderPathIndex = 0; ShaderPathIndex < ShaderPaths.size(); ++ShaderPathIndex)
//	//{
//	//	//_FilesWatchHandles[ShaderPathIndex] = FindFirstChangeNotification(
//	//	//	(ShaderPaths[ShaderPathIndex]).c_str(),
//	//	//	FALSE,
//	//	//	FILE_NOTIFY_CHANGE_LAST_WRITE
//	//	//);
//	//	//ETERNAL_ASSERT(_FilesWatchHandles[ShaderPathIndex] != INVALID_HANDLE_VALUE);
//	//	_DirectoryHandles[ShaderPathIndex] = CreateFile((ShaderPaths[ShaderPathIndex]).c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
//	//	ETERNAL_ASSERT(_DirectoryHandles[ShaderPathIndex] != INVALID_HANDLE_VALUE);
//	//	DWORD LastError = GetLastError();
//
//	//	char ErrorMessage[1024];
//	//	sprintf_s(ErrorMessage, "Error: %d\n", LastError);
//	//}
//}

AutoRecompileShaderTask::~AutoRecompileShaderTask()
{
	delete[] _FilesWatchHandles;
	_FilesWatchHandles = nullptr;
	delete[] _DirectoryHandles;
	_DirectoryHandles = nullptr;
}


void AutoRecompileShaderTask::DoExecute()
{
	//vector<string>& ShaderPaths = ShaderFactory::Get()->GetShaderPaths();
	//FILE_NOTIFY_INFORMATION FileNotifyInformation[1024];

	//for (;;)
	//{
	//	DWORD BytesReturned;
	//	for (int ShaderPathIndex = 0; ShaderPathIndex < ShaderPaths.size(); ++ShaderPathIndex)
	//	{
	//		if (!ReadDirectoryChangesW(_DirectoryHandles[ShaderPathIndex], &FileNotifyInformation, 1024, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &BytesReturned, nullptr, nullptr))
	//		{
	//			OutputDebugString("error");
	//		}
	//		else
	//		{
	//			OutputDebugString("ok");
	//			ETERNAL_BREAK();
	//		}
	//	}
	//}

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
}
