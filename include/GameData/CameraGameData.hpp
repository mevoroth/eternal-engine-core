#pragma once

#include "GameData/GameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		enum class CameraProjection : uint32_t
		{
			CAMERA_PROJECTION_ORTHOGRAPHIC = 0,
			CAMERA_PROJECTION_PERSPECTIVE
		};

		class CameraGameData : public GameData
		{
		public:
			virtual void* InternalLoad(_In_ const GameDataSource& InNode) const override final;
		};
	}
}
