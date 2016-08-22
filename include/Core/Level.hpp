#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class GameObject;

		class Level
		{
		public:
			Level();

		private:
			vector<GameObject*> _GameObjects;
		};
	}
}

#endif
