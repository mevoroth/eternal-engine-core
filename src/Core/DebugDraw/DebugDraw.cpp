#include "Core/DebugDraw/DebugDraw.hpp"
#include "Core/System.hpp"
#include "Types/Types.hpp"

namespace Eternal
{
	using namespace Eternal::Types;

	void DebugDrawLine(Core::System& InSystem)
	{
		InSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3(-100.0f, 0.0f, 0.0f));
		InSystem.GetGameFrame().DebugPrimitives.Lines.push_back(Vector3( 100.0f, 0.0f, 0.0f));
	}
}
