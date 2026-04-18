#pragma once

#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace FileSystem
	{
		struct FileSystemPathStorage;
		using FileSystemPath = FileSystemPathStorage;
	}
	namespace Core
	{
		class Level;
	}
	namespace Resources
	{
		using namespace Eternal::Core;
		using namespace Eternal::FileSystem;

		struct LevelPayload : public Payload
		{
			Level* LoadedLevel = nullptr;
		};

		struct LevelRequest : public StreamingRequest
		{
			LevelRequest(_In_ const FileSystemPath& InPath);
		};
	}
}
