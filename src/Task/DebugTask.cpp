#include "Task/DebugTask.hpp"

#include <vector>
#include "Macros/Macros.hpp"
#include "Graphics/Context.hpp"
//#include "d3d11/d3d11ver"/

using namespace Eternal::Task;

DebugTask::DebugTask(Context& ContextObj)
	: _Context(ContextObj)
{
	D3D11PosColorVertexBuffer::PosColorVertex AxisVertices[] = {
		// Axis X
		{ Vector4(10000.f, 0.f, 0.f, 1.f), Vector4(1.f, 0.f, 0.f, 1.f) },
		{ Vector4(-10000.f, 0.f, 0.f, 1.f), Vector4(1.f, 0.f, 0.f, 1.f) },

		// Axis Y
		{ Vector4(0.f, 10000.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f) },
		{ Vector4(0.f, -10000.f, 0.f, 1.f), Vector4(0.f, 1.f, 0.f, 1.f) },

		// Axis Z
		{ Vector4(0.f, 0.f, 10000.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f) },
		{ Vector4(0.f, 0.f, -10000.f, 1.f), Vector4(0.f, 0.f, 1.f, 1.f) },
	};

	for (int AxisIndex = 0; AxisIndex < ETERNAL_ARRAYSIZE(AxisVertices); ++AxisIndex)
		_AxisGizmoVertices.push_back(AxisVertices[AxisIndex]);

	_AxisGizmoVerticesBuffer = new D3D11PosColorVertexBuffer(_AxisGizmoVerticesBuffer);
}

void DebugTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void DebugTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}

void DebugTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	_DrawGizmo();

	SetState(DONE);
}

void DebugTask::_DrawGizmo()
{
	_Context.SetTopology(Context::LINELIST);
	_Context.DrawDirect(_AxisGizmoVerticesBuffer);
}
