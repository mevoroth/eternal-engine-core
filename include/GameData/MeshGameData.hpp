#pragma once

#include "GameData/GameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		class MeshGameData : public GameData
		{
		public:
			virtual void* InternalLoad(_In_ const GameDataSource& InNode) const override final;
		};
	}
}
