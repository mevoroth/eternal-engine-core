#pragma once

#include "Core/MainInput.hpp"
#include "File/FilePath.hpp"
#include <vector>
#include <string>

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

	namespace Core
	{
		using namespace std;
		using namespace Eternal::FileSystem;
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;

		using CreateCustomRendererFunctor = Renderer* (*)(_In_ GraphicsContext& InGraphicsContext);

		class Game;

		struct SystemCreateInformation
		{
			SystemCreateInformation(_In_ GraphicsContextCreateInformation* InContextInformation);
			SystemCreateInformation(_Inout_ SystemCreateInformation&& InOutSystemCreateInformation);
			~SystemCreateInformation();

			MainInput ExecutableInput;

			GraphicsContextCreateInformation* ContextInformation		= nullptr;
			Game* GameContext											= nullptr;
			CreateCustomRendererFunctor CreateCustomRendererFunction	= nullptr;

			vector<FileSystemPath> ShaderIncludePath;
			FileSystemPath ShaderPDBPath;
			FileSystemPath FBXPath;
			FileSystemPath FBXCachePath;
			FileSystemPath TexturePath;
			FileSystemPath LevelPath;
			FileSystemPath PipelineCachePath;
			FileSystemPath MaterialPath;
			FileSystemPath AnimationPath;
			FileSystemPath SFXSoundPath;
			FileSystemPath BGMSoundPath;
			FileSystemPath ConfigurationPath;
		};

		SystemCreateInformation CreateSystemInformation(_In_ const MainInput& InMainInput);
	}
}
