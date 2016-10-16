#include "Task/ImguiEndTask.hpp"

#include <cstdint>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include "Macros/Macros.hpp"
#include "imgui.h"
#include "Imgui/D3D11ImguiVertexBuffer.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Resource.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/Format.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "d3d11/D3D11Renderer.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/IndexBufferFactory.hpp"
#include "d3d11/D3D11Texture.hpp"
#include "d3d11/D3D11Sampler.hpp"
#include "d3d11/D3D11BlendState.hpp"
#include "Types/Types.hpp"

using namespace Eternal::Task;
using namespace Eternal::Imgui;
using namespace Eternal::Graphics;
using namespace Eternal::Types;

ImguiEndTask::ImguiEndTask(_In_ ContextCollection& Contexts, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports)
	: _Contexts(Contexts)
{
	InputLayout::VertexDataType VerticesDataType[] = {
		InputLayout::IMGUI_POSITION_T,
		InputLayout::IMGUI_UV_T,
		InputLayout::IMGUI_COLOR_T
	};

	ShaderFactory::Get()->RegisterShaderPath("..\\eternal-engine-core\\shaders\\Imgui\\");

	_VS = ShaderFactory::Get()->CreateVertexShader("Imgui", "imgui.vs.hlsl", VerticesDataType, ETERNAL_ARRAYSIZE(VerticesDataType));
	_PS = ShaderFactory::Get()->CreatePixelShader("Imgui", "imgui.ps.hlsl");

	_ViewProjectionMatrixConstant = CreateConstant(sizeof(Matrix4x4), D3D11Resource::DYNAMIC, Resource::WRITE);

	uint8_t* Pixels;
	int Height;
	int Width;

	ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);
	_Texture = new D3D11Texture(RGBA8888, D3D11Resource::DEFAULT, Resource::NONE, Width, Height, Pixels);
	ImGui::GetIO().Fonts->TexID = ((D3D11Texture*)_Texture)->GetD3D11ShaderResourceView();

	_Sampler = Samplers.GetSampler(SamplerCollection::BILINEAR);

	_BlendState = new D3D11BlendState(BlendState::SRC_ALPHA, BlendState::INV_SRC_ALPHA, BlendState::OP_ADD,
		BlendState::SRC_ALPHA, BlendState::INV_SRC_ALPHA, BlendState::OP_ADD);

	_Viewport = Viewports.GetViewport(ViewportCollection::FULLSCREEN);
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

	ImGui::Render();

	ImDrawData* ImguiDrawData = ImGui::GetDrawData();
	ETERNAL_ASSERT(ImguiDrawData->Valid);
	
	Context& ContextObj = _Contexts.Get();

	ContextObj.Begin();

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
	Resource* ViewProjConstant = _ViewProjectionMatrixConstant->GetAsResource();
	Resource::LockedResource LockedResourceObj = ViewProjConstant->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	memcpy(LockedResourceObj.Data, &ViewProjMatrix, sizeof(Matrix4x4));
	ViewProjConstant->Unlock(ContextObj);

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetBlendMode(_BlendState);
	
	ContextObj.BindShader<Context::VERTEX>(_VS);
	ContextObj.BindShader<Context::PIXEL>(_PS);

	ContextObj.BindConstant<Context::VERTEX>(0, _ViewProjectionMatrixConstant);
	ContextObj.BindSampler<Context::PIXEL>(0, _Sampler);

	ContextObj.SetViewport(_Viewport);
	ContextObj.SetRenderTargets(&_RenderTarget, 1);

	for (int DrawListIndex = 0, VertexBufferCount = ImguiDrawData->CmdListsCount; DrawListIndex < VertexBufferCount; ++DrawListIndex)
	{
		int ElementOffset = 0;
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
			IndexBuffer* IndicesBuffer = CreateIndexBuffer<uint32_t>(*Indices);
			ContextObj.BindBuffer<Context::PIXEL>(0, _Texture->GetAsResource());
			ContextObj.DrawIndexed(&VerticesBuffer, IndicesBuffer);
			delete Indices;
		}

		delete Vertices;
	}

	// REMOVE THIS
	RenderTarget* NullRenderTarget = nullptr;
	ContextObj.SetRenderTargets(&NullRenderTarget, 1);

	ContextObj.UnbindBuffer<Context::PIXEL>(0);
	ContextObj.UnbindSampler<Context::PIXEL>(0);
	ContextObj.UnbindConstant<Context::VERTEX>(0);

	ContextObj.End();
	_Contexts.Release(ContextObj);

	SetState(Task::DONE);
}

void ImguiEndTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(Task::IDLE);
}
