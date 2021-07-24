#include "Core/System.hpp"
#include "Core/Game.hpp"
#include "Graphics/GraphicsInclude.hpp"
#include "GraphicData/SceneResources.hpp"
#include "File/FilePath.hpp"
#include "Time/Timer.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/LogFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Task/Core/RendererTask.hpp"
#include "Task/Core/StreamingTask.hpp"
#include "Parallel/AtomicS32.hpp"
#include "Parallel/AtomicS32Factory.hpp"
#include "Parallel/Sleep.hpp"
#include "Resources/Streaming.hpp"
#include "Resources/LevelLoader.hpp"
#include "Optick/Optick.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::FileSystem;
		using namespace Eternal::LogSystem;
		using namespace Eternal::Platform;
		using namespace Eternal::Parallel;

		System::System(_In_ SystemCreateInformation& InSystemCreateInformation)
			: _SystemCreateInformation(InSystemCreateInformation)
			, _GameIndex(CreateAtomicS32())
			, _RenderIndex(CreateAtomicS32())
		{
			FilePath::Register(InSystemCreateInformation.ShaderIncludePath,	FileType::FILE_TYPE_SHADERS);
			FilePath::Register(InSystemCreateInformation.LevelPath,			FileType::FILE_TYPE_LEVELS);
			FilePath::Register(InSystemCreateInformation.FBXPath,			FileType::FILE_TYPE_MESHES);
			FilePath::Register(InSystemCreateInformation.TexturePath,		FileType::FILE_TYPE_TEXTURES);

			_Timer				= CreateTimer(TimeType::TIME_TYPE_WIN);
			_Logs				= CreateMultiChannelLog({ LogType::LOG_TYPE_CONSOLE/*, LogType::LOG_TYPE_FILE*/ });
			_Input				= CreateMultiInput({ InputType::INPUT_TYPE_WIN, InputType::INPUT_TYPE_XINPUT });

			WindowsProcess::SetInputHandler(_Input);
			WindowsProcess::SetIsRunning(&InSystemCreateInformation.GameContext->_Running);
			
			_GraphicsContext	= CreateGraphicsContext(InSystemCreateInformation.ContextInformation);
			_Imgui				= new Imgui(*_GraphicsContext, _Input);
			
			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Frames[FrameIndex].ImguiFrameContext = _Imgui->CreateContext(*_GraphicsContext);

			_Streaming			= new Streaming();
			_Streaming->RegisterLoader(AssetType::ASSET_TYPE_LEVEL, new LevelLoader());

			_SceneResources		= new SceneResources(*_GraphicsContext);

			TaskCreateInformation RendererCreateInformation(*this, "RendererTask");
			_RendererTask		= new RendererTask(RendererCreateInformation);

			TaskCreateInformation StreamingCreateInformation(*this, "StreamingTask");
			_StreamingTask		= new StreamingTask(StreamingCreateInformation);

			ParallelSystemCreateInformation ParallelSystemInformation(
				GraphicsContext::FrameBufferingCount,
				{ _RendererTask, _StreamingTask }
			);
			_ParallelSystem		= new ParallelSystem(ParallelSystemInformation);

			Optick::OptickStart(*_GraphicsContext);
		}

		System::~System()
		{
			Optick::OptickStop();

			delete _ParallelSystem;
			_ParallelSystem = nullptr;

			delete _StreamingTask;
			_StreamingTask = nullptr;

			delete _RendererTask;
			_RendererTask = nullptr;

			delete _SceneResources;
			_SceneResources = nullptr;

			delete _Streaming;
			_Streaming = nullptr;

			for (uint32_t FrameIndex = 0; FrameIndex < GraphicsContext::FrameBufferingCount; ++FrameIndex)
				_Imgui->DestroyContext(_Frames[FrameIndex].ImguiFrameContext);

			delete _Imgui;
			_Imgui = nullptr;

			DestroyGraphicsContext(_GraphicsContext);

			DeleteInput(_Input);
			DeleteLog(_Logs);
			DestroyTimer(_Timer);
		}

		void System::StartFrame()
		{
			ETERNAL_PROFILER(BASIC)();
			while (GetGameFrame().SystemState->Load() != SystemCanBeWritten)
			{
				Sleep(10);
			}
			GetParallelSystem().StartFrame();
			GetImgui().SetContext(GetGameFrame().ImguiFrameContext);
			GetImgui().Begin();
			GetStreaming().GatherPayloads();
		}

		void System::Update()
		{
			ETERNAL_PROFILER(BASIC)();
			WindowsProcess::ExecuteMessageLoop();
			_Timer->Update();
			_Input->Update();
		}

		void System::EndFrame()
		{
			ETERNAL_PROFILER(BASIC)();
			GetStreaming().CommitRequests();
			GetImgui().End();
			GetParallelSystem().EndFrame();
			GetGameFrame().SystemState->Store(SystemCanBeRendered);
			AdvanceGame();
		}

		//////////////////////////////////////////////////////////////////////////
		// System Frame

		SystemFrame& System::GetGameFrame()
		{
			ETERNAL_ASSERT(_GameIndex);
			return _Frames[_GameIndex->Load()];
		}

		SystemFrame& System::GetRenderFrame()
		{
			ETERNAL_ASSERT(_RenderIndex);
			return _Frames[_RenderIndex->Load()];
		}

		void System::AdvanceRender()
		{
			_RenderIndex->Store((_RenderIndex->Load() + 1) % _Frames.size());
		}

		void System::AdvanceGame()
		{
			_GameIndex->Store((_GameIndex->Load() + 1) % _Frames.size());
		}

		SystemFrame::SystemFrame()
			: SystemState(CreateAtomicS32())
		{
			GraphicsCommands.reserve(EstimatedGraphicsCommandsCount);
		}

		SystemFrame::~SystemFrame()
		{
			DestroyAtomicS32(SystemState);
		}
	}
}
