#pragma once

#include <vector>

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

			vector<const char*> ShaderIncludePath;
			const char* ShaderPDBPath				= nullptr;
			const char* FBXPath						= nullptr;
			const char* FBXCachePath				= nullptr;
			const char* TexturePath					= nullptr;
			const char* LevelPath					= nullptr;
			const char* PipelineCachePath			= nullptr;
			const char* MaterialPath				= nullptr;
		};
	}
}
