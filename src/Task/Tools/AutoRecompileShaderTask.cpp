#include "Task/Tools/AutoRecompileShaderTask.hpp"

#include "Graphics/GraphicsContext.hpp"
#include "File/FilePath.hpp"
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <array>
#include <locale>
#include <codecvt>

namespace Eternal
{
	namespace Tasks
	{
		using namespace Eternal::FileSystem;

		enum class AutoRecompileShaderState
		{
			AUTO_RECOMPILE_SHADER_QUERY,
			AUTO_RECOMPILE_SHADER_WAIT,
			AUTO_RECOMPILE_SHADER_POLL
		};

		struct AutoRecompileShaderPrivateData
		{
			static constexpr uint32_t InvalidWaitObject = ~0u;

			AutoRecompileShaderState		AutoRecompileState;
			vector<HANDLE>					DirectoryHandles;
			vector<OVERLAPPED>				DirectoryOverlappeds;
			vector<array<uint8_t, 1024>>	AutoRecompileChangeBuffers;
			uint32_t						WaitObjectIndex = InvalidWaitObject;
		};

		AutoRecompileShaderTask::AutoRecompileShaderTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ GraphicsContext& InContext)
			: Task(InTaskCreateInformation)
			, _AutoRecompileShaderData(new AutoRecompileShaderPrivateData())
			, _Context(InContext)
		{
			const vector<string>& FolderPaths							= FilePath::GetFolderPaths(FileType::FILE_TYPE_SHADERS);
			vector<HANDLE>& DirectoryHandles							= _AutoRecompileShaderData->DirectoryHandles;
			vector<OVERLAPPED>& DirectoryOverlappeds					= _AutoRecompileShaderData->DirectoryOverlappeds;
			vector<array<uint8_t, 1024>>& AutoRecompileChangeBuffers	= _AutoRecompileShaderData->AutoRecompileChangeBuffers;

			DirectoryHandles.resize(FolderPaths.size());
			DirectoryOverlappeds.resize(FolderPaths.size());
			AutoRecompileChangeBuffers.resize(FolderPaths.size());

			for (uint32_t FolderIndex = 0; FolderIndex < DirectoryHandles.size(); ++FolderIndex)
			{
				DirectoryHandles[FolderIndex] = CreateFile(
					FolderPaths[FolderIndex].c_str(),
					FILE_LIST_DIRECTORY,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
					NULL
				);

				ETERNAL_ASSERT(DirectoryHandles[FolderIndex] != INVALID_HANDLE_VALUE);

				DirectoryOverlappeds[FolderIndex].hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			}
		}

		AutoRecompileShaderTask::~AutoRecompileShaderTask()
		{
			delete _AutoRecompileShaderData;
			_AutoRecompileShaderData = nullptr;
		}

		void AutoRecompileShaderTask::DoExecute()
		{
			switch (_AutoRecompileShaderData->AutoRecompileState)
			{
			case AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_QUERY:
			{
				for (uint32_t DirectoryIndex = 0; DirectoryIndex < _AutoRecompileShaderData->DirectoryHandles.size(); ++DirectoryIndex)
				{
					BOOL ReadDirectorySuccess = ReadDirectoryChangesW(
						_AutoRecompileShaderData->DirectoryHandles[DirectoryIndex],
						_AutoRecompileShaderData->AutoRecompileChangeBuffers[DirectoryIndex].data(), 1024,
						TRUE,
						FILE_NOTIFY_CHANGE_FILE_NAME |
						FILE_NOTIFY_CHANGE_DIR_NAME |
						FILE_NOTIFY_CHANGE_LAST_WRITE,
						NULL,
						&_AutoRecompileShaderData->DirectoryOverlappeds[DirectoryIndex],
						NULL
					);
					ETERNAL_ASSERT(ReadDirectorySuccess == TRUE);
				}
				_AutoRecompileShaderData->AutoRecompileState = AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_WAIT;
			} break;

			case AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_WAIT:
			{
				HANDLE* Handles = reinterpret_cast<HANDLE*>(alloca(sizeof(HANDLE) * _AutoRecompileShaderData->DirectoryOverlappeds.size()));

				for (uint32_t DirectoryIndex = 0; DirectoryIndex < _AutoRecompileShaderData->DirectoryOverlappeds.size(); ++DirectoryIndex)
					Handles[DirectoryIndex] = _AutoRecompileShaderData->DirectoryOverlappeds[DirectoryIndex].hEvent;

				DWORD WaitResult = WaitForMultipleObjects(
					static_cast<DWORD>(_AutoRecompileShaderData->DirectoryOverlappeds.size()),
					Handles,
					FALSE,
					0
				);

				for (uint32_t DirectoryIndex = 0; DirectoryIndex < _AutoRecompileShaderData->DirectoryOverlappeds.size(); ++DirectoryIndex)
				{
					if (WaitResult == (WAIT_OBJECT_0 + DirectoryIndex))
					{
						_AutoRecompileShaderData->WaitObjectIndex		= DirectoryIndex;
						_AutoRecompileShaderData->AutoRecompileState	= AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_POLL;
						break;
					}
				}
			} break;

			case AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_POLL:
			{
				DWORD BytesTransferred;
				BOOL OverlappedResult = GetOverlappedResult(
					_AutoRecompileShaderData->DirectoryHandles[_AutoRecompileShaderData->WaitObjectIndex],
					&_AutoRecompileShaderData->DirectoryOverlappeds[_AutoRecompileShaderData->WaitObjectIndex],
					&BytesTransferred,
					FALSE
				);

				ETERNAL_ASSERT(OverlappedResult == TRUE);

				char DebugFileName[1024] = {};

				FILE_NOTIFY_INFORMATION* FileNotifyInformation = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(_AutoRecompileShaderData->AutoRecompileChangeBuffers[_AutoRecompileShaderData->WaitObjectIndex].data());
				for (;;)
				{
					DWORD FileNameLength = FileNotifyInformation->FileNameLength / sizeof(wchar_t);
					wchar_t* FileName = reinterpret_cast<wchar_t*>(alloca((FileNameLength + 1) * sizeof(wchar_t)));
					memcpy(FileName, FileNotifyInformation->FileName, FileNameLength * sizeof(wchar_t));
					FileName[FileNameLength] = 0;
					wstring FileNameUTF8(FileName);
					string FileNameString = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(FileNameUTF8);

					if (FileNotifyInformation->Action == FILE_ACTION_RENAMED_NEW_NAME)
					{
						string FullPathSource = FilePath::Find(FileNameString, FileType::FILE_TYPE_SHADERS);
						if (FullPathSource.length() > 0)
						{
							ResolvedPipelineDependency PipelineDependencies = _Context.GetPipelineDependency().ResolveSource(FullPathSource);
							if (PipelineDependencies.IsShaderSourceResolved())
								_Context.RegisterPipelineRecompile(PipelineDependencies);
						}
					}

					if (FileNotifyInformation->NextEntryOffset)
						*reinterpret_cast<uint8_t**>(&FileNotifyInformation) += FileNotifyInformation->NextEntryOffset;
					else
						break;
				}
				_AutoRecompileShaderData->AutoRecompileState = AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_QUERY;
			} break;

			default:
				ETERNAL_BREAK();
				_AutoRecompileShaderData->AutoRecompileState = AutoRecompileShaderState::AUTO_RECOMPILE_SHADER_QUERY;
				break;
			}
		}
	}
}