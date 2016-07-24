#include "Task/ImguiEndTask.hpp"

#include <cstdint>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include "Macros/Macros.hpp"
#include "imgui.h"
#include "Imgui/D3D11ImguiVertexBuffer.hpp"
#include "d3d11/D3D11UInt32IndexBuffer.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Resource.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "d3d11/D3D11Renderer.hpp"
#include "d3d11/D3D11Constant.hpp"
#include "d3d11/D3D11Texture.hpp"
#include "d3d11/D3D11Sampler.hpp"
#include "d3d11/D3D11BlendState.hpp"
#include "Types/Types.hpp"

using namespace Eternal::Task;
using namespace Eternal::Imgui;
using namespace Eternal::Graphics;
using namespace Eternal::Types;

ImguiEndTask::ImguiEndTask(Context& ContextObj)
	: _Context(ContextObj)
{
	InputLayout::VertexDataType VerticesDataType[] = {
		InputLayout::IMGUI_POSITION_T,
		InputLayout::IMGUI_UV_T,
		InputLayout::IMGUI_COLOR_T
	};

	ShaderFactory::Get()->RegisterShaderPath("..\\eternal-engine-core\\shaders\\Imgui\\");

	_VS = ShaderFactory::Get()->CreateVertexShader("Imgui", "imgui.vs.hlsl", VerticesDataType, ETERNAL_ARRAYSIZE(VerticesDataType));
	_PS = ShaderFactory::Get()->CreatePixelShader("Imgui", "imgui.ps.hlsl");

	_ViewProjectionMatrixConstant = new D3D11Constant(sizeof(Matrix4x4), D3D11Resource::DYNAMIC, Resource::WRITE);

	uint8_t* Pixels;
	int Height;
	int Width;

	ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);
	_Texture = new D3D11Texture(Texture::RGBA8888, D3D11Resource::DEFAULT, Resource::NONE, Width, Height, Pixels);
	ImGui::GetIO().Fonts->TexID = ((D3D11Texture*)_Texture)->GetD3D11ShaderResourceView();

	_Sampler = new D3D11Sampler(true, true, true, false, Sampler::WRAP, Sampler::WRAP, Sampler::WRAP);

	_BlendState = new D3D11BlendState(BlendState::SRC_ALPHA, BlendState::INV_SRC_ALPHA, BlendState::OP_ADD,
		BlendState::SRC_ALPHA, BlendState::INV_SRC_ALPHA, BlendState::OP_ADD);
}

void ImguiEndTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(Task::SETUP);
}

void ImguiEndTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(Task::EXECUTING);

	OutputDebugString("[ImguiEndTask::Execute]Begin ImguiEndTask\n");

	ImGui::Render();

	ImDrawData* ImguiDrawData = ImGui::GetDrawData();
	ETERNAL_ASSERT(ImguiDrawData->Valid);
	
	float L = 0.0f;
	float R = ImGui::GetIO().DisplaySize.x;
	float B = ImGui::GetIO().DisplaySize.y;
	float T = 0.0f;

	Matrix4x4 ViewProjMatrix(
		2.0f / (R - L),		0.0f,				0.0f, 0.0f,
		0.0f,				2.0f / (T - B),		0.0f, 0.0f,
		0.0f,				0.0f,				0.5f, 0.0f,
		(R + L) / (L - R),	(T + B) / (B - T),	0.5f, 1.0f
	);
	Resource::LockedResource LockedResourceObj = ((D3D11Constant*)_ViewProjectionMatrixConstant)->Lock(_Context, Resource::LOCK_WRITE_DISCARD);
	memcpy(LockedResourceObj.Data, &ViewProjMatrix, sizeof(Matrix4x4));
	((D3D11Constant*)_ViewProjectionMatrixConstant)->Unlock(_Context);

	_Context.SetBlendMode(_BlendState);

	_Context.BindShader<Context::VERTEX>(_VS);
	_Context.BindShader<Context::PIXEL>(_PS);

	_Context.BindConstant<Context::VERTEX>(0, _ViewProjectionMatrixConstant);
	_Context.BindSampler<Context::PIXEL>(0, _Sampler);

	_Context.SetRenderTargets(&_RenderTarget, 1);

	int ElementOffset = 0;

	for (int DrawListIndex = 0, VertexBufferCount = ImguiDrawData->CmdListsCount; DrawListIndex < VertexBufferCount; ++DrawListIndex)
	{
		ImDrawList* CurrentDrawList = ImguiDrawData->CmdLists[DrawListIndex];

		vector<ImDrawVert>* Vertices = new vector<ImDrawVert>();
		for (int VertexIndex = 0; VertexIndex < CurrentDrawList->VtxBuffer.size(); ++VertexIndex)
		{
			Vertices->push_back(CurrentDrawList->VtxBuffer[VertexIndex]);
		}
		D3D11ImguiVertexBuffer VerticesBuffer(*Vertices);

		for (int CommandBufferIndex = 0, CommandBufferCount = CurrentDrawList->CmdBuffer.size(); CommandBufferIndex < CommandBufferCount; ++CommandBufferIndex)
		{
			ImDrawCmd& CurrentDrawCmd = CurrentDrawList->CmdBuffer[CommandBufferIndex];

			vector<uint32_t>* Indices = new vector<uint32_t>();
			for (int ElementIndex = 0, ElementCount = CurrentDrawCmd.ElemCount; ElementIndex < ElementCount; ++ElementIndex)
			{
				Indices->push_back(CurrentDrawList->IdxBuffer[ElementIndex + ElementOffset]);
			}
			ElementOffset += CurrentDrawCmd.ElemCount;
			D3D11UInt32IndexBuffer IndicesBuffer(*Indices);
			_Context.BindBuffer<Context::PIXEL>(0, (D3D11Texture*)_Texture);
			_Context.DrawIndexed(&VerticesBuffer, &IndicesBuffer);
			delete Indices;
		}

		delete Vertices;
	}

	RenderTarget* NullRenderTarget = nullptr;
	_Context.SetRenderTargets(&NullRenderTarget, 1);

	_Context.UnbindBuffer<Context::PIXEL>(0);
	_Context.UnbindSampler<Context::PIXEL>(0);
	_Context.UnbindConstant<Context::VERTEX>(0);

	static_cast<Graphics::D3D11Renderer*>(Graphics::Renderer::Get())->Flush();

	OutputDebugString("[ImguiEndTask::Execute]End ImguiEndTask\n");

	SetState(Task::DONE);
}

void ImguiEndTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(Task::IDLE);
}
