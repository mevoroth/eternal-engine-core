#pragma once

#include "Types/Color.hpp"

namespace Eternal
{
	namespace Types
	{
		struct Vector2;
	}

	namespace Core
	{
		class System;
	}

	struct DebugDrawParameters
	{
		Types::Color32 Color = Types::Color32::Red();
	};

	struct DebugDrawCircleParameters : public DebugDrawParameters
	{
		static constexpr uint32_t SubdivisionCountDefault = 16u;

		uint32_t SubdivisionCount = SubdivisionCountDefault;
	};

	using namespace Eternal::Types;

	template<typename PositionType>
	void DebugDrawLine(_Inout_ Core::System& InOutSystem, _In_ const PositionType& InStart, _In_ const PositionType& InEnd, _In_ const DebugDrawParameters& InDebugDrawParameters = DebugDrawParameters());

	void DebugDrawCircle(_Inout_ Core::System& InOutSystem, _In_ const Vector2& InCenter, float Radius, _In_ const DebugDrawCircleParameters& InDebugDrawCircleParameters = DebugDrawCircleParameters());
}
