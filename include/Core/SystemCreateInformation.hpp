#pragma once

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
		using namespace Eternal::Graphics;
		using namespace Eternal::GraphicsEngine;

		using CreateCustomRendererFunctor = Renderer* (*)(_In_ GraphicsContext& InGraphicsContext);

		class Game;

		struct SystemCreateInformation
		{
			SystemCreateInformation(_In_ GraphicsContextCreateInformation* InContextInformation);
			~SystemCreateInformation();

			GraphicsContextCreateInformation* ContextInformation		= nullptr;
			Game* GameContext											= nullptr;
			CreateCustomRendererFunctor CreateCustomRendererFunction	= nullptr;

			vector<string> ShaderIncludePath;
			string ShaderPDBPath;
			string FBXPath;
			string FBXCachePath;
			string TexturePath;
			string LevelPath;
			string PipelineCachePath;
			string MaterialPath;
			string AnimationPath;
			string SFXSoundPath;
			string BGMSoundPath;
		};
	}
}
