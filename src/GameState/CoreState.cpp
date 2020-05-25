#include "GameState/CoreState.hpp"

#include "GraphicsSettings.hpp"
#include "Platform/WindowsProcess.hpp"
#include "Window/Window.hpp"
#include "NextGenGraphics/Types/DeviceType.hpp"
#include "NextGenGraphics/DeviceFactory.hpp"
#include "NextGenGraphics/Device.hpp"
#include "NextGenGraphics/ShaderFactory.hpp"
//#include "Graphics/RendererFactory.hpp"
//#include "Graphics/Renderer.hpp"
//#include "Graphics/ShaderFactory.hpp"
//#include "Graphics/ShaderFactoryFactory.hpp"
#include "Graphics/ContextFactory.hpp"
#include "Graphics/CommandQueueFactory.hpp"
#include "Graphics/SwapChain.hpp"
#include "Graphics/SwapChainFactory.hpp"
#include "Graphics/Format.hpp"
#include "Graphics/Fence.hpp"
#include "Graphics/FenceFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/LogFactory.hpp"
#include "Parallel/CpuCoreCount.hpp"
#include "Parallel/TaskManager.hpp"
#include "Task/ControlsTask.hpp"
#include "Task/ImguiBeginTask.hpp"
#include "Task/ImguiEndTask.hpp"
#include "Task/TimeTask.hpp"
#include "Task/Core/UpdateComponentTask.hpp"
#include "Task/Core/GameStateTask.hpp"
#include "Task/Core/CommandsTask.hpp"
#include "Task/Graphics/InitFrameTask.hpp"
#include "Task/Graphics/SwapFrameTask.hpp"
//#include "Task/Graphics/RenderObjectsTask.hpp"
//#include "Task/Graphics/LightingTask.hpp"
//#include "Task/Graphics/CompositingTask.hpp"
#include "Task/NextGenGraphics/RenderObjectsTask.hpp"
#include "Task/NextGenGraphics/DebugRenderTask.hpp"
#include "Task/NextGenGraphics/FinalizeFrameTask.hpp"
#include "Resources/Pool.hpp"
#include "Resources/TextureFactory.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/TransformComponent.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/LightComponent.hpp"
#include "GraphicData/GraphicResources.hpp"
#include "GraphicData/CommandQueues.hpp"
//#include "GraphicData/ContextCollection.hpp"
//#include "GraphicData/RenderTargetCollection.hpp"
//#include "GraphicData/SamplerCollection.hpp"
//#include "GraphicData/ViewportCollection.hpp"
//#include "GraphicData/BlendStateCollection.hpp"
#include "GraphicData/RenderingListCollection.hpp"
#include "GraphicData/GraphicTaskConfigCollection.hpp"
#include "Import/fbx/ImportFbx.hpp"
#include "Import/tga/ImportTga.hpp"
#include "SaveSystem/SaveSystem.hpp"
#include "GameData/GameDatas.hpp"
#include "File/FilePath.hpp"

#ifdef ETERNAL_DEBUG
#include "Task/Tools/AutoRecompileShaderTask.hpp"
#include "include/RenderDoc.h"
#endif

using namespace Eternal::Resources;
using namespace Eternal::Core;

extern Pool<TransformComponent>*	g_TransformComponentPool;
extern Pool<CameraComponent, 4>*	g_CameraComponentPool;
extern Pool<LightComponent, 128>*	g_LightComponentPool;

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
		using namespace Eternal::Parallel;
		using namespace Eternal::File;

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
			FilePath::Register(_Settings.ShaderIncludePath,	FileType::SHADERS);
			FilePath::Register(_Settings.SavePath,			FileType::SAVES);
			FilePath::Register(_Settings.TexturePath,		FileType::TEXTURES);

			_Time = CreateTime(TimeType::WIN);

			_FileLog	= CreateLog(FILE, "Eternal");
			_ConsoleLog	= CreateLog(CONSOLE, "Eternal");
			Eternal::Log::Log* Logs[] =
			{
				//_FileLog,
				_ConsoleLog
			};
			_MultiChannelLog = CreateMultiChannelLog(Logs, ETERNAL_ARRAYSIZE(Logs));

			_PadInput		= CreateInput(XINPUT);
			_KeyboardInput	= CreateInput(InputType::WIN);
			Eternal::Input::Input* Inputs[] =
			{
				_PadInput,
				_KeyboardInput
			};
			_MultiInput = CreateMultiInput(Inputs, ETERNAL_ARRAYSIZE(Inputs));

			WindowsProcess::SetInputHandler(_KeyboardInput);
			_WindowsProcess		= new WindowsProcess();

			_Window				= new Window(_hInstance, _nCmdShow, "ReShield", "EternalClass", 1280, 720);
			_Window->Create(WindowsProcess::WindowProc);

#ifdef ETERNAL_DEBUG
			RenderDoc::Initialize();
#endif

			//*
			_Device				= CreateDevice(DeviceType::D3D12, *_Window);
			/*/
			_Device				= CreateDevice(VULKAN, *_Window);
			/**/
			_ShaderFactory		= new ShaderFactory();
			_GraphicResources	= new GraphicResources();
			_GraphicResources->Initialize(*_Device);

			//_MainCommandQueue	= CreateCommandQueue(*_Device, FRAME_LAG);
			_MainCommandQueue	= _GraphicResources->GetCommandQueues()->Get(COMMAND_QUEUE_GRAPHIC);
			_SwapChain			= CreateSwapChain(*_Device, *_Window, *_MainCommandQueue->Queue);
			_FrameFence			= CreateFence(*_Device);
			_MainCommandQueue->QueueFence	=	_FrameFence; // HACK

			for (uint32_t ContextIndex = 0; ContextIndex < ETERNAL_ARRAYSIZE(GetSharedData()->GfxContexts); ++ContextIndex)
			{
				GetSharedData()->GfxContexts[ContextIndex] = CreateContext(*_Device);
			}

			//_Device = CreateDevice(WINDOWS, WindowsProcess::WindowProc, _hInstance, _nCmdShow, "ReShield", "EternalClass");
			//_Renderer = CreateRenderer(RENDERER_D3D11);

			//_InitializeGraphicContexts();

			//_ShaderFactory = CreateShaderFactory(SHADER_FACTORY_D3D11);

			_ImportFbx = new ImportFbx();
			_ImportFbx->RegisterPath(_Settings.FBXPath);

			_ImportTga = new ImportTga();

			//_TextureFactory = new TextureFactory();
			//_TextureFactory->RegisterTexturePath(_Settings.TexturePath);

			_SaveSystem = new Eternal::SaveSystem::SaveSystem();
			//_SaveSystem->RegisterSavePath(_Settings.SavePath);

			_TaskManager = new TaskManager();

			_InitializeGraphicTaskConfigs();
			//_InitializeViewports();
			//_InitializeBlendStates();
			//_InitializeSamplers();
			//_InitializeRenderTargets();
			_InitializePools();
			_InitializeRenderingLists();
			_InitializeTasks();

			_GameDatas = new GameDatas();
			_SaveSystem->SetGameDataLoader(_GameDatas);

			_ScheduleTasks();
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
			//_ReleaseRenderTargets();
			//_ReleaseSamplers();
			//_ReleaseBlendStates();
			//_ReleaseViewports();
			_ReleaseGraphicTaskConfigs();

			delete _TaskManager;
			_TaskManager = nullptr;

			//delete _ShaderFactory;
			//_ShaderFactory = nullptr;

			//delete _Renderer;
			//_Renderer = nullptr;

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

			DeleteLog(_MultiChannelLog);
			DeleteLog(_ConsoleLog);
			DeleteLog(_FileLog);

			delete _Time;
			_Time = nullptr;

#ifdef ETERNAL_DEBUG
			RenderDoc::Release();
#endif
		}

		void CoreState::_InitializePools()
		{
			TransformComponent::Initialize();
			CameraComponent::Initialize();
			LightComponent::Initialize();
		}

		void CoreState::_ReleasePools()
		{
			TransformComponent::Release();
			CameraComponent::Release();
			LightComponent::Release();
		}

		void CoreState::_InitializeTasks()
		{
			ControlsTask* ControlsTaskObj = new ControlsTask();
			ControlsTaskObj->SetTaskName("Controls Task");
			ControlsTaskObj->RegisterInput(_PadInput);
			ControlsTaskObj->RegisterInput(_KeyboardInput);
			_ControlsTask = ControlsTaskObj;

			//ImguiBeginTask* ImguiBeginTaskObj = new ImguiBeginTask(_Device->GetWindow(), Device::WIDTH, Device::HEIGHT);
			//ImguiBeginTaskObj->SetTaskName("Imgui Begin Task");
			//ImguiBeginTaskObj->SetupKeyboard(_KeyboardInput);
			//_ImguiBeginTask = ImguiBeginTaskObj;

			//ImguiEndTask* ImguiEndTaskObj = new ImguiEndTask(*_ContextCollection, *_SamplerCollection, *_ViewportCollection);
			//ImguiEndTaskObj->SetTaskName("Imgui End Task");
			//ImguiEndTaskObj->SetRenderTarget(_Renderer->GetBackBuffer());
			//_ImguiEndTask = ImguiEndTaskObj;

			TimeTask* TimeTaskObj = new TimeTask(_Time);
			TimeTaskObj->SetTaskName("Time Task");
			_TimeTask = TimeTaskObj;

			CommandsTask* CommandsTaskObj = new CommandsTask(*_Device, _GraphicResources, GetSharedData());
			CommandsTaskObj->SetTaskName("Commands Taks");
			_CommandsTask = CommandsTaskObj;

			UpdateComponentTask< Pool<TransformComponent> >* UpdateTransformComponentTaskObj = new UpdateComponentTask< Pool<TransformComponent> >(_Time, g_TransformComponentPool);
			UpdateTransformComponentTaskObj->SetTaskName("Update Transform Pool Task");
			_UpdateTransformComponentTask = UpdateTransformComponentTaskObj;

			UpdateComponentTask< Pool<CameraComponent, 4> >* UpdateCameraComponentTaskObj = new UpdateComponentTask< Pool<CameraComponent, 4> >(_Time, g_CameraComponentPool);
			UpdateCameraComponentTaskObj->SetTaskName("Update Camera Pool Task");
			_UpdateCameraComponentTask = UpdateCameraComponentTaskObj;

			UpdateComponentTask< Pool<LightComponent, 128> >* UpdateLightComponentTaskObj = new UpdateComponentTask< Pool<LightComponent, 128> >(_Time, g_LightComponentPool);
			UpdateLightComponentTaskObj->SetTaskName("Update Light Pool Task");
			_UpdateLightComponentTask = UpdateLightComponentTaskObj;

			GameStateTask* GameStateTaskObj = new GameStateTask(_InitialGameState, GetSharedData());
			GameStateTaskObj->SetTaskName("Game State Task");
			_GameStateTask = GameStateTaskObj;

			//RenderTargetCollection* RenderTargetCollections[] =
			//{ 
			//	_OpaqueRenderTargets,
			//	_LightRenderTargets,
			//	_ShadowRenderTargets
			//};
			InitFrameTask* InitFrameTaskObj = new InitFrameTask(*_Device, *_SwapChain, *_FrameFence, GetSharedData());
			InitFrameTaskObj->SetTaskName("Init Frame Task");
			_InitFrameTask = InitFrameTaskObj;
			
			//RenderObjectsTask* OpaqueTaskObj = new RenderOpaqueObjectsTask(_GraphictaskConfigCollection->GetGraphicTaskConfig(GraphicTaskConfigCollection::OPAQUE_TASK),
			//	*_ContextCollection, *_OpaqueRenderTargets, *_SamplerCollection, *_ViewportCollection, *_BlendStateCollection, GetSharedData());
			//OpaqueTaskObj->SetTaskName("Opaque Task (Render Objects)");
			//_OpaqueTask = OpaqueTaskObj;

			//RenderObjectsTask* ShadowTaskObj = new RenderObjectsShadowTask(_GraphictaskConfigCollection->GetGraphicTaskConfig(GraphicTaskConfigCollection::SHADOW_TASK),
			//	*_ContextCollection, *_ShadowRenderTargets, *_SamplerCollection, *_ViewportCollection, *_BlendStateCollection, GetSharedData());
			//ShadowTaskObj->SetTaskName("Shadow Task (Render Objects)");
			//_ShadowTask = ShadowTaskObj;

			//LightingTask* LightingTaskObj = new LightingTask(*_ContextCollection, *_OpaqueRenderTargets, *_ShadowRenderTargets, *_LightRenderTargets, *_SamplerCollection, *_ViewportCollection, GetSharedData());
			//LightingTaskObj->SetTaskName("Lighting Task");
			//_LightingTask = LightingTaskObj;

			//CompositingTask* CompositingTaskObj = new CompositingTask(*_ContextCollection, *_OpaqueRenderTargets, *_LightRenderTargets, *_SamplerCollection, *_ViewportCollection, *_BlendStateCollection);
			//CompositingTaskObj->SetTaskName("Compositing Task");
			//_CompositingTask = CompositingTaskObj;

#pragma region NextGenGraphics
			RenderOpaqueObjectsTask* OpaqueTaskObj = new RenderOpaqueObjectsTask(*_Device, _GraphicResources, GetSharedData());
			OpaqueTaskObj->SetTaskName("Render Object Task (Opaque Task)");
			//_OpaqueTask = OpaqueTaskObj;

			DebugRenderTask* DebugRenderTaskObj = new DebugRenderTask(*_Device, *_SwapChain, _GraphicResources, GetSharedData());
			DebugRenderTaskObj->SetTaskName("Debug Render Task");
			_OpaqueTask = DebugRenderTaskObj;

			FinalizeFrameTask* FinalizeFrameTaskObj = new FinalizeFrameTask(*_Device, *_SwapChain, *_FrameFence, _GraphicResources, GetSharedData());
			FinalizeFrameTaskObj->SetTaskName("Finalize Frame Task");
			_FinalizeFrameTask = FinalizeFrameTaskObj;
#pragma endregion NextGenGraphics

			AutoRecompileShaderTask* AutoRecompileShaderTaskObj = new AutoRecompileShaderTask();
			AutoRecompileShaderTaskObj->SetTaskName("Auto Recompile Shader Task");
			AutoRecompileShaderTaskObj->SetFrameConstraint(false);
			_AutoRecompileShaderTask = AutoRecompileShaderTaskObj;
		}

		void CoreState::_ScheduleTasks()
		{
			TaskManager& Scheduler = *_TaskManager;
			Scheduler().PushTask(_InitFrameTask);
			Scheduler().PushTask(_AutoRecompileShaderTask);
			Scheduler().PushTask(_ControlsTask);
			Scheduler().PushTask(_TimeTask);
			Scheduler().PushTask(_CommandsTask, _InitFrameTask);
			{
				Scheduler().PushTask(_UpdateTransformComponentTask, _ControlsTask);
				Scheduler().PushTask(_UpdateTransformComponentTask, _TimeTask);
				Scheduler().PushTask(_UpdateTransformComponentTask, _CommandsTask);
			}
			Scheduler().PushTask(_UpdateCameraComponentTask, _UpdateTransformComponentTask);
			Scheduler().PushTask(_UpdateLightComponentTask, _UpdateTransformComponentTask);
			//{
			//	Scheduler().PushTask(_ImguiBeginTask, _ControlsTask);
			//	Scheduler().PushTask(_ImguiBeginTask, _TimeTask);
			//}
			{
				Scheduler().PushTask(_GameStateTask, _UpdateCameraComponentTask);
				//Scheduler().PushTask(_GameStateTask, _ImguiBeginTask);
			}
			//{
			//	Scheduler().PushTask(_OpaqueTask, _InitFrameTask);
			//	Scheduler().PushTask(_OpaqueTask, _GameStateTask);
			//}
			//{
			//	Scheduler().PushTask(_ShadowTask, _UpdateLightComponentTask);
			//	Scheduler().PushTask(_ShadowTask, _GameStateTask);
			//}
			//Scheduler().PushTask(_LightingTask, _OpaqueTask);
			//Scheduler().PushTask(_CompositingTask, _LightingTask);
			//Scheduler().PushTask(_ImguiEndTask, _CompositingTask);
			//Scheduler().PushTask(_SwapFrameTask, _ImguiEndTask);

#pragma region NextGenGraphics
			Scheduler().PushTask(_OpaqueTask, _GameStateTask);
			Scheduler().PushTask(_FinalizeFrameTask, _OpaqueTask);
#pragma endregion NextGenGraphics
		}

		void CoreState::_ReleaseTasks()
		{
			delete _FinalizeFrameTask;
			_FinalizeFrameTask = nullptr;

			delete _LightingTask;
			_LightingTask = nullptr;

			delete _OpaqueTask;
			_OpaqueTask = nullptr;

			delete _ShadowTask;
			_ShadowTask = nullptr;

			delete _InitFrameTask;
			_InitFrameTask = nullptr;

			delete _GameStateTask;
			_GameStateTask = nullptr;

			delete _UpdateCameraComponentTask;
			_UpdateCameraComponentTask = nullptr;

			delete _UpdateTransformComponentTask;
			_UpdateTransformComponentTask = nullptr;

			delete _TimeTask;
			_TimeTask = nullptr;

			delete _ImguiEndTask;
			_ImguiEndTask = nullptr;

			delete _ImguiBeginTask;
			_ImguiBeginTask = nullptr;

			delete _ControlsTask;
			_ControlsTask = nullptr;
		}

		//void CoreState::_InitializeGraphicContexts()
		//{
		//	_ContextCollection = new ContextCollection(*_Renderer, (int)CpuCoreCount());
		//}

		//void CoreState::_ReleaseGraphicContexts()
		//{
		//	delete _ContextCollection;
		//	_ContextCollection = nullptr;
		//}
		
		void CoreState::_InitializeGraphicTaskConfigs()
		{
			_GraphictaskConfigCollection = new GraphicTaskConfigCollection();
		}

		void CoreState::_ReleaseGraphicTaskConfigs()
		{
			delete _GraphictaskConfigCollection;
			_GraphictaskConfigCollection = nullptr;
		}
		
		//void CoreState::_InitializeRenderTargets()
		//{
		//	Format OpaqueFormat[] = {
		//		FORMAT_BGRA8888,	// Diffuse
		//		FORMAT_BGRA8888,	// Specular
		//		FORMAT_BGRA8888,	// Emissive
		//		FORMAT_BGRA8888,	// Normal
		//		FORMAT_BGRA8888,	// Roughness
		//		FORMAT_R32,			// W
		//		FORMAT_RGBA32323232	// Debug: World position
		//	};
		//	_OpaqueRenderTargets = new RenderTargetCollection(1600, 900, ETERNAL_ARRAYSIZE(OpaqueFormat), OpaqueFormat, true);
		//	Format LightFormat[] = {
		//		RGBA8888
		//	};
		//	_LightRenderTargets = new RenderTargetCollection(1600, 900, ETERNAL_ARRAYSIZE(LightFormat), LightFormat);
		//	//_ShadowRenderTargets = new RenderTargetCollection(2048, 2048);
		//	Format ShadowFormat[] = {
		//		R32
		//	};
		//	_ShadowRenderTargets = new RenderTargetCollection(2048, 2048, ETERNAL_ARRAYSIZE(ShadowFormat), ShadowFormat, true);
		//}

		//void CoreState::_ReleaseRenderTargets()
		//{
		//	delete _OpaqueRenderTargets;
		//	_OpaqueRenderTargets = nullptr;
		//	delete _LightRenderTargets;
		//	_LightRenderTargets = nullptr;
		//	delete _ShadowRenderTargets;
		//	_ShadowRenderTargets = nullptr;
		//}

		//void CoreState::_InitializeSamplers()
		//{
		//	_SamplerCollection = new SamplerCollection();
		//}

		//void CoreState::_ReleaseSamplers()
		//{
		//	delete _SamplerCollection;
		//	_SamplerCollection = nullptr;
		//}

		//void CoreState::_InitializeViewports()
		//{
		//	_ViewportCollection = new ViewportCollection();
		//}

		//void CoreState::_ReleaseViewports()
		//{
		//	delete _ViewportCollection;
		//	_ViewportCollection = nullptr;
		//}

		//void CoreState::_InitializeBlendStates()
		//{
		//	_BlendStateCollection = new BlendStateCollection();
		//}

		//void CoreState::_ReleaseBlendStates()
		//{
		//	delete _BlendStateCollection;
		//	_BlendStateCollection = nullptr;
		//}

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
