#ifndef _GAMEOBJECT_HPP_
#define _GAMEOBJECT_HPP_

#include "Time/Time.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Time;
		class GameObject
		{
			virtual void Begin() = 0;
			virtual void Update(const TimeT& ElapsedMilliseconds) = 0;
			virtual void End() = 0;
		};
	}
}

#endif
