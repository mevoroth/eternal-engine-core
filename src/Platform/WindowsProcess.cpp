#include "Platform/WindowsProcess.hpp"

#include "Input/Input.hpp"

Eternal::Input::Input* Eternal::Platform::WindowsProcess::_InputHandler = nullptr;
Eternal::Input::Input::Key _KeyMapping[256];

namespace Eternal
{
	namespace Platform
	{
		WindowsProcess::WindowsProcess()
		{
			for (uint32_t KeyIndex = 0; KeyIndex < ETERNAL_ARRAYSIZE(_KeyMapping); ++KeyIndex)
			{
				_KeyMapping[KeyIndex] = Eternal::Input::Input::Key::INVALID_KEY;
			}

			Map(VK_BACK, Eternal::Input::Input::BACKSPACE);
			Map(VK_TAB, Eternal::Input::Input::TAB);
			Map(VK_RETURN, Eternal::Input::Input::RETURN);
			Map(VK_PAUSE, Eternal::Input::Input::PAUSE);
			Map(VK_CAPITAL, Eternal::Input::Input::CAPSLOCK);
			Map(VK_ESCAPE, Eternal::Input::Input::ESC);
			Map(VK_SPACE, Eternal::Input::Input::SPACE);

			Map(VK_PRIOR, Eternal::Input::Input::PGUP);
			Map(VK_NEXT, Eternal::Input::Input::PGDOWN);
			Map(VK_END, Eternal::Input::Input::END);
			Map(VK_HOME, Eternal::Input::Input::HOME);
			Map(VK_LEFT, Eternal::Input::Input::LEFT);
			Map(VK_UP, Eternal::Input::Input::UP);
			Map(VK_RIGHT, Eternal::Input::Input::RIGHT);
			Map(VK_DOWN, Eternal::Input::Input::DOWN);
			Map(VK_PRINT, Eternal::Input::Input::PRINT);
			Map(VK_SNAPSHOT, Eternal::Input::Input::PRINTSCR);
			Map(VK_INSERT, Eternal::Input::Input::INS);
			Map(VK_DELETE, Eternal::Input::Input::DEL);

			// Numeric keys above letters
			MapRange('0', Eternal::Input::Input::ALPHA0, 10);

			// Letters
			MapRange('A', Eternal::Input::Input::A, 26);

			// Numpad keys
			MapRange(VK_NUMPAD0, Eternal::Input::Input::KP0, 10);

			Map(VK_LWIN, Eternal::Input::Input::LEFTWINDOWS);
			Map(VK_RWIN, Eternal::Input::Input::RIGHTWINDOWS);
			Map(VK_APPS, Eternal::Input::Input::APPLICATION);

			Map(VK_MULTIPLY, Eternal::Input::Input::KPMUL);
			Map(VK_ADD, Eternal::Input::Input::KPPLUS);
			//Map(VK_SEPARATOR, Input::KP
			Map(VK_SUBTRACT, Eternal::Input::Input::KPMINUS);
			Map(VK_DECIMAL, Eternal::Input::Input::KPPERIOD);
			Map(VK_DIVIDE, Eternal::Input::Input::KPDIV);

			// Function buttons
			MapRange(VK_F1, Eternal::Input::Input::F1, 12);

			Map(VK_NUMLOCK, Eternal::Input::Input::NUMLOCK);
			Map(VK_SCROLL, Eternal::Input::Input::SCROLLLOCK);

			Map(VK_LSHIFT, Eternal::Input::Input::LEFTSHIFT);
			Map(VK_RSHIFT, Eternal::Input::Input::RIGHTSHIFT);
			Map(VK_LCONTROL, Eternal::Input::Input::LEFTCTRL);
			Map(VK_RCONTROL, Eternal::Input::Input::RIGHTCTRL);

			Map(VK_OEM_PLUS, Eternal::Input::Input::EQ);
			Map(VK_OEM_COMMA, Eternal::Input::Input::COMMA);
			Map(VK_OEM_MINUS, Eternal::Input::Input::MINUS);
			Map(VK_OEM_PERIOD, Eternal::Input::Input::PERIOD);
			Map(VK_OEM_1, Eternal::Input::Input::DOLLAR);
			Map(VK_OEM_2, Eternal::Input::Input::COLON);
			Map(VK_OEM_4, Eternal::Input::Input::RIGHTPARENTHESIS);
			Map(VK_OEM_5, Eternal::Input::Input::ASTERISK);
			Map(VK_OEM_102, Eternal::Input::Input::LESS);
			Map(VK_OEM_8, Eternal::Input::Input::EXCLAIM);
		}

		void WindowsProcess::SetInputHandler(_In_ Eternal::Input::Input* InputHandler)
		{
			_InputHandler = InputHandler;
		}

		void WindowsProcess::MapRange(_In_ uint32_t WindowsKeyStart, _In_ const Eternal::Input::Input::Key& KeyNameStart, _In_ uint32_t Range)
		{
			for (uint32_t KeyIndex = 0; KeyIndex < Range; ++KeyIndex)
			{
				_KeyMapping[WindowsKeyStart + KeyIndex] = (Eternal::Input::Input::Key)(KeyNameStart + KeyIndex);
			}
		}

		void WindowsProcess::Map(_In_ uint32_t WindowsKey, _In_ const Eternal::Input::Input::Key& KeyName)
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
				_InputHandler->NotifyKeyPressed(Eternal::Input::Input::MOUSE0);
			} break;

			case WM_LBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Eternal::Input::Input::MOUSE0);
			} break;

			case WM_RBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(Eternal::Input::Input::MOUSE1);
			} break;

			case WM_RBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Eternal::Input::Input::MOUSE1);
			} break;

			case WM_MBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(Eternal::Input::Input::MOUSE2);
			} break;

			case WM_MBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(Eternal::Input::Input::MOUSE2);
			} break;

			case WM_MOUSEMOVE:
			{
				_InputHandler->NotifyAxis(Eternal::Input::Input::MOUSE_X, (float)(lParam & 0xFFFF));
				_InputHandler->NotifyAxis(Eternal::Input::Input::MOUSE_Y, (float)(lParam >> 16));
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
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}
