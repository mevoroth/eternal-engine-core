#pragma once

#if ETERNAL_PLATFORM_WINDOWS || ETERNAL_PLATFORM_SCARLETT

#include <Windows.h>

namespace Eternal
{
	namespace InputSystem
	{
		enum class InputKey;

		class Input;
	}

	namespace Platform
	{
		using namespace Eternal::InputSystem;

		class MicrosoftProcess
		{
		public:
			static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			static void ExecuteMessageLoop();
			static void SetInputHandler(_In_ Input* InInputHandler);
			static void Map(_In_ uint32_t InWindowsKey, _In_ const InputKey& InKeyName);
			static void SetIsRunning(_In_ bool* InIsRunning) { _IsRunning = InIsRunning; }

			MicrosoftProcess();

		private:
			static Input*	_InputHandler;
			static bool*	_IsRunning;

			static void MapRange(_In_ uint32_t InWindowsKeyStart, _In_ const InputKey& InKeyNameStart, _In_ uint32_t InRange);
		};
	}
}

#endif
