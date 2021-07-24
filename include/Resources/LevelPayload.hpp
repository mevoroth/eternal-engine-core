#pragma once

#include "Resources/Streaming.hpp"

namespace Eternal
{
	namespace Core
	{
		class Level;
	}
	namespace Resources
	{
		using namespace Eternal::Core;

		struct LevelPayload : public Payload
		{
			Level* LoadedLevel = nullptr;
		};

		struct LevelRequest : public StreamingRequest
		{
			LevelRequest(_In_ const string& InPath);
		};
	}
}
