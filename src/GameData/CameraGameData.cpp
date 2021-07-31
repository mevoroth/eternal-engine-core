#include "GameData/CameraGameData.hpp"

#include "Core/GameObject.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;

		void* CameraGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			GameObject* CameraGameObject = new GameObject();
			InNode.Get<GameObject>(*CameraGameObject);
			return CameraGameObject;
		}
	}
}
