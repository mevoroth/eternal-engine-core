#include "Core/DebugDraw/DebugDrawPrimitives.hpp"

namespace Eternal
{
	namespace Core
	{
		namespace Debug
		{
			constexpr uint32_t DebugDrawPrimitives::DebugPassPrimitivesCount;

			DebugDrawPrimitives::DebugDrawPrimitives()
			{
				Lines.reserve(DebugPassPrimitivesCount);
			}
		}
	}
}
