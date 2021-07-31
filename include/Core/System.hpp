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

	namespace GraphicsEngine
	{
		class Renderer;
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

	namespace Components
	{
		class MeshCollection;
		class Camera;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;
		using namespace Eternal::Time;
		using namespace Eternal::Platform;
		using namespace Eternal::LogSystem;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Import;
		using namespace Eternal::Parallel;
		using namespace Eternal::Tasks;
		using namespace Eternal::ImguiSystem;
		using namespace Eternal::Resources;
		using namespace Eternal::Components;

		class Game;

		static constexpr int SystemCanBeRendered = 1;
		static constexpr int SystemCanBeWritten = 0;

		struct SystemFrame
		{
			static constexpr uint32_t EstimatedGraphicsCommandsCount = 1024;
			static constexpr uint32_t EstimatedMeshesCount = 4096;

			SystemFrame();
			~SystemFrame();

			AtomicS32* SystemState = nullptr;
			ImguiContext ImguiFrameContext;
			vector<MeshCollection*> MeshCollections;
			vector<MeshCollection*> PendingMeshCollections;
			vector<GraphicsCommand*> GraphicsCommands;
			Camera* View = nullptr;
			Camera* PendingView = nullptr;
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

			inline Streaming& GetStreaming()
			{
				ETERNAL_ASSERT(_Streaming);
				return *_Streaming;
			}

			inline Renderer& GetRenderer()
			{
				ETERNAL_ASSERT(_Renderer);
				return *_Renderer;
			}

			inline Timer& GetTimer()
			{
				ETERNAL_ASSERT(_Timer);
				return *_Timer;
			}

			SystemFrame& GetGameFrame();
			SystemFrame& GetOldestGameFrame();
			SystemFrame& GetRenderFrame();
			void AdvanceRender();
			void AdvanceGame();

			void StartFrame();
			void EndFrame();
			void Update();
			void Render();

		private:
			SystemCreateInformation										_SystemCreateInformation;
			WindowsProcess												_WindowProcess;
			ParallelSystem*												_ParallelSystem		= nullptr;
			GraphicsContext*											_GraphicsContext	= nullptr;
			Imgui*														_Imgui				= nullptr;
			Timer*														_Timer				= nullptr;
			Log*														_Logs				= nullptr;
			Input*														_Input				= nullptr;
			Streaming*													_Streaming			= nullptr;
			Renderer*													_Renderer			= nullptr;

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
