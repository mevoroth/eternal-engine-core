#include "Core/DebugDraw/DebugDraw.hpp"
#include "Core/System.hpp"
#include "Math/Math.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	using namespace Eternal::Math;
	using namespace Eternal::Types;

	void DebugDrawCircle(_Inout_ Core::System& InOutSystem, const Vector2& InCenter, float Radius)
	{
		RotationMatrix2x2 RotationMatrix(PI_2 / 16.0f);
		Vector2 Offset(Radius, 0.0f);

		for (uint32_t Index = 0; Index < 16; ++Index)
		{
			Vector2 NextOffset = RotationMatrix * Offset;
			InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3(InCenter + Offset, 1.0f));
			InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3(InCenter + NextOffset, 1.0f));
			Offset = NextOffset;
		}
	}

	template<> void DebugDrawLine(_Inout_ Core::System& InOutSystem, const Vector2& InStart, const Vector2& InEnd)
	{
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3(InStart,	1.0f));
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3(InEnd,	1.0f));
	}

	template<> void DebugDrawLine(_Inout_ Core::System& InOutSystem, const Vector3& InStart, const Vector3& InEnd)
	{
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(InStart);
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back(InEnd);
	}
}
