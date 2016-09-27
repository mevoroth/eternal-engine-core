#include "Task/Graphics/CompositingTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "GraphicData/BlendStateCollection.hpp"

using namespace Eternal::Task;
using namespace Eternal::Graphics;

CompositingTask::CompositingTask(_In_ Context& MainContext, _In_ ContextCollection& DeferredContexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
	: _MainContext(MainContext)
	, _DeferredContexts(DeferredContexts)
	, _RenderTargets(RenderTargets)
	, _Samplers(Samplers)
	, _Viewports(Viewports)
	, _BlendStates(BlendStates)
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

	RenderTarget* BackBuffer = Renderer::Get()->GetBackBuffer();
	RenderTarget* NullRenderTarget = nullptr;

	BackBuffer->Clear(&_MainContext);
	_RenderTargets.GetDepthStencilRenderTarget()->Clear(&_MainContext);
	for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		_RenderTargets.GetRenderTargets()[BufferIndex]->Clear(&_MainContext);
	}

	//_MainContext.SetTopology(Context::TRIANGLELIST);
	//_MainContext.SetBlendMode(_BlendStates.GetBlendState(BlendStateCollection::SOURCE));
	//_MainContext.SetViewport(_Viewports.GetViewport(ViewportCollection::FULLSCREEN));
	//_MainContext.BindShader<Context::VERTEX>(_VS);
	//_MainContext.BindShader<Context::PIXEL>(_PS);
	//_MainContext.BindBuffer<Context::PIXEL>(0, _RenderTargets.GetDepthStencilRenderTarget()->GetAsResource());
	//for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	//{
	//	_MainContext.BindBuffer<Context::PIXEL>(BufferIndex + 1, _RenderTargets.GetRenderTargets()[BufferIndex]->GetAsResource());
	//}
	//_MainContext.BindSampler<Context::PIXEL>(0, _Samplers.GetSampler(SamplerCollection::BILINEAR));

	//_MainContext.SetRenderTargets(&BackBuffer, 1);
	//_MainContext.DrawPrimitive(6);
	//_MainContext.SetRenderTargets(&NullRenderTarget, 1);

	//_MainContext.UnbindSampler<Context::PIXEL>(0);
	//for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	//{
	//	_MainContext.UnbindBuffer<Context::PIXEL>(BufferIndex);
	//}
	//_MainContext.UnbindShader<Context::PIXEL>();
	//_MainContext.UnbindShader<Context::VERTEX>();

	Context& CompositingContext = _DeferredContexts.Get();
	CompositingContext.Begin();

	CompositingContext.SetTopology(Context::TRIANGLELIST);
	CompositingContext.SetBlendMode(_BlendStates.GetBlendState(BlendStateCollection::SOURCE));
	CompositingContext.SetViewport(_Viewports.GetViewport(ViewportCollection::FULLSCREEN));
	CompositingContext.BindShader<Context::VERTEX>(_VS);
	CompositingContext.BindShader<Context::PIXEL>(_PS);
	CompositingContext.BindBuffer<Context::PIXEL>(0, _RenderTargets.GetDepthStencilRenderTarget()->GetAsResource());
	for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		CompositingContext.BindBuffer<Context::PIXEL>(BufferIndex + 1, _RenderTargets.GetRenderTargets()[BufferIndex]->GetAsResource());
	}
	CompositingContext.BindSampler<Context::PIXEL>(0, _Samplers.GetSampler(SamplerCollection::BILINEAR));

	CompositingContext.SetRenderTargets(&BackBuffer, 1);
	CompositingContext.DrawPrimitive(6);
	CompositingContext.SetRenderTargets(&NullRenderTarget, 1);

	CompositingContext.UnbindSampler<Context::PIXEL>(0);
	for (int BufferIndex = 0; BufferIndex < _RenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		CompositingContext.UnbindBuffer<Context::PIXEL>(BufferIndex);
	}
	CompositingContext.UnbindShader<Context::PIXEL>();
	CompositingContext.UnbindShader<Context::VERTEX>();

	CompositingContext.End();
	_DeferredContexts.Release(CompositingContext);

#error "IMPLEMENT FLUSH"
	//for (int DeferredContextIndex = 0; DeferredContextIndex < _DeferredContextsCount; ++DeferredContextIndex)
	//{
	//	_DeferredContexts[DeferredContextIndex]->Flush(_MainContext);
	//}

	SetState(DONE);
}
