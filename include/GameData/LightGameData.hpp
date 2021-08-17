#pragma once

#include "GameData/GameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		enum class LightType : uint32_t
		{
			LIGHT_TYPE_DIRECTIONAL = 0,
			LIGHT_TYPE_POINT,
			LIGHT_TYPE_SPOTLIGHT,
			LIGHT_TYPE_AREA
		};

		class LightGameData final : public GameData
		{
		public:
			virtual void* InternalLoad(_In_ const GameDataSource& InNode) const override final;
		};
	}
}
