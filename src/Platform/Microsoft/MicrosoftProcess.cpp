#if ETERNAL_PLATFORM_WINDOWS || ETERNAL_PLATFORM_SCARLETT

#include "Platform/Microsoft/MicrosoftProcess.hpp"

#include "Input/Input.hpp"
#include "Input/InputDefines.hpp"
#include "Types/Enums.hpp"

Eternal::InputSystem::Input* Eternal::Platform::MicrosoftProcess::_InputHandler = nullptr;
static Eternal::InputSystem::InputKey _KeyMapping[256];

namespace Eternal
{
	namespace Platform
	{
		using namespace Eternal::InputSystem;
		using namespace Eternal::Types;

		bool* MicrosoftProcess::_IsRunning = nullptr;

		MicrosoftProcess::MicrosoftProcess()
		{
			for (uint32_t KeyIndex = 0; KeyIndex < ETERNAL_ARRAYSIZE(_KeyMapping); ++KeyIndex)
			{
				_KeyMapping[KeyIndex] = InputKey::KEY_INVALID;
			}

			Map(VK_BACK,			InputKey::KEY_BACKSPACE);
			Map(VK_TAB,				InputKey::KEY_TAB);
			Map(VK_RETURN,			InputKey::KEY_RETURN);
			Map(VK_PAUSE,			InputKey::KEY_PAUSE);
			Map(VK_CAPITAL,			InputKey::KEY_CAPSLOCK);
			Map(VK_ESCAPE,			InputKey::KEY_ESC);
			Map(VK_SPACE,			InputKey::KEY_SPACE);

			Map(VK_PRIOR,			InputKey::KEY_PGUP);
			Map(VK_NEXT,			InputKey::KEY_PGDOWN);
			Map(VK_END,				InputKey::KEY_END);
			Map(VK_HOME,			InputKey::KEY_HOME);
			Map(VK_LEFT,			InputKey::KEY_LEFT);
			Map(VK_UP,				InputKey::KEY_UP);
			Map(VK_RIGHT,			InputKey::KEY_RIGHT);
			Map(VK_DOWN,			InputKey::KEY_DOWN);
			Map(VK_PRINT,			InputKey::KEY_PRINT);
			Map(VK_SNAPSHOT,		InputKey::KEY_PRINTSCR);
			Map(VK_INSERT,			InputKey::KEY_INS);
			Map(VK_DELETE,			InputKey::KEY_DEL);

			// Numeric keys above letters
			MapRange('0',			InputKey::KEY_ALPHA0, 10);

			// Letters
			MapRange('A',			InputKey::KEY_A, 26);

			// Numpad keys
			MapRange(VK_NUMPAD0,	InputKey::KEY_KP0, 10);

			Map(VK_LWIN,			InputKey::KEY_LEFTWINDOWS);
			Map(VK_RWIN,			InputKey::KEY_RIGHTWINDOWS);
			Map(VK_APPS,			InputKey::KEY_APPLICATION);

			Map(VK_MULTIPLY,		InputKey::KEY_KPMUL);
			Map(VK_ADD,				InputKey::KEY_KPPLUS);
			Map(VK_SUBTRACT,		InputKey::KEY_KPMINUS);
			Map(VK_DECIMAL,			InputKey::KEY_KPPERIOD);
			Map(VK_DIVIDE,			InputKey::KEY_KPDIV);

			// Function buttons
			MapRange(VK_F1,			InputKey::KEY_F1, 12);

			Map(VK_NUMLOCK,			InputKey::KEY_NUMLOCK);
			Map(VK_SCROLL,			InputKey::KEY_SCROLLLOCK);

			Map(VK_LSHIFT,			InputKey::KEY_LEFTSHIFT);
			Map(VK_RSHIFT,			InputKey::KEY_RIGHTSHIFT);
			Map(VK_LCONTROL,		InputKey::KEY_LEFTCTRL);
			Map(VK_RCONTROL,		InputKey::KEY_RIGHTCTRL);

			Map(VK_OEM_PLUS,		InputKey::KEY_EQ);
			Map(VK_OEM_COMMA,		InputKey::KEY_COMMA);
			Map(VK_OEM_MINUS,		InputKey::KEY_MINUS);
			Map(VK_OEM_PERIOD,		InputKey::KEY_PERIOD);
			Map(VK_OEM_1,			InputKey::KEY_DOLLAR);
			Map(VK_OEM_2,			InputKey::KEY_COLON);
			Map(VK_OEM_4,			InputKey::KEY_RIGHTPARENTHESIS);
			Map(VK_OEM_5,			InputKey::KEY_ASTERISK);
			Map(VK_OEM_102,			InputKey::KEY_LESS);
			Map(VK_OEM_8,			InputKey::KEY_EXCLAIM);
		}

		void MicrosoftProcess::SetInputHandler(_In_ Input* InInputHandler)
		{
			_InputHandler = InInputHandler;
		}

		void MicrosoftProcess::MapRange(_In_ uint32_t InWindowsKeyStart, _In_ const InputKey& InKeyNameStart, _In_ uint32_t InRange)
		{
			for (uint32_t KeyIndex = 0; KeyIndex < InRange; ++KeyIndex)
				_KeyMapping[InWindowsKeyStart + KeyIndex] = static_cast<InputKey>(ToUInt(InKeyNameStart) + KeyIndex);
		}

		void MicrosoftProcess::Map(_In_ uint32_t InWindowsKey, _In_ const InputKey& InKeyName)
		{
			_KeyMapping[InWindowsKey] = InKeyName;
		}

		void MicrosoftProcess::ExecuteMessageLoop()
		{
			MSG Message = { 0 };
			if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}

		LRESULT MicrosoftProcess::WindowProc(HWND InhWnd, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
		{
			ETERNAL_ASSERT(_InputHandler);

			switch (InMessage)
			{
#if ETERNAL_PLATFORM_WINDOWS
			case WM_PAINT:
			{
				PAINTSTRUCT PaintStruct;
				HDC DeviceContextHandle = BeginPaint(InhWnd, &PaintStruct);
				(void)DeviceContextHandle;
				EndPaint(InhWnd, &PaintStruct);
			} break;
#endif

			case WM_LBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(InputKey::KEY_MOUSE0);
			} break;

			case WM_LBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(InputKey::KEY_MOUSE0);
			} break;

			case WM_RBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(InputKey::KEY_MOUSE1);
			} break;

			case WM_RBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(InputKey::KEY_MOUSE1);
			} break;

			case WM_MBUTTONDOWN:
			{
				_InputHandler->NotifyKeyPressed(InputKey::KEY_MOUSE2);
			} break;

			case WM_MBUTTONUP:
			{
				_InputHandler->NotifyKeyReleased(InputKey::KEY_MOUSE2);
			} break;

			case WM_MOUSEMOVE:
			{
				_InputHandler->NotifyAxis(InputAxis::AXIS_MOUSE_X, (float)(InlParam & 0xFFFF));
				_InputHandler->NotifyAxis(InputAxis::AXIS_MOUSE_Y, (float)(InlParam >> 16));
			} break;

			case WM_KEYDOWN:
			{
				_InputHandler->NotifyKeyPressed(_KeyMapping[InwParam]);
			} break;

			case WM_KEYUP:
			{
				_InputHandler->NotifyKeyReleased(_KeyMapping[InwParam]);
			} break;

			case WM_DESTROY:
				if (_IsRunning)
					*_IsRunning = false;
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProc(InhWnd, InMessage, InwParam, InlParam);
		}
	}
}

#endif
