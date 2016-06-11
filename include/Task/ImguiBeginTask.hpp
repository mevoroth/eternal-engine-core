#ifndef _IMGUI_BEGIN_TASK_HPP_
#define _IMGUI_BEGIN_TASK_HPP_

#include <vector>
#include "Parallel/Task.hpp"
#include "Input/Input.hpp"

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

			virtual void Setup() override;
			virtual void Execute() override;
			virtual void Reset() override;

			void SetupKeyboard(Input::Input* KeyboardInput)
			{
				_KeyboardInput = KeyboardInput;
			}
			
		private:
			void Map(_In_ const Input::Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey);

			vector<Input::Input::Key> _MappedKeys;
			Input::Input* _KeyboardInput = nullptr;
		};
	}
}

#endif
