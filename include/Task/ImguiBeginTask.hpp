#ifndef _IMGUI_BEGIN_TASK_HPP_
#define _IMGUI_BEGIN_TASK_HPP_

#include <vector>
#include <cstdint>
#include "Parallel/Task.hpp"
#include "Input/Input.hpp"
#include "imgui.h"

#ifndef _WINDEF_
struct HWND__;
typedef HWND__* HWND;
#endif

enum ImGuiKey_;

using namespace std;

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Parallel;

		class ImguiBeginTask : public Task
		{
		public:
			ImguiBeginTask(_In_ HWND HWNDObj, _In_ float Width, _In_ float Height);

			virtual void DoSetup() override;
			virtual void DoExecute() override;
			virtual void DoReset() override;

			void SetupKeyboard(Input::Input* KeyboardInput)
			{
				_KeyboardInput = KeyboardInput;
			}
			
		private:
			void Map(_In_ const Input::Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey);
			void _ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Input::Key& KeyName);
			void _ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Input::Key& KeyNameStart, _In_ uint32_t Range);

			vector<Input::Input::Key> _MappedKeys;
			Input::Input* _KeyboardInput = nullptr;
		};
	}
}

#endif
