#include "Core/DebugDraw/DebugDraw.hpp"
#include "Core/System.hpp"
#include "Math/Math.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	using namespace Eternal::Math;
	using namespace Eternal::Types;

	static constexpr float DebugDrawFrontDepth = 0.0f;

	constexpr uint32_t DebugDrawCircleParameters::SubdivisionCountDefault;

	template<> void DebugDrawLine(_Inout_ Core::System& InOutSystem, const Vector2& InStart, const Vector2& InEnd, _In_ const DebugDrawParameters& InDebugDrawParameters)
	{
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back({
			Vector3(InStart,	DebugDrawFrontDepth),
			InDebugDrawParameters.Color
		});
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back({
			Vector3(InEnd,		DebugDrawFrontDepth),
			InDebugDrawParameters.Color

		});
	}

	template<> void DebugDrawLine(_Inout_ Core::System& InOutSystem, const Vector3& InStart, const Vector3& InEnd, _In_ const DebugDrawParameters& InDebugDrawParameters)
	{
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back({
			InStart,
			InDebugDrawParameters.Color
		});
		InOutSystem.GetGameFrame().DebugPrimitives.Lines.push_back({
			InEnd,
			InDebugDrawParameters.Color
		});
	}

	void DebugDrawCircle(_Inout_ Core::System& InOutSystem, const Vector2& InCenter, float Radius, _In_ const DebugDrawCircleParameters& InDebugDrawCircleParameters)
	{
		RotationMatrix2x2 RotationMatrix(PI_2 / static_cast<float>(InDebugDrawCircleParameters.SubdivisionCount));
		Vector2 Offset(Radius, 0.0f);

		for (uint32_t Index = 0; Index < InDebugDrawCircleParameters.SubdivisionCount; ++Index)
		{
			Vector2 NextOffset = RotationMatrix * Offset;
			DebugDrawLine(InOutSystem, InCenter + Offset, InCenter + NextOffset, InDebugDrawCircleParameters);
			Offset = NextOffset;
		}
	}
}
