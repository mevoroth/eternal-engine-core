#include "Imgui/D3D11ImguiVertexBuffer.hpp"

using namespace Eternal::Imgui;

size_t D3D11ImguiVertexBuffer::_Size = sizeof(ImDrawVert);

D3D11ImguiVertexBuffer::D3D11ImguiVertexBuffer(vector<ImDrawVert>& Vertex)
	: _Vertex(Vertex)
	, D3D11VertexBuffer(Vertex.size() * sizeof(ImDrawVert), (void*)&Vertex[0])
{
}

size_t D3D11ImguiVertexBuffer::GetSize() const
{
	return _Size;
}
