#include "Task/Graphics/CompositingTask.hpp"

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

CompositingTask::CompositingTask(_In_ ContextCollection& DeferredContexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& LightingRenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
	: _Contexts(DeferredContexts)
	, _OpaqueRenderTargets(OpaqueRenderTargets)
	, _LightingRenderTargets(LightingRenderTargets)
	, _Samplers(Samplers)
	, _Viewports(Viewports)
	, _BlendStates(BlendStates)
{
	_VS = ShaderFactory::Get()->CreateVertexShader("Compositing", "compositing.vs.hlsl");
	_PS = ShaderFactory::Get()->CreatePixelShader("Compositing", "compositing.ps.hlsl");
}

void CompositingTask::DoSetup()
{
}
void CompositingTask::DoReset()
{
}
void CompositingTask::DoExecute()
{
	RenderTarget* BackBuffer = Renderer::Get()->GetBackBuffer();
	RenderTarget* NullRenderTarget = nullptr;

	Context& CompositingContext = _Contexts.Get();
	CompositingContext.Begin();

	CompositingContext.SetTopology(Context::TRIANGLELIST);
	CompositingContext.SetBlendMode(_BlendStates.GetBlendState(BlendStateCollection::SOURCE));
	CompositingContext.SetViewport(_Viewports.GetViewport(ViewportCollection::FULLSCREEN));
	CompositingContext.BindShader<Context::VERTEX>(_VS);
	CompositingContext.BindShader<Context::PIXEL>(_PS);
	CompositingContext.BindBuffer<Context::PIXEL>(0, _OpaqueRenderTargets.GetDepthStencilRenderTarget()->GetAsResource());
	for (int BufferIndex = 0; BufferIndex < _OpaqueRenderTargets.GetRenderTargetsCount() - 1; ++BufferIndex)
	{
		CompositingContext.BindBuffer<Context::PIXEL>(BufferIndex + 1, _OpaqueRenderTargets.GetRenderTargets()[BufferIndex]->GetAsResource());
	}
	const int SlotOffset = _OpaqueRenderTargets.GetRenderTargetsCount() + 1 - 1;
	for (int BufferIndex = 0; BufferIndex < _LightingRenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		CompositingContext.BindBuffer<Context::PIXEL>(BufferIndex + SlotOffset, _LightingRenderTargets.GetRenderTargets()[BufferIndex]->GetAsResource());
	}
	CompositingContext.BindSampler<Context::PIXEL>(0, _Samplers.GetSampler(SamplerCollection::BILINEAR));

	CompositingContext.SetRenderTargets(&BackBuffer, 1);
	CompositingContext.DrawPrimitive(6);
	CompositingContext.SetRenderTargets(&NullRenderTarget, 1);

	CompositingContext.UnbindSampler<Context::PIXEL>(0);
	for (int BufferIndex = 0; BufferIndex < _OpaqueRenderTargets.GetRenderTargetsCount(); ++BufferIndex)
	{
		CompositingContext.UnbindBuffer<Context::PIXEL>(BufferIndex);
	}
	CompositingContext.UnbindShader<Context::PIXEL>();
	CompositingContext.UnbindShader<Context::VERTEX>();

	CompositingContext.End();
	_Contexts.Release(CompositingContext);
}
