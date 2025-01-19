#pragma once

#include "Core/SystemCreateInformation.hpp"
#include "Core/Game.hpp"
#include "DebugTools/Debug.hpp"
#include "File/FilePath.hpp"
#include "Graphics/Types/DeviceType.hpp"

#if ETERNAL_PLATFORM_WINDOWS
#include <Windows.h>
#else

#ifndef HINSTANCE
#define HINSTANCE	void*
#endif

#ifndef LPSTR
#define LPSTR		const char*
#endif

#endif

namespace Eternal
{
	namespace Core
	{
		struct MainInput
		{
			const char* ApplicationName	= nullptr;
			HINSTANCE hInstance			= nullptr;
			HINSTANCE hPrevInstance		= nullptr;
			LPSTR lpCmdLine				= nullptr;
			int nCmdShow				= 0;
		};

		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMaintInput);
		template<typename CustomSetupType, typename GameStateType>
		int Main(_In_ const MainInput& InMaintInput)
		{
			using namespace Eternal::FileSystem;

			Eternal::DebugTools::WaitForDebugger();

			//OPTICK_APP(InMainInput.ApplicationName);
			
			SystemCreateInformation SystemInformation = CreateSystemInformation(InMaintInput);
			SystemInformation.FBXPath			= FilePath::MakePath("assets\\fbx\\");
			SystemInformation.FBXCachePath		= FilePath::MakePath("assets\\fbx\\cache\\");
			SystemInformation.ShaderIncludePath	= { FilePath::MakePath("eternal-engine-shaders\\Shaders\\"),
													FilePath::MakePath("eternal-engine-shaders\\") };
			SystemInformation.ShaderPDBPath		= FilePath::MakePath("assets\\shaders_pdb\\");
			SystemInformation.TexturePath		= FilePath::MakePath("assets\\textures\\");
			SystemInformation.LevelPath			= FilePath::MakePath("assets\\scenes\\");
			SystemInformation.PipelineCachePath	= FilePath::MakePath("assets\\pipelines\\");
			SystemInformation.MaterialPath		= FilePath::MakePath("assets\\materials\\");

			CustomSetupType::SetupCustomSystemCreateInformation(SystemInformation);

			GameCreateInformation GameInformation(SystemInformation);

			StartGame<GameStateType> EternalGame(GameInformation);
			EternalGame.Run();

			return 0;
		}
	}
}

#if !ETERNAL_PLATFORM_WINDOWS
#undef HINSTANCE
#undef LPSTR
#endif
