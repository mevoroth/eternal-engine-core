#include "Task/Graphics/SolidGBufferTask.hpp"

#include "Graphics/Context.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/ViewportFactory.hpp"
#include "Graphics/SamplerFactory.hpp"
#include "Core/GraphicGameObject.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/CameraMaterialProperty.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "Core/MeshComponent.hpp"
#include "Mesh/Mesh.hpp"

#include "d3d11/D3D11Context.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::GraphicData;

		class SolidGBufferData
		{
		public:
			SolidGBufferData(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargetCollectionObj)
				: _Context(ContextObj)
				, _RenderTargetCollection(RenderTargetCollectionObj)
			{
				_Constant = CreateConstant(sizeof(CameraMaterialProperty::CommonConstants), Resource::DYNAMIC, Resource::WRITE);
				_Viewport = CreateViewport(0, 0, 1600, 900);
				_Sampler = CreateSampler(true, true, false, false, Sampler::WRAP, Sampler::WRAP, Sampler::WRAP);
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

		private:
			Context& _Context;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
			Constant* _Constant = nullptr;
			Viewport* _Viewport = nullptr;
			Sampler* _Sampler = nullptr;
		};
	}
}

using namespace Eternal::Task;

#include <d3d11.h>

SolidGBufferTask::SolidGBufferTask(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargets)
{
	_SolidGBufferData = new SolidGBufferData(ContextObj, RenderTargets);
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

	ID3D11CommandList* CmdList = nullptr;

	//if (CmdList)
	//{

	//	SetState(DONE);
	//	return;
	//}

	vector<GraphicGameObject*>& GameObjects = _SolidGBufferData->GetGraphicObjects().GetGraphicGameObjects();
	Material* MaterialObj = _SolidGBufferData->GetGraphicObjects().GetMaterial();
	RenderTargetCollection& RenderTargetCollectionObj = _SolidGBufferData->GetRenderTargetCollection();

	RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS

	Context& ContextObj = _SolidGBufferData->GetContext();

	MaterialObj->Apply(ContextObj);
	ContextObj.SetRenderTargets(RenderTargetCollectionObj.GetRenderTargets(), RenderTargetCollectionObj.GetRenderTargetsCount());

	for (int GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex)
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		//GameObjects[GameObjectIndex]->GetTransformComponent()->GetLocalToWorldTransform()
		_Draw(*CurrentMesh);
	}

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();

	ContextObj.SetRenderTargets(NullRenderTargets, ETERNAL_ARRAYSIZE(NullRenderTargets)); // REMOVE THIS
	MaterialObj->Reset(ContextObj);

	((D3D11Context&)ContextObj).GetD3D11Context()->FinishCommandList(FALSE, &CmdList);
	((D3D11Context&)_SolidGBufferData->GetContext()).GetD3D11Context()->ExecuteCommandList(CmdList, FALSE);
	CmdList->Release();

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
