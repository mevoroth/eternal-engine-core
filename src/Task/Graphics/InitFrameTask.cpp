#include "Task/Graphics/InitFrameTask.hpp"

#include "Macros/Macros.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/RenderTarget.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/RenderTargetCollection.hpp"

using namespace Eternal::Task;

InitFrameTask::InitFrameTask(_In_ Renderer& RendererObj, _In_ ContextCollection& Contexts, RenderTargetCollection** RenderTargetCollections, _In_ int RenderTargetsCollectionsCount)
	: _Renderer(RendererObj)
	, _Contexts(Contexts)
	, _RenderTargetCollectionsCount(RenderTargetsCollectionsCount)
{
	ETERNAL_ASSERT(_RenderTargetCollectionsCount > 0);
	_RenderTargetCollections = new RenderTargetCollection*[_RenderTargetCollectionsCount];
	for (int RenderTargetsIndex = 0; RenderTargetsIndex < _RenderTargetCollectionsCount; ++RenderTargetsIndex)
	{
		_RenderTargetCollections[RenderTargetsIndex] = RenderTargetCollections[RenderTargetsIndex];
	}
}

InitFrameTask::~InitFrameTask()
{
	delete[] _RenderTargetCollections;
	_RenderTargetCollections = nullptr;
}

void InitFrameTask::DoSetup()
{
}

void InitFrameTask::DoExecute()
{
	Context& ContextObj = _Contexts.Get();
	ContextObj.Begin();

	_Renderer.GetBackBuffer()->Clear(&ContextObj);
	for (int RenderTargetsIndex = 0; RenderTargetsIndex < _RenderTargetCollectionsCount; ++RenderTargetsIndex)
	{
		RenderTargetCollection& CurrentRenderTargets = *_RenderTargetCollections[RenderTargetsIndex];
		if (CurrentRenderTargets.GetDepthStencilRenderTarget())
			CurrentRenderTargets.GetDepthStencilRenderTarget()->Clear(&ContextObj);
		for (int BufferIndex = 0, BufferCount = CurrentRenderTargets.GetRenderTargetsCount(); BufferIndex < BufferCount; ++BufferIndex)
		{
			CurrentRenderTargets.GetRenderTargets()[BufferIndex]->Clear(&ContextObj);
		}
	}

	_RenderTargetCollections[0]->GetRenderTargets()[3]->Clear(&ContextObj, 0.5f);

	ContextObj.End();
	_Contexts.Release(ContextObj);
}

void InitFrameTask::DoReset()
{
}
