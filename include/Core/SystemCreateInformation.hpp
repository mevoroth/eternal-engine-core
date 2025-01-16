#pragma once

#include <vector>
#include <string>

namespace Eternal
{
	namespace Graphics
	{
		struct GraphicsContextCreateInformation;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Graphics;

		class Game;

		struct SystemCreateInformation
		{
			SystemCreateInformation(_In_ GraphicsContextCreateInformation& InContextInformation)
				: ContextInformation(InContextInformation)
			{
			}

			GraphicsContextCreateInformation& ContextInformation;

			Game* GameContext						= nullptr;

			vector<string> ShaderIncludePath;
			string ShaderPDBPath;
			string FBXPath;
			string FBXCachePath;
			string TexturePath;
			string LevelPath;
			string PipelineCachePath;
			string MaterialPath;
		};
	}
}
