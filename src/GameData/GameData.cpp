#include "GameData/GameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		void IterateGameDataCollection(_In_ const GameDataSource& InSource, _In_ GameObjectFunctor Functor)
		{
			uint32_t GameObjectCount = 0;
			InSource.Get(GameObjectCount);
			for (uint32_t GameObjectIndex = 0; GameObjectIndex < GameObjectCount; ++GameObjectIndex)
			{
				const GameDataSource& SubNode = InSource.GetSubNode(GameObjectIndex);
				Functor(SubNode);
			}
		}
	}
}
