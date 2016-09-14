#include "Task/Graphics/OpaqueTask.hpp"

#include "Graphics/Context.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/ViewportFactory.hpp"
#include "Graphics/SamplerFactory.hpp"
#include "Graphics/DepthStencilFactory.hpp"
#include "Graphics/DepthTest.hpp"
#include "Graphics/StencilTest.hpp"
#include "Graphics/DepthStencil.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Core/GraphicGameObject.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/CameraMaterialProperty.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "GraphicData/BlendStateCollection.hpp"
#include "Core/MeshComponent.hpp"
#include "Core/MaterialComponent.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::Graphics;

		class OpaqueTaskData
		{
		public:
			OpaqueTaskData(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargetCollectionObj, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
				: _Context(ContextObj)
				, _RenderTargetCollection(RenderTargetCollectionObj)
			{
				InputLayout::VertexDataType DataTypes[] = {
					InputLayout::POSITION_T,
					InputLayout::NORMAL_T,
					InputLayout::UV_T
				};

				_VS = ShaderFactory::Get()->CreateVertexShader("Opaque", "opaque.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_PS = ShaderFactory::Get()->CreatePixelShader("Opaque", "opaque.ps.hlsl");

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

			Shader* GetVS()
			{
				ETERNAL_ASSERT(_VS);
				return _VS;
			}

			Shader* GetPS()
			{
				ETERNAL_ASSERT(_PS);
				return _PS;
			}

		private:
			Context& _Context;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
			Shader* _VS = nullptr;
			Shader* _PS = nullptr;
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

OpaqueTask::OpaqueTask(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates)
{
	_OpaqueTaskData = new OpaqueTaskData(ContextObj, RenderTargets, Samplers, Viewports, BlendStates);
}

void OpaqueTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void OpaqueTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(EXECUTING);
	SetState(DONE);
	return;
	vector<GraphicGameObject*>& GameObjects = _OpaqueTaskData->GetGraphicObjects().GetGraphicGameObjects();
	Material* MaterialObj = _OpaqueTaskData->GetGraphicObjects().GetMaterial();
	RenderTargetCollection& RenderTargetCollectionObj = _OpaqueTaskData->GetRenderTargetCollection();

	RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS

	Context& ContextObj = _OpaqueTaskData->GetContext();

	ContextObj.Begin();
	
	//MaterialObj->Apply(ContextObj);

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetViewport(_OpaqueTaskData->GetViewport());
	ContextObj.SetBlendMode(_OpaqueTaskData->GetBlendState());
	ContextObj.SetDepthBuffer(RenderTargetCollectionObj.GetDepthStencilRenderTarget());
	ContextObj.BindDepthStencilState(_OpaqueTaskData->GetDepthStencil());
	ContextObj.SetRenderTargets(RenderTargetCollectionObj.GetRenderTargets(), RenderTargetCollectionObj.GetRenderTargetsCount());
	ContextObj.BindSampler<Context::PIXEL>(0, _OpaqueTaskData->GetSampler());
	ContextObj.BindShader<Context::VERTEX>(_OpaqueTaskData->GetVS());
	ContextObj.BindShader<Context::PIXEL>(_OpaqueTaskData->GetPS());

	for (int GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex)
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		Material* CurrentMaterial = GameObjects[GameObjectIndex]->GetMaterialComponent()->GetMaterial();
		CurrentMaterial->Apply(ContextObj);
		//GameObjects[GameObjectIndex]->GetTransformComponent()->GetLocalToWorldTransform()
		_Draw(*CurrentMesh);
		CurrentMaterial->Reset(ContextObj);
	}

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();
	ContextObj.UnbindSampler<Context::PIXEL>(0);

	ContextObj.SetRenderTargets(NullRenderTargets, ETERNAL_ARRAYSIZE(NullRenderTargets));	// REMOVE THIS
	ContextObj.UnbindDepthStencilState();
	ContextObj.SetDepthBuffer(nullptr);														// REMOVE THIS

	//MaterialObj->Reset(ContextObj);

	ContextObj.End();

	SetState(DONE);
}

void OpaqueTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}

void OpaqueTask::SetGraphicObjects(_In_ GraphicObjects& Objects)
{
	_OpaqueTaskData->SetGraphicObjects(Objects);
}

void OpaqueTask::_Draw(_In_ Mesh& MeshObj)
{
	if (MeshObj.IsValidNode())
	{
		MeshObj.InitializeBuffers();
		Context& ContextObj = _OpaqueTaskData->GetContext();
		
		ContextObj.DrawIndexed(MeshObj.GetVertexBuffer(), MeshObj.GetIndexBuffer());
	}

	for (int SubMeshIndex = 0; SubMeshIndex < MeshObj.GetSubMeshesCount(); ++SubMeshIndex)
	{
		Mesh& SubMesh = MeshObj.GetSubMesh(SubMeshIndex);
		_Draw(SubMesh);
	}
}

Constant* OpaqueTask::GetCommonConstant()
{
	return _OpaqueTaskData->GetConstant();
}

Viewport* OpaqueTask::GetViewport()
{
	return _OpaqueTaskData->GetViewport();
}


Sampler* OpaqueTask::GetSampler()
{
	return _OpaqueTaskData->GetSampler();
}
