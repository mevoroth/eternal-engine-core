#include "Task/ImguiBeginTask.hpp"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include "Macros/Macros.hpp"
#include "imgui.h"
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

	// TODO IMPLEMENT INPUT CHAR
	//io.AddInputCharacter

	ImGui::NewFrame();

	SetState(Task::DONE);
}

void ImguiBeginTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(Task::IDLE);
}
