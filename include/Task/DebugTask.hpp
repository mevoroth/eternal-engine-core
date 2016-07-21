#ifndef _DEBUG_TASK_HPP_
#define _DEBUG_TASK_HPP_

#include <vector>
#include "d3d11/D3D11PosColorVertexBuffer.hpp"
#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Context;
		class VertexBuffer;
		class Shader;
		class RenderTarget;
		class Constant;
	}
	namespace Task
	{
		using namespace Eternal::Graphics;
		using namespace Eternal::Parallel;

		class DebugTask : public Task
		{
		public:
			DebugTask(Context& ContextObj);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

			void SetRenderTarget(RenderTarget* RenderTargetObj);
			void SetModelViewProjectionMatrix(const Matrix4x4& ModelViewProjectionMatrix);

		private:
			void _DrawGizmo();

			Context& _Context;

			VertexBuffer* _AxisGizmoVerticesBuffer = nullptr;
			vector<Graphics::D3D11PosColorVertexBuffer::PosColorVertex> _AxisGizmoVertices;

			Shader* _VS = nullptr;
			Shader* _PS = nullptr;

			Constant* _ModelViewProjectionMatrixConstant = nullptr;

			RenderTarget* _RenderTarget = nullptr;

			Matrix4x4 _ModelViewProjectionMatrix;
		};
	}
}

#endif
