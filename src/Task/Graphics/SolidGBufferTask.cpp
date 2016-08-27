#include "Task/Graphics/SolidGBufferTask.hpp"

#include "Graphics/Context.hpp"
#include "Core/GraphicGameObject.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "Core/MeshComponent.hpp"
#include "Mesh/Mesh.hpp"

namespace Eternal
{
	namespace Task
	{
		class SolidGBufferData
		{
		public:
			SolidGBufferData(_In_ Context& ContextObj, _In_ RenderTargetCollection& RenderTargetCollectionObj)
				: _Context(ContextObj)
				, _RenderTargetCollection(RenderTargetCollectionObj)
			{
			}

			void SetGraphicObjects(_In_ GraphicObjects& Objects)
			{
				_Objects = &Objects;
			}

			GraphicObjects& GetGraphicObjects()
			{
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

		private:
			Context& _Context;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
		};
	}
}

using namespace Eternal::Task;

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
		Context& ContextObj = _SolidGBufferData->GetContext();
		
		ContextObj.DrawIndexed(MeshObj.GetVertexBuffer(), MeshObj.GetIndexBuffer());
	}

	for (int SubMeshIndex = 0; SubMeshIndex < MeshObj.GetSubMeshesCount(); ++SubMeshIndex)
	{
		Mesh& SubMesh = MeshObj.GetSubMesh(SubMeshIndex);
		_Draw(SubMesh);
	}
}
