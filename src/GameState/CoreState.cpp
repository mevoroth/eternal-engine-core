#include "GameState/CoreState.hpp"

#include "Platform/WindowsProcess.hpp"
#include "Graphics/DeviceFactory.hpp"
#include "Graphics/RendererFactory.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/ShaderFactory.hpp"
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
#include "Task/Graphics/PrepareOpaqueTask.hpp"
#include "Task/Graphics/OpaqueTask.hpp"
#include "Task/Graphics/CompositingTask.hpp"
#include "Task/Graphics/SwapFrameTask.hpp"
#include "Resources/Pool.hpp"
#include "Core/TransformComponent.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "GraphicData/BlendStateCollection.hpp"
#include "GraphicData/RenderingListCollection.hpp"
#include "Import/fbx/ImportFbx.hpp"
#include "SaveSystem/SaveSystem.hpp"
#include "GameData/GameDatas.hpp"
#include "Graphics/Format.hpp"

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
		using namespace Eternal::SaveSystem;
		using namespace Eternal::GameData;

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
				//_FileLog,
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
			_ShaderFactory->RegisterShaderPath(_Settings.ShaderIncludePath);

			_ImportFbx = new ImportFbx();
			_ImportFbx->RegisterPath(_Settings.FBXIncludePath);

			_SaveSystem = new Eternal::SaveSystem::SaveSystem();
			_SaveSystem->RegisterSavePath(_Settings.SavePath);

			_TaskManager = new TaskManager();

			_InitializeViewports();
			_InitializeBlendStates();
			_InitializeSamplers();
			_InitializeRenderTargets();
			_InitializePools();
			_InitializeRenderingLists();
			_InitializeTasks();

			_GameDatas = new GameDatas();
			_SaveSystem->SetGameDataLoader(_GameDatas);

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
			_TaskManager->GetTaskScheduler().PushTask(_PrepareOpaqueTask, _GameStateTask);
			_TaskManager->GetTaskScheduler().PushTask(_ImguiEndTask, _GameStateTask);
			_TaskManager->GetTaskScheduler().PushTask(_OpaqueTask, _PrepareOpaqueTask);
			{
				_TaskManager->GetTaskScheduler().PushTask(_CompositingTask, _OpaqueTask);
				_TaskManager->GetTaskScheduler().PushTask(_CompositingTask, _ImguiEndTask);
			}
			_TaskManager->GetTaskScheduler().PushTask(_SwapFrameTask, _CompositingTask);
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
			_ReleaseRenderingLists();
			_ReleasePools();
			_ReleaseRenderTargets();
			_ReleaseSamplers();
			_ReleaseBlendStates();
			_ReleaseViewports();

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

			GameStateTask* GameStateTaskObj = new GameStateTask(_InitialGameState, GetSharedData());
			GameStateTaskObj->SetTaskName("Game State Task");
			_GameStateTask = GameStateTaskObj;

			OpaqueTask* OpaqueTaskObj = new OpaqueTask(*_Contexts[0], *_OpaqueRenderTargets, *_SamplerCollection, *_ViewportCollection, *_BlendStateCollection);
			OpaqueTaskObj->SetTaskName("Opaque Task");
			_OpaqueTask = OpaqueTaskObj;

			PrepareOpaqueTask* PrepareOpaqueTaskObj = new PrepareOpaqueTask(*OpaqueTaskObj);
			PrepareOpaqueTaskObj->SetTaskName("Prepare Opaque Task");
			_PrepareOpaqueTask = PrepareOpaqueTaskObj;

			CompositingTask* CompositingTaskObj = new CompositingTask(*_Renderer->GetMainContext(), _Contexts, ETERNAL_ARRAYSIZE(_Contexts), *_OpaqueRenderTargets, *_SamplerCollection, *_ViewportCollection, *_BlendStateCollection);
			CompositingTaskObj->SetTaskName("Compositing Task");
			_CompositingTask = CompositingTaskObj;

			SwapFrameTask* SwapFrameTaskObj = new SwapFrameTask(*_Renderer);
			SwapFrameTaskObj->SetTaskName("Swap Frame Task");
			_SwapFrameTask = SwapFrameTaskObj;
		}

		void CoreState::_ReleaseTasks()
		{
			delete _SwapFrameTask;
			_SwapFrameTask = nullptr;

			delete _CompositingTask;
			_CompositingTask = nullptr;

			delete _PrepareOpaqueTask;
			_PrepareOpaqueTask = nullptr;

			delete _OpaqueTask;
			_OpaqueTask = nullptr;

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
			for (int GraphicContextIndex = 0; GraphicContextIndex < 4; ++GraphicContextIndex)
			{
				delete _Contexts[GraphicContextIndex];
			}
		}

		void CoreState::_InitializeRenderTargets()
		{
			Format OpaqueFormat[] = {
				RGBA8888,
				RGBA8888,
				RGBA8888,
				RGBA8888,
			};
			_OpaqueRenderTargets = new RenderTargetCollection(1600, 900, ETERNAL_ARRAYSIZE(OpaqueFormat), OpaqueFormat, true);
			Format LightFormat[] = {
				RGBA8888
			};
			_LightRenderTargets = new RenderTargetCollection(1600, 900, ETERNAL_ARRAYSIZE(LightFormat), LightFormat);
		}

		void CoreState::_ReleaseRenderTargets()
		{
			delete _OpaqueRenderTargets;
			_OpaqueRenderTargets = nullptr;
			delete _LightRenderTargets;
			_LightRenderTargets = nullptr;
		}

		void CoreState::_InitializeSamplers()
		{
			_SamplerCollection = new SamplerCollection();
		}

		void CoreState::_ReleaseSamplers()
		{
			delete _SamplerCollection;
			_SamplerCollection = nullptr;
		}

		void CoreState::_InitializeViewports()
		{
			_ViewportCollection = new ViewportCollection();
		}

		void CoreState::_ReleaseViewports()
		{
			delete _ViewportCollection;
			_ViewportCollection = nullptr;
		}

		void CoreState::_InitializeBlendStates()
		{
			_BlendStateCollection = new BlendStateCollection();
		}

		void CoreState::_ReleaseBlendStates()
		{
			delete _BlendStateCollection;
			_BlendStateCollection = nullptr;
		}

		void CoreState::_InitializeRenderingLists()
		{
			_RenderingListCollection = new RenderingListCollection();
		}

		void CoreState::_ReleaseRenderingLists()
		{
			delete _RenderingListCollection;
			_RenderingListCollection = nullptr;
		}
	}
}
