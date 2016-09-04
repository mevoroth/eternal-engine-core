#include "Task/Graphics/CompositingTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"

using namespace Eternal::Task;
using namespace Eternal::Graphics;

CompositingTask::CompositingTask(_In_ Context& MainContext, _In_ Context** DeferredContexts, _In_ int DeferredContextCount, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports)
	: _MainContext(MainContext)
	, _DeferredContexts(DeferredContexts)
	, _DeferredContextsCount(DeferredContextCount)
	, _RenderTargets(RenderTargets)
	, _Samplers(Samplers)
	, _Viewports(Viewports)
{
	_VS = ShaderFactory::Get()->CreateVertexShader("Compositing", "compositing.vs.hlsl");
	_PS = ShaderFactory::Get()->CreatePixelShader("Compositing", "compositing.ps.hlsl");
}

void CompositingTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}
void CompositingTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}
void CompositingTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	for (int DeferredContextIndex = 0; DeferredContextIndex < _DeferredContextsCount; ++DeferredContextIndex)
	{
		_DeferredContexts[DeferredContextIndex]->Flush(_MainContext);
	}

	RenderTarget* BackBuffer = Renderer::Get()->GetBackBuffer();
	RenderTarget* NullRenderTarget = nullptr;

	_MainContext.SetTopology(Context::TRIANGLELIST);
	_MainContext.SetViewport(_Viewports.GetViewport(ViewportCollection::FULLSCREEN));
	_MainContext.BindShader<Context::VERTEX>(_VS);
	_MainContext.BindShader<Context::PIXEL>(_PS);
	_MainContext.BindBuffer<Context::PIXEL>(0, _RenderTargets.GetDepthStencilRenderTarget()->GetAsResource());
	for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		_MainContext.BindBuffer<Context::PIXEL>(BufferIndex + 1, _RenderTargets.GetRenderTargets()[BufferIndex]->GetAsResource());
	}
	_MainContext.BindSampler<Context::PIXEL>(0, _Samplers.GetSampler(SamplerCollection::BILINEAR));

	_MainContext.SetRenderTargets(&BackBuffer, 1);
	_MainContext.DrawPrimitive(6);
	_MainContext.SetRenderTargets(&NullRenderTarget, 1);

	_MainContext.UnbindSampler<Context::PIXEL>(0);
	for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		_MainContext.UnbindBuffer<Context::PIXEL>(BufferIndex);
	}
	_MainContext.UnbindShader<Context::PIXEL>();
	_MainContext.UnbindShader<Context::VERTEX>();

	SetState(DONE);
}
