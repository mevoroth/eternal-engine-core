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
		class Context;
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
	namespace Log
	{
		class Log;
	}
	namespace Import
	{
		class ImportFbx;
	}
	namespace GraphicData
	{
		class RenderTargetCollection;
		class SamplerCollection;
		class ViewportCollection;
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
		using namespace Eternal::Import;
		using namespace Eternal::GraphicData;

		class CoreState : public GameState
		{
		public:
			class CoreStateSettings
			{
			public:
				const char* ShaderIncludePath = nullptr;
				const char* FBXIncludePath = nullptr;
			};

			CoreState(_In_ const CoreStateSettings& Settings, _In_ HINSTANCE hInstance, _In_ int nCmdShow, _In_ GameState* InitialGameState);

			virtual void Begin() override;
			virtual void Update() override;
			virtual GameState* NextState() override;
			virtual void End() override;

		private:
			CoreStateSettings _Settings;

			Eternal::Time::Time* _Time = nullptr;

			Eternal::Log::Log* _FileLog = nullptr;
			Eternal::Log::Log* _ConsoleLog = nullptr;
			Eternal::Log::Log* _MultiChannelLog = nullptr;

			WindowsProcess* _WindowsProcess = nullptr;

			Device* _Device = nullptr;
			HINSTANCE _hInstance = nullptr;
			int _nCmdShow = 0;

			Renderer* _Renderer = nullptr;

			ShaderFactory* _ShaderFactory = nullptr;

			TaskManager* _TaskManager = nullptr;

			Eternal::Input::Input* _KeyboardInput = nullptr;
			Eternal::Input::Input* _PadInput = nullptr;
			Eternal::Input::Input* _MultiInput = nullptr;

			GameState* _InitialGameState = nullptr;

			Task* _ControlsTask = nullptr;
			Task* _ImguiBeginTask = nullptr;
			Task* _ImguiEndTask = nullptr;
			Task* _TimeTask = nullptr;
			Task* _UpdateComponentTask = nullptr;
			Task* _GameStateTask = nullptr;
			Task* _PrepareSolidTask = nullptr;
			Task* _SolidGBufferTask = nullptr;
			Task* _CompositingTask = nullptr;
			Task* _SwapFrameTask = nullptr;

			ImportFbx* _ImportFbx = nullptr;

			Context* _Contexts[4];
			SamplerCollection* _SamplerCollection = nullptr;
			RenderTargetCollection* _RenderTargetCollection = nullptr;
			ViewportCollection* _ViewportCollection = nullptr;

			void _InitializePools();
			void _ReleasePools();
			void _InitializeTasks();
			void _ReleaseTasks();
			void _InitializeGraphicContexts();
			void _ReleaseGraphicContexts();
			void _InitializeRenderTargets();
			void _ReleaseRenderTargets();
			void _InitializeSamplers();
			void _ReleaseSamplers();
			void _InitializeViewports();
			void _ReleaseViewports();
		};
	}
}

#endif
