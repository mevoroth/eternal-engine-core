#ifndef _D3D11_IMGUI_VERTEX_BUFFER_HPP_
#define _D3D11_IMGUI_VERTEX_BUFFER_HPP_

#include "d3d11/D3D11VertexBuffer.hpp"

#include <vector>

#include "imgui.h"

namespace Eternal
{
	namespace Imgui
	{
		using namespace Eternal::Graphics;
		using namespace std;

		class D3D11ImguiVertexBuffer : public D3D11VertexBuffer
		{
		public:
			D3D11ImguiVertexBuffer(_In_ vector<ImDrawVert>& Vertex);

			virtual size_t GetSize() const override;
			virtual size_t GetVerticesCount() const override;

		private:
			static size_t _Size;

			vector<ImDrawVert>& _Vertex;
		};
	}
}

#endif
