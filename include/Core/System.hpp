#pragma once

#include "Platform/WindowsProcess.hpp"
#include "Parallel/ParallelSystem.hpp"
#include "Graphics/GraphicsContext.hpp"
#include "Imgui/Imgui.hpp"
#include <array>

namespace Eternal
{
	namespace Graphics
	{
		struct GraphicsContextCreateInformation;
		class GraphicsContext;
	}

	namespace Time
	{
		class Timer;
	}

	namespace LogSystem
	{
		class Log;
	}

	namespace InputSystem
	{
		class Input;
	}

	namespace Import
	{
		class ImportTga;
		class ImportFbx;
		class ImportLevel;
	}

	namespace Tasks
	{
		class RendererTask;
		class StreamingTask;
	}

	namespace Parallel
	{
		class AtomicS32;
		class Mutex;
	}

	namespace Resources
	{
		class Streaming;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Graphics;
		using namespace Eternal::Time;
		using namespace Eternal::Platform;
		using namespace Eternal::LogSystem;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Import;
		using namespace Eternal::Parallel;
		using namespace Eternal::Tasks;
		using namespace Eternal::ImguiSystem;
		using namespace Eternal::Resources;

		class Game;

		static constexpr int SystemCanBeRendered = 1;
		static constexpr int SystemCanBeWritten = 0;

		struct SystemFrame
		{
			SystemFrame();
			~SystemFrame();

			AtomicS32* SystemState = nullptr;
			ImguiContext ImguiFrameContext;
		};

		struct SystemCreateInformation
		{
			SystemCreateInformation(_In_ GraphicsContextCreateInformation& InContextInformation)
				: ContextInformation(InContextInformation)
			{
			}

			GraphicsContextCreateInformation&	ContextInformation;

			Game* GameContext					= nullptr;

			const char* ShaderIncludePath		= nullptr;
			const char* FBXPath					= nullptr;
			const char* TexturePath				= nullptr;
			const char* LevelPath				= nullptr;
		};

		class System
		{
		public:

			System(_In_ SystemCreateInformation& InSystemCreateInformation);
			~System();

			inline GraphicsContext& GetGraphicsContext()
			{
				ETERNAL_ASSERT(_GraphicsContext);
				return *_GraphicsContext;
			}

			inline Input& GetInput()
			{
				ETERNAL_ASSERT(_Input);
				return *_Input;
			}

			inline ParallelSystem& GetParallelSystem()
			{
				ETERNAL_ASSERT(_ParallelSystem);
				return *_ParallelSystem;
			}

			inline Imgui& GetImgui()
			{
				ETERNAL_ASSERT(_Imgui);
				return *_Imgui;
			}

			inline ImportTga& GetImportTga()
			{
				ETERNAL_ASSERT(_ImportTga);
				return *_ImportTga;
			}

			inline ImportFbx& GetImportFbx()
			{
				ETERNAL_ASSERT(_ImportFbx);
				return *_ImportFbx;
			}

			inline ImportLevel& GetImportLevel()
			{
				ETERNAL_ASSERT(_ImportLevel);
				return *_ImportLevel;
			}

			inline Streaming& GetStreaming()
			{
				ETERNAL_ASSERT(_Streaming);
				return *_Streaming;
			}

			SystemFrame& GetGameFrame();
			SystemFrame& GetRenderFrame();
			void AdvanceRender();
			void AdvanceGame();

			void StartFrame();
			void EndFrame();
			void Update();

		private:
			SystemCreateInformation										_SystemCreateInformation;
			WindowsProcess												_WindowProcess;
			ParallelSystem*												_ParallelSystem		= nullptr;
			GraphicsContext*											_GraphicsContext	= nullptr;
			Imgui*														_Imgui				= nullptr;
			Timer*														_Timer				= nullptr;
			Log*														_Logs				= nullptr;
			Input*														_Input				= nullptr;
			ImportTga*													_ImportTga			= nullptr;
			ImportFbx*													_ImportFbx			= nullptr;
			ImportLevel*												_ImportLevel		= nullptr;
			Streaming*													_Streaming			= nullptr;

			//////////////////////////////////////////////////////////////////////////
			// Tasks
			RendererTask*												_RendererTask		= nullptr;
			StreamingTask*												_StreamingTask		= nullptr;

			//////////////////////////////////////////////////////////////////////////
			// System state
			array<SystemFrame, GraphicsContext::FrameBufferingCount>	_Frames;
			AtomicS32*													_RenderIndex		= nullptr;
			AtomicS32*													_GameIndex			= nullptr;
		};
	}
}
