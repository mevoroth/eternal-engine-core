#include "Task/ImguiBeginTask.hpp"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include "Macros/Macros.hpp"
#include "Input/Input.hpp"

using namespace Eternal::Task;
using namespace Eternal::Input;

Eternal::Task::ImguiBeginTask::ImguiBeginTask(_In_ HWND HWNDObj, _In_ float Width, _In_ float Height)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = Width;
	io.DisplaySize.y = Height;
	io.IniFilename = "imgui.ini";

	//unsigned char* Pixels;
	//int Width, Height;

	//io.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

	Map(Input::Input::TAB, ImGuiKey_Tab);
	Map(Input::Input::LEFT, ImGuiKey_LeftArrow);
	Map(Input::Input::RIGHT, ImGuiKey_RightArrow);
	Map(Input::Input::UP, ImGuiKey_UpArrow);
	Map(Input::Input::DOWN, ImGuiKey_DownArrow);
	Map(Input::Input::PGUP, ImGuiKey_PageUp);
	Map(Input::Input::PGDOWN, ImGuiKey_PageDown);
	Map(Input::Input::HOME, ImGuiKey_Home);
	Map(Input::Input::END, ImGuiKey_End);
	Map(Input::Input::DEL, ImGuiKey_Delete);
	Map(Input::Input::BACKSPACE, ImGuiKey_Backspace);
	Map(Input::Input::RETURN, ImGuiKey_Enter);
	Map(Input::Input::ESC, ImGuiKey_Escape);
	Map(Input::Input::A, ImGuiKey_A);
	Map(Input::Input::C, ImGuiKey_C);
	Map(Input::Input::V, ImGuiKey_V);
	Map(Input::Input::X, ImGuiKey_X);
	Map(Input::Input::Y, ImGuiKey_Y);
	Map(Input::Input::Z, ImGuiKey_Z);

	io.ImeWindowHandle = HWNDObj;
}

void ImguiBeginTask::Map(_In_ const Input::Input::Key& EternalKey, _In_ const ImGuiKey_& ImguiKey)
{
	ImGui::GetIO().KeyMap[ImguiKey] = EternalKey;
	_MappedKeys.push_back(EternalKey);
}

void ImguiBeginTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(Task::SETUP);
}

void ImguiBeginTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(Task::EXECUTING);

	ImGuiIO& io = ImGui::GetIO();
	for (int MappedKeyIndex = 0; MappedKeyIndex < _MappedKeys.size(); ++MappedKeyIndex)
	{
		io.KeysDown[_MappedKeys[MappedKeyIndex]] = _KeyboardInput->IsDown(_MappedKeys[MappedKeyIndex]);
	}

	io.MouseDown[0] = _KeyboardInput->IsPressed(Input::Input::MOUSE0); // L-Click
	io.MouseDown[1] = _KeyboardInput->IsPressed(Input::Input::MOUSE1); // R-Click
	io.MouseDown[2] = _KeyboardInput->IsPressed(Input::Input::MOUSE2); // M-Click
	io.MousePos.x = _KeyboardInput->GetAxis(Input::Input::MOUSE_X);
	io.MousePos.y = _KeyboardInput->GetAxis(Input::Input::MOUSE_Y);

	//io.AddInputCharacter(Input::Input::A)
	_ProcessInputCharacterRange((ImWchar)'a', Input::Input::A, 26);
	_ProcessInputCharacterRange((ImWchar)'0', Input::Input::KP0, 10);
	_ProcessInputCharacter('.', Input::Input::KPPERIOD);
	_ProcessInputCharacter('+', Input::Input::KPPLUS);
	_ProcessInputCharacter('-', Input::Input::KPMINUS);
	_ProcessInputCharacter('*', Input::Input::KPMUL);
	_ProcessInputCharacter('/', Input::Input::KPDIV);

	ImGui::NewFrame();

	SetState(Task::DONE);
}

void ImguiBeginTask::_ProcessInputCharacter(_In_ const ImWchar& ImguiKey, _In_ const Input::Input::Key& KeyName)
{
	ImGuiIO& io = ImGui::GetIO();
	if (Input::Input::Get()->IsPressed(KeyName))
		io.AddInputCharacter(ImguiKey);
}

void ImguiBeginTask::_ProcessInputCharacterRange(_In_ const ImWchar& ImguiKeyStart, _In_ const Input::Input::Key& KeyNameStart, _In_ uint32_t Range)
{
	ImGuiIO& io = ImGui::GetIO();
	for (uint32_t KeyIndex = 0; KeyIndex < Range; ++KeyIndex)
	{
		if (Input::Input::Get()->IsPressed((Input::Input::Key)(KeyNameStart + KeyIndex)))
			io.AddInputCharacter(ImguiKeyStart + (ImWchar)KeyIndex);
	}
}

void ImguiBeginTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(Task::IDLE);
}
