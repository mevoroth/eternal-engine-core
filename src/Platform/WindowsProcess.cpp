#if ETERNAL_PLATFORM_WINDOWS

#include "Platform/WindowsProcess.hpp"

#include "Input/Input.hpp"

Eternal::InputSystem::Input* Eternal::Platform::WindowsProcess::_InputHandler = nullptr;
Eternal::InputSystem::Input::Key _KeyMapping[256];

namespace Eternal
{
	namespace Platform
	{
		using namespace Eternal::InputSystem;

		bool* WindowsProcess::_IsRunning = nullptr;

		WindowsProcess::WindowsProcess()
		{
			for (uint32_t KeyIndex = 0; KeyIndex < ETERNAL_ARRAYSIZE(_KeyMapping); ++KeyIndex)
			{
				_KeyMapping[KeyIndex] = Input::INVALID_KEY;
			}

			Map(VK_BACK,			Input::BACKSPACE);
			Map(VK_TAB,				Input::TAB);
			Map(VK_RETURN,			Input::RETURN);
			Map(VK_PAUSE,			Input::PAUSE);
			Map(VK_CAPITAL,			Input::CAPSLOCK);
			Map(VK_ESCAPE,			Input::ESC);
			Map(VK_SPACE,			Input::SPACE);

			Map(VK_PRIOR,			Input::PGUP);
			Map(VK_NEXT,			Input::PGDOWN);
			Map(VK_END,				Input::END);
			Map(VK_HOME,			Input::HOME);
			Map(VK_LEFT,			Input::LEFT);
			Map(VK_UP,				Input::UP);
			Map(VK_RIGHT,			Input::RIGHT);
			Map(VK_DOWN,			Input::DOWN);
			Map(VK_PRINT,			Input::PRINT);
			Map(VK_SNAPSHOT,		Input::PRINTSCR);
			Map(VK_INSERT,			Input::INS);
			Map(VK_DELETE,			Input::DEL);

			// Numeric keys above letters
			MapRange('0',			Input::ALPHA0, 10);

			// Letters
			MapRange('A',			Input::A, 26);

			// Numpad keys
			MapRange(VK_NUMPAD0,	Input::KP0, 10);

			Map(VK_LWIN,			Input::LEFTWINDOWS);
			Map(VK_RWIN,			Input::RIGHTWINDOWS);
			Map(VK_APPS,			Input::APPLICATION);

			Map(VK_MULTIPLY,		Input::KPMUL);
			Map(VK_ADD,				Input::KPPLUS);
			Map(VK_SUBTRACT,		Input::KPMINUS);
			Map(VK_DECIMAL,			Input::KPPERIOD);
			Map(VK_DIVIDE,			Input::KPDIV);

			// Function buttons
			MapRange(VK_F1,			Input::F1, 12);

			Map(VK_NUMLOCK,			Input::NUMLOCK);
			Map(VK_SCROLL,			Input::SCROLLLOCK);

			Map(VK_LSHIFT,			Input::LEFTSHIFT);
			Map(VK_RSHIFT,			Input::RIGHTSHIFT);
			Map(VK_LCONTROL,		Input::LEFTCTRL);
			Map(VK_RCONTROL,		Input::RIGHTCTRL);

			Map(VK_OEM_PLUS,		Input::EQ);
			Map(VK_OEM_COMMA,		Input::COMMA);
			Map(VK_OEM_MINUS,		Input::MINUS);
			Map(VK_OEM_PERIOD,		Input::PERIOD);
			Map(VK_OEM_1,			Input::DOLLAR);
			Map(VK_OEM_2,			Input::COLON);
			Map(VK_OEM_4,			Input::RIGHTPARENTHESIS);
			Map(VK_OEM_5,			Input::ASTERISK);
			Map(VK_OEM_102,			Input::LESS);
			Map(VK_OEM_8,			Input::EXCLAIM);
		}

		void WindowsProcess::SetInputHandler(_In_ Input* InputHandler)
		{
			_InputHandler = InputHandler;
		}

		void WindowsProcess::MapRange(_In_ uint32_t WindowsKeyStart, _In_ const Input::Key& KeyNameStart, _In_ uint32_t Range)
		{
			for (uint32_t KeyIndex = 0; KeyIndex < Range; ++KeyIndex)
			{
				_KeyMapping[WindowsKeyStart + KeyIndex] = (Input::Key)(KeyNameStart + KeyIndex);
			}
		}

		void WindowsProcess::Map(_In_ uint32_t WindowsKey, _In_ const Input::Key& KeyName)
		{
			_KeyMapping[WindowsKey] = KeyName;
		}

		void WindowsProcess::ExecuteMessageLoop()
		{
			MSG Message = { 0 };
			if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}

		LRESULT WindowsProcess::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			ETERNAL_ASSERT(_InputHandler);

			switch (message)
			{
			case WM_PAINT:
			{
				PAINTSTRUCT PaintStruct;
				HDC Hdc = BeginPaint(hWnd, &PaintStruct);
				EndPaint(hWnd, &PaintStruct);
			} break;

			case WM_LBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(Input::MOUSE0);
			} break;

			case WM_LBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Input::MOUSE0);
			} break;

			case WM_RBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(Input::MOUSE1);
			} break;

			case WM_RBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Input::MOUSE1);
			} break;

			case WM_MBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(Input::MOUSE2);
			} break;

			case WM_MBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Input::MOUSE2);
			} break;

			case WM_MOUSEMOVE:
			{
				_InputHandler->NotifyAxis(Input::MOUSE_X, (float)(lParam & 0xFFFF));
				_InputHandler->NotifyAxis(Input::MOUSE_Y, (float)(lParam >> 16));
			} break;

			case WM_KEYDOWN:
			{
				_InputHandler->NotifyKeyPressed(_KeyMapping[wParam]);
			} break;

			case WM_KEYUP:
			{
				_InputHandler->NotifyKeyReleased(_KeyMapping[wParam]);
			} break;

			case WM_DESTROY:
				if (_IsRunning)
					*_IsRunning = false;
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}

#endif
