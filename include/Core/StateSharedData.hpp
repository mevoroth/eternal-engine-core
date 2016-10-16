#ifndef _STATE_SHARED_DATA_HPP_
#define _STATE_SHARED_DATA_HPP_

#include <vector>

namespace Eternal
{
	namespace Core
	{
		using namespace std;

		class GraphicGameObject;
		class CameraGameObject;
		class LightGameObject;

		class StateSharedData
		{
		public:
			vector<GraphicGameObject*>* GraphicGameObjects = nullptr;
			CameraGameObject* Camera = nullptr;
			LightGameObject* Lights = nullptr;
		};
	}
}

#endif
