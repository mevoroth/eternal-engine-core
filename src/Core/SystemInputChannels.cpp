#if !ETERNAL_PLATFORM_ANDROID

#include "Input/InputFactory.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::InputSystem;

		std::vector<InputType> System::InputChannels = { InputType::INPUT_TYPE_KEYBOARD, InputType::INPUT_TYPE_DEFAULT_PAD, InputType::INPUT_TYPE_MOUSE };
	}
}

#endif
