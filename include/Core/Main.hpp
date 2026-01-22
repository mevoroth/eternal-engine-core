#pragma once

#include "Core/MainInput.hpp"
#include "Core/Game.hpp"
#include "Core/SystemCreateInformation.hpp"
#include "DebugTools/Debug.hpp"
#include "File/FilePath.hpp"
#include "Graphics/Types/DeviceType.hpp"

namespace Eternal
{
	namespace Core
	{
		template<typename CustomSetupType, typename GameStateType>
		int Main(_In_ const MainInput& InMainInput)
		{
			using namespace Eternal::FileSystem;

			//Eternal::DebugTools::WaitForDebugger();

			//OPTICK_APP(InMainInput.ApplicationName);
			
			SystemCreateInformation SystemInformation = CreateSystemInformation(InMainInput);
			SystemInformation.ExecutableInput	= std::move(InMainInput);
			SystemInformation.FBXPath			= FilePath::MakePath("assets\\fbx\\");
			SystemInformation.FBXCachePath		= FilePath::MakePath("assets\\fbx\\cache\\");
			SystemInformation.ShaderIncludePath	= { FilePath::MakePath("eternal-engine-shaders\\Shaders\\"),
													FilePath::MakePath("eternal-engine-shaders\\") };
			SystemInformation.ShaderPDBPath		= FilePath::MakePath("assets\\shaders_pdb\\");
			SystemInformation.TexturePath		= FilePath::MakePath("assets\\textures\\");
			SystemInformation.LevelPath			= FilePath::MakePath("assets\\scenes\\");
			SystemInformation.PipelineCachePath	= FilePath::MakePath("assets\\pipelines\\");
			SystemInformation.MaterialPath		= FilePath::MakePath("assets\\materials\\");
			SystemInformation.AnimationPath		= FilePath::MakePath("assets\\animations\\");
			SystemInformation.SFXSoundPath		= FilePath::MakePath("assets\\sounds\\sfx\\");
			SystemInformation.BGMSoundPath		= FilePath::MakePath("assets\\sounds\\bgm\\");

			CustomSetupType::SetupCustomSystemCreateInformation(SystemInformation);

			GameCreateInformation GameInformation(std::move(SystemInformation));

			StartGame<GameStateType>* EternalGame = new StartGame<GameStateType>(std::move(GameInformation));
			EternalGame->RunGame();

			return 0;
		}
	}
}
