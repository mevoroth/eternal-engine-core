#pragma once

#include "Types/Types.hpp"

namespace Eternal
{
	namespace Core
	{
		namespace Debug
		{
			using namespace std;
			using namespace Eternal::Types;

			struct DebugDrawPrimitives
			{
				static constexpr uint32_t DebugPassPrimitivesCount = 4096u;

				DebugDrawPrimitives();

				vector<Vector3> Lines;
			};
		}
	}
}
