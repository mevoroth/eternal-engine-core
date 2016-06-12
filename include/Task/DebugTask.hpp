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

		private:
			void _DrawGizmo();

			Context& _Context;

			VertexBuffer* _AxisGizmoVerticesBuffer = nullptr;
			vector<Graphics::D3D11PosColorVertexBuffer::PosColorVertex> _AxisGizmoVertices;
		};
	}
}

#endif
