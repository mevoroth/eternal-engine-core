#pragma once

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

	using namespace Eternal::Types;

	void DebugDrawCircle(_Inout_ Core::System& InOutSystem, const Vector2& InCenter, float Radius);

	template<typename PositionType>
	void DebugDrawLine(_Inout_ Core::System& InOutSystem, const PositionType& InStart, const PositionType& InEnd);
}
