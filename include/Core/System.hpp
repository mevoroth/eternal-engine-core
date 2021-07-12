#pragma once

#include "Platform/WindowsProcess.hpp"
#include "Parallel/ParallelSystem.hpp"

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
	}

	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Time;
		using namespace Eternal::Platform;
		using namespace Eternal::LogSystem;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Import;
		using namespace Eternal::Parallel;

		class Game;

		struct SystemCreateInformation
		{
			SystemCreateInformation(_In_ GraphicsContextCreateInformation& InContextInformation)
				: ContextInformation(InContextInformation)
			{
			}

			Game*								GameContext;
			GraphicsContextCreateInformation&	ContextInformation;

			const char* ShaderIncludePath		= nullptr;
			const char* FBXPath					= nullptr;
			const char* TexturePath				= nullptr;
			const char* SavePath				= nullptr;
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

			Input& GetInput()
			{
				ETERNAL_ASSERT(_Input);
				return *_Input;
			}

		private:
			SystemCreateInformation	_SystemCreateInformation;
			WindowsProcess			_WindowProcess;
			ParallelSystem*			_ParallelSystem		= nullptr;
			GraphicsContext*		_GraphicsContext	= nullptr;
			Timer*					_Timer				= nullptr;
			Log*					_Logs				= nullptr;
			Input*					_Input				= nullptr;
			ImportTga*				_ImportTga			= nullptr;
		};
	}
}
