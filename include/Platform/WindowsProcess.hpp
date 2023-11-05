#pragma once

#if ETERNAL_PLATFORM_WINDOWS

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#include "Input/Input.hpp"

namespace Eternal
{
	namespace Platform
	{
		using namespace Eternal::InputSystem;

		class WindowsProcess
		{
		public:
			static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			static void ExecuteMessageLoop();
			static void SetInputHandler(_In_ Input* InputHandler);
			static void Map(_In_ uint32_t WindowsKey, _In_ const Input::Key& KeyName);
			static void SetIsRunning(_In_ bool* InIsRunning) { _IsRunning = InIsRunning; }

			WindowsProcess();

		private:
			static Input*	_InputHandler;
			static bool*	_IsRunning;

			static void MapRange(_In_ uint32_t WindowsKeyStart, _In_ const Input::Key& KeyNameStart, _In_ uint32_t Range);
		};
	}
}

#endif
