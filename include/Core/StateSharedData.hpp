#ifndef _STATE_SHARED_DATA_HPP_
#define _STATE_SHARED_DATA_HPP_

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class GameObject;

		class StateSharedData
		{
		public:
			vector<GameObject*>* GameObjects = nullptr;
		};
	}
}

#endif
