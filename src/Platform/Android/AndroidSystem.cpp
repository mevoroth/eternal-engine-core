#if ETERNAL_PLATFORM_ANDROID

#include "Platform/Android/AndroidSystem.hpp"
#include "Graphics/GraphicsContextFactory.hpp"
#include "Android/AndroidGraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Graphics;

		AndroidSystem::AndroidSystem(_In_ SystemCreateInformation& InSystemCreateInformation)
			: System(InSystemCreateInformation)
		{

			_GraphicsContext = CreateGraphicsContext(*static_cast<const AndroidGraphicsContextCreateInformation*>(InSystemCreateInformation.ContextInformation));

		}
	}
}

#endif
