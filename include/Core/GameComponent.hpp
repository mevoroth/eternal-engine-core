#ifndef _GAME_COMPONENT_HPP_
#define _GAME_COMPONENT_HPP_

#include "Time/Time.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Time;

		class GameComponent
		{
		public:
			virtual void Begin() = 0;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) = 0;
			virtual void End() = 0;
		};
	}
}

#endif
