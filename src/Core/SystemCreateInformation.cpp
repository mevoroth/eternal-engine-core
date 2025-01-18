#include "Core/SystemCreateInformation.hpp"
#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation::SystemCreateInformation(_In_ GraphicsContextCreateInformation* InContextInformation)
			: ContextInformation(InContextInformation)
		{
		}

		SystemCreateInformation::~SystemCreateInformation()
		{
			Graphics::DestroyGraphicsContextCreateInformation(ContextInformation);
		}
	}
}
