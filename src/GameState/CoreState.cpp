#include "GameState/CoreState.hpp"

#include "Platform/WindowsProcess.hpp"
#include "Graphics/DeviceFactory.hpp"
#include "Graphics/RendererFactory.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderFactoryFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/LogFactory.hpp"
#include "Parallel/TaskManager.hpp"
#include "Task/ControlsTask.hpp"
#include "Task/ImguiBeginTask.hpp"
#include "Task/ImguiEndTask.hpp"
#include "Task/TimeTask.hpp"
#include "Task/Core/UpdateComponentTask.hpp"
#include "Task/Core/GameStateTask.hpp"
#include "Task/Graphics/SolidGBufferTask.hpp"
#include "Task/Graphics/SwapFrameTask.hpp"
#include "Resources/Pool.hpp"
#include "Core/TransformComponent.hpp"

#include "Import/fbx/ImportFbx.hpp"

using namespace Eternal::Resources;
using namespace Eternal::Core;

extern Pool<TransformComponent>* g_TransformComponentPool;

namespace Eternal
{
	namespace GameState
	{
		using namespace Eternal::Input;
		using namespace Eternal::Task;
		using namespace Eternal::Platform;
		using namespace Eternal::Log;

		CoreState::CoreState(_In_ const CoreStateSettings& Settings, _In_ HINSTANCE hInstance, _In_ int nCmdShow, _In_ GameState* InitialGameState)
			: _Settings(Settings)
			, _hInstance(hInstance)
			, _nCmdShow(nCmdShow)
			, _InitialGameState(InitialGameState)
		{
			ETERNAL_ASSERT(_InitialGameState);
		}

		void CoreState::Begin()
		{
			_Time = CreateTime(TimeType::WIN);

			_FileLog = CreateLog(FILE, "Eternal");
			_ConsoleLog = CreateLog(CONSOLE, "Eternal");
			Eternal::Log::Log* Logs[] =
			{
				_FileLog,
				_ConsoleLog
			};
			_MultiChannelLog = CreateMultiChannelLog(Logs, ETERNAL_ARRAYSIZE(Logs));

			_PadInput = CreateInput(XINPUT);
			_KeyboardInput = CreateInput(InputType::WIN);
			Eternal::Input::Input* Inputs[] =
			{
				_PadInput,
				_KeyboardInput
			};
			_MultiInput = CreateMultiInput(Inputs, ETERNAL_ARRAYSIZE(Inputs));

			WindowsProcess::SetInputHandler(_KeyboardInput);
			_WindowsProcess = new WindowsProcess();

			_Device = CreateDevice(WINDOWS, WindowsProcess::WindowProc, _hInstance, _nCmdShow, "ReShield", "EternalClass");
			_Renderer = CreateRenderer(RENDERER_D3D11);

			_InitializeGraphicContexts();

			_ShaderFactory = CreateShaderFactory(SHADER_FACTORY_D3D11);

			_ImportFbx = new ImportFbx();
			_ImportFbx->RegisterPath(_Settings.FBXIncludePath);

			_TaskManager = new TaskManager();

			_InitializePools();
			_InitializeTasks();

			_TaskManager->GetTaskScheduler().PushTask(_ControlsTask);
			_TaskManager->GetTaskScheduler().PushTask(_TimeTask);
			{
				_TaskManager->GetTaskScheduler().PushTask(_UpdateComponentTask, _ControlsTask);
				_TaskManager->GetTaskScheduler().PushTask(_UpdateComponentTask, _TimeTask);
			}
			{
				_TaskManager->GetTaskScheduler().PushTask(_ImguiBeginTask, _ControlsTask);
				_TaskManager->GetTaskScheduler().PushTask(_ImguiBeginTask, _TimeTask);
			}
			{
				_TaskManager->GetTaskScheduler().PushTask(_GameStateTask, _UpdateComponentTask);
				_TaskManager->GetTaskScheduler().PushTask(_GameStateTask, _ImguiBeginTask);
			}
			_TaskManager->GetTaskScheduler().PushTask(_SolidGBufferTask, _GameStateTask);
			_TaskManager->GetTaskScheduler().PushTask(_ImguiEndTask, _GameStateTask);
			{
				_TaskManager->GetTaskScheduler().PushTask(_SwapFrameTask, _SolidGBufferTask);
				_TaskManager->GetTaskScheduler().PushTask(_SwapFrameTask, _ImguiEndTask);
			}
		}
		
		void CoreState::Update()
		{
			while (((GameStateTask*)_GameStateTask)->GetRemainingState())
			{
				WindowsProcess::ExecuteMessageLoop();

				_TaskManager->Schedule();
				_TaskManager->Barrier();
				_TaskManager->GetTaskScheduler().Reset();
			}
		}
		
		Core::GameState* CoreState::NextState()
		{
			return nullptr;
		}

		void CoreState::End()
		{
			_ReleaseTasks();
			_ReleasePools();

			delete _TaskManager;
			_TaskManager = nullptr;

			delete _ShaderFactory;
			_ShaderFactory = nullptr;

			delete _Renderer;
			_Renderer = nullptr;

			delete _Device;
			_Device = nullptr;

			delete _WindowsProcess;
			_WindowsProcess = nullptr;

			delete _MultiInput;
			_MultiInput = nullptr;

			delete _PadInput;
			_PadInput = nullptr;

			delete _KeyboardInput;
			_KeyboardInput = nullptr;

			delete _MultiChannelLog;
			_MultiChannelLog = nullptr;

			delete _ConsoleLog;
			_ConsoleLog = nullptr;

			delete _FileLog;
			_FileLog = nullptr;

			delete _Time;
			_Time = nullptr;
		}

		void CoreState::_InitializePools()
		{
			TransformComponent::Initialize();
		}

		void CoreState::_ReleasePools()
		{
			TransformComponent::Release();
		}

		void CoreState::_InitializeTasks()
		{
			ControlsTask* ControlsTaskObj = new ControlsTask();
			ControlsTaskObj->SetTaskName("Controls Task");
			ControlsTaskObj->RegisterInput(_PadInput);
			ControlsTaskObj->RegisterInput(_KeyboardInput);
			_ControlsTask = ControlsTaskObj;

			ImguiBeginTask* ImguiBeginTaskObj = new ImguiBeginTask(_Device->GetWindow(), Device::WIDTH, Device::HEIGHT);
			ImguiBeginTaskObj->SetTaskName("Imgui Begin Task");
			ImguiBeginTaskObj->SetupKeyboard(_KeyboardInput);
			_ImguiBeginTask = ImguiBeginTaskObj;

			ImguiEndTask* ImguiEndTaskObj = new ImguiEndTask(*_Renderer->GetMainContext());
			ImguiEndTaskObj->SetTaskName("Imgui End Task");
			_ImguiEndTask = ImguiEndTaskObj;

			TimeTask* TimeTaskObj = new TimeTask(_Time);
			TimeTaskObj->SetTaskName("Time Task");
			_TimeTask = TimeTaskObj;

			UpdateComponentTask< Pool<TransformComponent> >* UpdateComponentTaskObj = new UpdateComponentTask< Pool<TransformComponent> >(_Time, g_TransformComponentPool);
			UpdateComponentTaskObj->SetTaskName("Update Transform Pool Task");
			_UpdateComponentTask = UpdateComponentTaskObj;

			GameStateTask* GameStateTaskObj = new GameStateTask(_InitialGameState);
			GameStateTaskObj->SetTaskName("Game State Task");
			_GameStateTask = GameStateTaskObj;

			SolidGBufferTask* SolidGBufferTaskObj = new SolidGBufferTask(*_Contexts[0]);
			SolidGBufferTaskObj->SetTaskName("Solid GBuffer Task");
			_SolidGBufferTask = SolidGBufferTaskObj;

			SwapFrameTask* SwapFrameTaskObj = new SwapFrameTask(*_Renderer);
			SwapFrameTaskObj->SetTaskName("Swap Frame Task");
			_SwapFrameTask = SwapFrameTaskObj;
		}

		void CoreState::_ReleaseTasks()
		{
			delete _GameStateTask;
			_GameStateTask = nullptr;

			delete _UpdateComponentTask;
			_UpdateComponentTask = nullptr;

			delete _TimeTask;
			_TimeTask = nullptr;

			delete _ImguiEndTask;
			_ImguiEndTask = nullptr;

			delete _ImguiBeginTask;
			_ImguiBeginTask = nullptr;

			delete _ControlsTask;
			_ControlsTask = nullptr;
		}

		void CoreState::_InitializeGraphicContexts()
		{
			for (int GraphicContextIndex = 0; GraphicContextIndex < 4; ++GraphicContextIndex)
			{
				_Contexts[GraphicContextIndex] = _Renderer->CreateDeferredContext();
			}
		}

		void CoreState::_ReleaseGraphicContexts()
		{

		}
	}
}
