#include "Core/System.hpp"
#include "Graphics/GraphicsInclude.hpp"

#include "File/FilePath.hpp"
#include "Time/Timer.hpp"
#include "Time/TimeFactory.hpp"
#include "Log/LogFactory.hpp"
#include "Input/InputFactory.hpp"
#include "Import/tga/ImportTga.hpp"
#include "Core/Game.hpp"
#include "Optick/Optick.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::FileSystem;
		using namespace Eternal::LogSystem;
		using namespace Eternal::Platform;

		System::System(_In_ SystemCreateInformation& InSystemCreateInformation)
			: _SystemCreateInformation(InSystemCreateInformation)
		{
			FilePath::Register(InSystemCreateInformation.ShaderIncludePath,	FileType::FILE_TYPE_SHADERS);
			FilePath::Register(InSystemCreateInformation.TexturePath,		FileType::FILE_TYPE_TEXTURES);

			_Timer				= CreateTimer(TimeType::TIME_TYPE_WIN);
			_Logs				= CreateMultiChannelLog({ LogType::LOG_TYPE_CONSOLE, LogType::LOG_TYPE_FILE });
			_Input				= CreateMultiInput({ InputType::INPUT_TYPE_WIN, InputType::INPUT_TYPE_XINPUT });

			WindowsProcess::SetInputHandler(_Input);
			WindowsProcess::SetIsRunning(&InSystemCreateInformation.GameContext->_Running);
			
			_GraphicsContext	= CreateGraphicsContext(InSystemCreateInformation.ContextInformation);

			ParallelSystemCreateInformation ParallelSystemInformation;
			ParallelSystemInformation.FrameCount = GraphicsContext::FrameBufferingCount;
			_ParallelSystem		= new ParallelSystem(ParallelSystemInformation);

			Optick::OptickStart(*_GraphicsContext);
		}

		System::~System()
		{
			Optick::OptickStop();

			DestroyGraphicsContext(_GraphicsContext);

			DeleteInput(_Input);
			DeleteLog(_Logs);
			DestroyTimer(_Timer);
		}
	}
}
