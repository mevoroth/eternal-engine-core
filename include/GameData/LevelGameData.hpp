#pragma once

#include "GameData/GameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		class LevelGameData final : public GameData
		{
		protected:
			virtual void* InternalLoad(_In_ const GameDataSource& InNode) const override final;
		};
	}
}
