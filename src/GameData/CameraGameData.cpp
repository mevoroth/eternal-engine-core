#include "GameData/CameraGameData.hpp"

#include "GameObjects/CameraGameObject.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;
		using namespace Eternal::GameObjects;

		void* CameraGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			CameraGameObject* CameraObject = new CameraGameObject();
			InNode.Get<GameObject>(*CameraObject);
			return CameraObject;
		}
	}
}
