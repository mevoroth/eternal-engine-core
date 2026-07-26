#pragma once

#include "GraphicData/MeshVertexFormat.hpp"

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

				vector<GraphicData::Position2DColorVertex> Lines;
			};
		}
	}
}
