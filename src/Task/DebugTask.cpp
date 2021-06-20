#include "Task/DebugTask.hpp"

#include "Graphics/Context.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/VertexBufferFactory.hpp"
#include "Graphics/IndexBufferFactory.hpp"
#include "GraphicData/VertexFormat.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/RenderTargetCollection.hpp"

using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Graphics;

		class DebugTaskData
		{
			struct TriangleIndexHelper
			{
				uint32_t V0, V1, V2;
			};
		public:
			DebugTaskData(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
				: _Contexts(Contexts)
				, _RenderTargets(OpaqueRenderTargets)
			{
				InputLayout::VertexDataType AxisGizmoVertexType[] = {
					InputLayout::POSITION_T,
					InputLayout::COLOR_T
				};

				_VS = ShaderFactory::Get()->CreateVertexShader("DrawColor", "drawcolor.vs.hlsl", AxisGizmoVertexType, ETERNAL_ARRAYSIZE(AxisGizmoVertexType));
				_PS = ShaderFactory::Get()->CreatePixelShader("DrawColor", "drawcolor.ps.hlsl");

				vector<GraphicData::PosColorVertex> AxisVertices;
				AxisVertices.resize(4 * 6); // Quad per axis
#pragma region Axis Vertices Declaration
				// X+
				AxisVertices[0] = { Vector4(1000.0f, 0.0f, 1.0f, 1.0f),		0xFF0000FF };
				AxisVertices[1] = { Vector4(1000.0f, 0.0f, -1.0f, 1.0f),	0xFF0000FF };
				AxisVertices[2] = { Vector4(0.0f, 0.0f, 1.0f, 1.0f),		0xFF0000FF };
				AxisVertices[3] = { Vector4(0.0f, 0.0f, -1.0f, 1.0f),		0xFF0000FF };

				// X-
				AxisVertices[4] = { Vector4(-1000.0f, 0.0f, -1.0f, 1.0f),	0xFFFFFF00 };
				AxisVertices[5] = { Vector4(-1000.0f, 0.0f, 1.0f, 1.0f),	0xFFFFFF00 };
				AxisVertices[6] = { Vector4(0.0f, 0.0f, -1.0f, 1.0f),		0xFFFFFF00 };
				AxisVertices[7] = { Vector4(0.0f, 0.0f, 1.0f, 1.0f),		0xFFFFFF00 };

				// Y+
				AxisVertices[8] = { Vector4(-1.0f, 1000.0f, 0.0f, 1.0f),	0xFF00FF00 };
				AxisVertices[9] = { Vector4(1.0f, 1000.0f, 0.0f, 1.0f),		0xFF00FF00 };
				AxisVertices[10] = { Vector4(-1.0f, 0.0f, 0.0f, 1.0f),		0xFF00FF00 };
				AxisVertices[11] = { Vector4(1.0f, 0.0f, 0.0f, 1.0f),		0xFF00FF00 };

				// Y-
				AxisVertices[12] = { Vector4(-1.0f, 0.0f, 0.0f, 1.0f),		0xFFFF00FF };
				AxisVertices[13] = { Vector4(1.0f, 0.0f, 0.0f, 1.0f),		0xFFFF00FF };
				AxisVertices[14] = { Vector4(-1.0f, -1000.0f, 0.0f, 1.0f),	0xFFFF00FF };
				AxisVertices[15] = { Vector4(1.0f, -1000.0f, 0.0f, 1.0f),	0xFFFF00FF };

				// Z+
				AxisVertices[16] = { Vector4(-1.0f, 0.0f, 1000.0f, 1.0f),	0xFFFF0000 };
				AxisVertices[17] = { Vector4(1.0f, 0.0f, 1000.0f, 1.0f),	0xFFFF0000 };
				AxisVertices[18] = { Vector4(-1.0f, 0.0f, 0.0f, 1.0f),		0xFFFF0000 };
				AxisVertices[19] = { Vector4(1.0f, 0.0f, 0.0f, 1.0f),		0xFFFF0000 };

				// Z-
				AxisVertices[20] = { Vector4(-1.0f, 0.0f, 0.0f, 1.0f),		0xFF00FFFF };
				AxisVertices[21] = { Vector4(1.0f, 0.0f, 0.0f, 1.0f),		0xFF00FFFF };
				AxisVertices[22] = { Vector4(-1.0f, 0.0f, -1000.0f, 1.0f),	0xFF00FFFF };
				AxisVertices[23] = { Vector4(1.0f, 0.0f, -1000.0f, 1.0f),	0xFF00FFFF };
#pragma endregion Axis Vertices Declaration
				_Vertices = CreateVertexBuffer(AxisVertices);

				vector<uint32_t> AxisIndices;
				AxisIndices.resize(6 * 6);
#pragma region Axis Indices Declaration
				TriangleIndexHelper* AxisIndicesTriangleAlias = (TriangleIndexHelper*)&AxisIndices[0];

				// X+
				AxisIndicesTriangleAlias[0] = { 0, 1, 2 };
				AxisIndicesTriangleAlias[1] = { 2, 1, 3 };

				// X-
				AxisIndicesTriangleAlias[2] = { 4, 5, 6 };
				AxisIndicesTriangleAlias[3] = { 6, 5, 7 };

				// Y+
				AxisIndicesTriangleAlias[4] = { 8, 9, 10 };
				AxisIndicesTriangleAlias[5] = { 10, 9, 11 };

				// Y-
				AxisIndicesTriangleAlias[6] = { 12, 13, 14 };
				AxisIndicesTriangleAlias[7] = { 14, 13, 15 };

				// Z+
				AxisIndicesTriangleAlias[8] = { 16, 17, 18 };
				AxisIndicesTriangleAlias[9] = { 18, 17, 19 };

				// Z-
				AxisIndicesTriangleAlias[10] = { 20, 21, 22 };
				AxisIndicesTriangleAlias[11] = { 22, 21, 23 };
#pragma endregion Axis Indices Declaration
				_Indices = CreateIndexBuffer(AxisIndices);
			}

			Shader* GetPS()
			{
				return _PS;
			}

			Shader* GetVS()
			{
				return _VS;
			}

			VertexBuffer* GetVertices()
			{
				return _Vertices;
			}

			IndexBuffer* GetIndices()
			{
				return _Indices;
			}

			Constant* GetFrameConstants()
			{
				return nullptr;
			}

			ContextCollection& GetContexts()
			{
				return _Contexts;
			}

			RenderTargetCollection& GetRenderTargets()
			{
				return _RenderTargets;
			}

		private:
			ContextCollection& _Contexts;
			RenderTargetCollection& _RenderTargets;

			Shader* _VS = nullptr;
			Shader* _PS = nullptr;

			VertexBuffer* _Vertices = nullptr;
			IndexBuffer* _Indices = nullptr;
		};
	}
}

DebugTask::DebugTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
{
	_DebugTaskData = new DebugTaskData(Contexts, OpaqueRenderTargets, Samplers, Viewports, SharedData);

	//_ModelViewProjectionMatrixConstant = new D3D11Constant(sizeof(Matrix4x4), D3D11Resource::DYNAMIC, Resource::WRITE);
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

void DebugTask::_DrawGizmo()
{
	//RenderTarget* NullRenderTarget = nullptr;

	//Context& ContextObj = _DebugTaskData->GetContexts().Get();

	//_DebugTaskData->GetFrameConstants()->GetAsResource()->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	//_DebugTaskData->GetFrameConstants()->GetAsResource()->Unlock(ContextObj);

	//ContextObj.SetTopology(Context::TRIANGLELIST);
	//ContextObj.BindShader<Context::VERTEX>(_DebugTaskData->GetVS());
	//ContextObj.BindShader<Context::PIXEL>(_DebugTaskData->GetPS());

	//ContextObj.BindConstant<Context::VERTEX>(0, _DebugTaskData->GetFrameConstants());

	//ContextObj.SetRenderTargets(_DebugTaskData->GetRenderTargets().GetRenderTargets(), _DebugTaskData->GetRenderTargets().GetRenderTargetsCount());
	//ContextObj.DrawIndexed(_DebugTaskData->GetVertices(), _DebugTaskData->GetIndices());
	//ContextObj.SetRenderTargets(&NullRenderTarget, 1);

	//ContextObj.UnbindConstant<Context::VERTEX>(0);

	//ContextObj.UnbindShader<Context::VERTEX>();
	//ContextObj.UnbindShader<Context::PIXEL>();
	//ContextObj.SetTopology(Context::TRIANGLELIST);

	//_DebugTaskData->GetContexts().Release(ContextObj);
}
