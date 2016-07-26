#ifndef _CORE_STATE_HPP_
#define _CORE_STATE_HPP_

#include "Core/GameState.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

class WindowsProcess;

namespace Eternal
{
	namespace Graphics
	{
		class Device;
		class Renderer;
		class ShaderFactory;
	}
	namespace Input
	{
		class Input;
	}
	namespace Parallel
	{
		class TaskManager;
		class Task;
	}
	namespace Time
	{
		class Time;
	}
	namespace Platform
	{
		class WindowsProcess;
	}
}

namespace Eternal
{

	namespace GameState
	{
		using namespace Eternal::Core;
		using namespace Eternal::Graphics;
		using namespace Eternal::Parallel;
		using namespace Eternal::Platform;

		class CoreState : public GameState
		{
		public:
			CoreState(_In_ HINSTANCE hInstance, _In_ int nCmdShow, _In_ GameState* InitialGameState);

			virtual void Begin() override;
			virtual void Update() override;
			virtual GameState* NextState() override;
			virtual void End() override;

			bool& GetQuit() { return _Quit; }

		private:
			bool _Quit = false;

			Eternal::Time::Time* _Time = nullptr;

			WindowsProcess* _WindowsProcess = nullptr;

			Device* _Device = nullptr;
			HINSTANCE _hInstance = nullptr;
			int _nCmdShow = 0;

			Renderer* _Renderer = nullptr;

			ShaderFactory* _ShaderFactory = nullptr;

			TaskManager* _TaskManager = nullptr;

			Eternal::Input::Input* _KeyboardInput = nullptr;
			Eternal::Input::Input* _PadInput = nullptr;

			GameState* _InitialGameState = nullptr;

			Task* _ControlsTask = nullptr;
			Task* _ImguiBeginTask = nullptr;
			Task* _ImguiEndTask = nullptr;
			Task* _TimeTask = nullptr;
			Task* _UpdateComponentTask = nullptr;
			Task* _GameStateTask = nullptr;

			void _InitPools();
			void _ReleasePools();
			void _InitTasks();
			void _ReleaseTasks();
		};
	}
}

#endif
