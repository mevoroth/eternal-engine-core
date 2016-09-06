#include "Task/Graphics/SolidGBufferTask.hpp"

#include "Graphics/Context.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/ViewportFactory.hpp"
#include "Graphics/SamplerFactory.hpp"
#include "Graphics/DepthStencilFactory.hpp"
#include "Graphics/DepthTest.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/DepthStencil.hpp"
#include "Core/GraphicGameObject.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/CameraMaterialProperty.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "GraphicData/BlendStateCollection.hpp"
#include "Core/MeshComponent.hpp"
#include "Mesh/Mesh.hpp"

#include "d3d11/D3D11Context.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::Graphics;

		class SolidGBufferData
		{
		public:
			SolidGBufferData(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargetCollectionObj, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
				: _Context(ContextObj)
				, _RenderTargetCollection(RenderTargetCollectionObj)
			{
				_Constant = CreateConstant(sizeof(CameraMaterialProperty::CommonConstants), Resource::DYNAMIC, Resource::WRITE);
				_DepthStencil = CreateDepthStencil(DepthTest(DepthTest::ALL, LESS), StencilTest());
				_Viewport = Viewports.GetViewport(ViewportCollection::FULLSCREEN);;
				_Sampler = Samplers.GetSampler(SamplerCollection::BILINEAR);
				_BlendState = BlendStates.GetBlendState(BlendStateCollection::ALPHA);
			}

			void SetGraphicObjects(_In_ GraphicObjects& Objects)
			{
				_Objects = &Objects;
			}

			GraphicObjects& GetGraphicObjects()
			{
				ETERNAL_ASSERT(_Objects);
				return *_Objects;
			}

			Context& GetContext()
			{
				return _Context;
			}

			RenderTargetCollection& GetRenderTargetCollection()
			{
				return _RenderTargetCollection;
			}

			Constant* GetConstant()
			{
				ETERNAL_ASSERT(_Constant);
				return _Constant;
			}

			Viewport* GetViewport()
			{
				ETERNAL_ASSERT(_Viewport);
				return _Viewport;
			}

			Sampler* GetSampler()
			{
				ETERNAL_ASSERT(_Sampler);
				return _Sampler;
			}

			DepthStencil* GetDepthStencil()
			{
				ETERNAL_ASSERT(_Sampler);
				return _DepthStencil;
			}

			BlendState* GetBlendState()
			{
				ETERNAL_ASSERT(_BlendState);
				return _BlendState;
			}

		private:
			Context& _Context;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
			Constant* _Constant = nullptr;
			Viewport* _Viewport = nullptr;
			Sampler* _Sampler = nullptr;
			BlendState* _BlendState = nullptr;
			DepthStencil* _DepthStencil = nullptr;
		};
	}
}

using namespace Eternal::Task;

#include <d3d11.h>

SolidGBufferTask::SolidGBufferTask(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
{
	_SolidGBufferData = new SolidGBufferData(ContextObj, RenderTargets, Samplers, Viewports, BlendStates);
}

void SolidGBufferTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void SolidGBufferTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(EXECUTING);

	vector<GraphicGameObject*>& GameObjects = _SolidGBufferData->GetGraphicObjects().GetGraphicGameObjects();
	Material* MaterialObj = _SolidGBufferData->GetGraphicObjects().GetMaterial();
	RenderTargetCollection& RenderTargetCollectionObj = _SolidGBufferData->GetRenderTargetCollection();

	RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS

	Context& ContextObj = _SolidGBufferData->GetContext();

	ContextObj.Begin();
	
	MaterialObj->Apply(ContextObj);
	ContextObj.SetBlendMode(_SolidGBufferData->GetBlendState());
	ContextObj.SetDepthBuffer(RenderTargetCollectionObj.GetDepthStencilRenderTarget());
	ContextObj.BindDepthStencilState(_SolidGBufferData->GetDepthStencil());
	ContextObj.SetRenderTargets(RenderTargetCollectionObj.GetRenderTargets(), RenderTargetCollectionObj.GetRenderTargetsCount());

	for (int GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex)
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		//GameObjects[GameObjectIndex]->GetTransformComponent()->GetLocalToWorldTransform()
		_Draw(*CurrentMesh);
	}

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();

	ContextObj.SetRenderTargets(NullRenderTargets, ETERNAL_ARRAYSIZE(NullRenderTargets));	// REMOVE THIS
	ContextObj.UnbindDepthStencilState();
	ContextObj.SetDepthBuffer(nullptr);														// REMOVE THIS
	MaterialObj->Reset(ContextObj);

	ContextObj.End();

	SetState(DONE);
}

void SolidGBufferTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}

void SolidGBufferTask::SetGraphicObjects(_In_ GraphicObjects& Objects)
{
	_SolidGBufferData->SetGraphicObjects(Objects);
}

void SolidGBufferTask::_Draw(_In_ Mesh& MeshObj)
{
	if (MeshObj.IsValidNode())
	{
		MeshObj.InitializeBuffers();
		Context& ContextObj = _SolidGBufferData->GetContext();
		
		ContextObj.DrawIndexed(MeshObj.GetVertexBuffer(), MeshObj.GetIndexBuffer());
	}

	for (int SubMeshIndex = 0; SubMeshIndex < MeshObj.GetSubMeshesCount(); ++SubMeshIndex)
	{
		Mesh& SubMesh = MeshObj.GetSubMesh(SubMeshIndex);
		_Draw(SubMesh);
	}
}

Constant* SolidGBufferTask::GetCommonConstant()
{
	return _SolidGBufferData->GetConstant();
}

Viewport* SolidGBufferTask::GetViewport()
{
	return _SolidGBufferData->GetViewport();
}


Sampler* SolidGBufferTask::GetSampler()
{
	return _SolidGBufferData->GetSampler();
}
