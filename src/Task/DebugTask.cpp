#include "Task/DebugTask.hpp"

#include <vector>
#include "Macros/Macros.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "d3d11/D3D11Constant.hpp"
//#include "d3d11/d3d11ver"/

using namespace Eternal::Task;

DebugTask::DebugTask(Context& ContextObj)
	: _Context(ContextObj)
{
	//D3D11PosColorVertexBuffer::PosColorVertex AxisVertices[] = {
	//	// Axis X
	//	{ Vector4(1000.f, 0.f, 0.f, 1.f), 0xFF0000FF },
	//	{ Vector4(-1000.f, 0.f, 0.f, 1.f), 0xFFFFFF00 },

	//	// Axis Y
	//	{ Vector4(0.f, 1000.f, 0.f, 1.f), 0xFF00FF00 },
	//	{ Vector4(0.f, -1000.f, 0.f, 1.f), 0xFFFF00FF },

	//	// Axis Z
	//	{ Vector4(0.f, 0.f, 1000.f, 1.f), 0xFFFF0000 },
	//	{ Vector4(0.f, 0.f, -1000.f, 1.f), 0xFF00FFFF },
	//};

	//for (int AxisIndex = 0; AxisIndex < ETERNAL_ARRAYSIZE(AxisVertices); ++AxisIndex)
	//	_AxisGizmoVertices.push_back(AxisVertices[AxisIndex]);

	//_AxisGizmoVerticesBuffer = new D3D11PosColorVertexBuffer(_AxisGizmoVertices);
	
	InputLayout::VertexDataType AxisGizmoVertexType[] = {
		InputLayout::POSITION_T,
		InputLayout::COLOR_T
	};

	_VS = ShaderFactory::Get()->CreateVertexShader("drawcolor", "drawcolor.vs.hlsl", AxisGizmoVertexType, ETERNAL_ARRAYSIZE(AxisGizmoVertexType));
	_PS = ShaderFactory::Get()->CreatePixelShader("drawcolor", "drawcolor.ps.hlsl");

	_ModelViewProjectionMatrixConstant = new D3D11Constant(sizeof(Matrix4x4), D3D11Resource::DYNAMIC, Resource::WRITE);
}

void DebugTask::DoSetup()
{
}

void DebugTask::DoReset()
{
}

void DebugTask::DoExecute()
{
	_DrawGizmo();
}

void DebugTask::SetRenderTarget(RenderTarget* RenderTargetObj)
{
	_RenderTarget = RenderTargetObj;
}

void DebugTask::SetModelViewProjectionMatrix(const Matrix4x4& ModelViewProjectionMatrix)
{
	_ModelViewProjectionMatrix = ModelViewProjectionMatrix;
}

void DebugTask::_DrawGizmo()
{
	RenderTarget* NullRenderTarget = nullptr;

	Resource::LockedResource LockedResourceObj = ((D3D11Constant*)_ModelViewProjectionMatrixConstant)->Lock(_Context, Resource::LOCK_WRITE_DISCARD);
	memcpy(LockedResourceObj.Data, &_ModelViewProjectionMatrix, sizeof(Matrix4x4));
	((D3D11Constant*)_ModelViewProjectionMatrixConstant)->Unlock(_Context);

	_Context.SetTopology(Context::LINELIST);
	_Context.BindShader<Context::VERTEX>(_VS);
	_Context.BindShader<Context::PIXEL>(_PS);

	_Context.BindConstant<Context::VERTEX>(0, _ModelViewProjectionMatrixConstant);
	
	_Context.SetRenderTargets(&_RenderTarget, 1);
	_Context.DrawDirect(_AxisGizmoVerticesBuffer);
	_Context.SetRenderTargets(&NullRenderTarget, 1);
	
	_Context.UnbindConstant<Context::VERTEX>(0);

	_Context.UnbindShader<Context::VERTEX>();
	_Context.UnbindShader<Context::PIXEL>();
	_Context.SetTopology(Context::TRIANGLELIST);
}
