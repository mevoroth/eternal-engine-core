#include "Task/Graphics/OpaqueTask.hpp"

#include "Types/Types.hpp"
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
#include "Core/StateSharedData.hpp"
#include "Core/GraphicGameObject.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "GraphicData/BlendStateCollection.hpp"
#include "Core/MeshComponent.hpp"
#include "Core/MaterialComponent.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/TransformComponent.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/CameraGameObject.hpp"
#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"
#include "Container/Stack.hpp"
#include "ShadersStructs/CB0FrameConstants.hpp"
#include "ShadersStructs/CB1ObjectConstants.hpp"

using namespace Eternal::Task;
using namespace Eternal::Shaders;

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::GraphicData;
		using namespace Eternal::Graphics;
		using namespace Eternal::Container;

		typedef Stack<Matrix4x4, 64> TransformStack;

		class OpaqueTaskData
		{
		public:
			OpaqueTaskData(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargetCollectionObj, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
				: _Contexts(Contexts)
				, _RenderTargetCollection(RenderTargetCollectionObj)
				, _SharedData(SharedData)
			{
				ETERNAL_ASSERT(_SharedData);

				InputLayout::VertexDataType DataTypes[] = {
					InputLayout::POSITION_T,
					InputLayout::NORMAL_T,
					InputLayout::UV_T
				};

				_VS = ShaderFactory::Get()->CreateVertexShader("Opaque", "opaque.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_PS = ShaderFactory::Get()->CreatePixelShader("Opaque", "opaque.ps.hlsl");

				//InputLayout::VertexDataType DataTypes[] = {
				//	InputLayout::POSITION_T,
				//	InputLayout::COLOR_T
				//};

				//_VS = ShaderFactory::Get()->CreateVertexShader("VertexColor", "vertexcolor.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				//_PS = ShaderFactory::Get()->CreatePixelShader("VertexColor", "vertexcolor.ps.hlsl");

				_CommonConstants = CreateConstant(sizeof(CB0FrameConstants), Resource::DYNAMIC, Resource::WRITE);
				_ObjectConstants = CreateConstant(sizeof(CB1ObjectConstants), Resource::DYNAMIC, Resource::WRITE);
				_DepthStencil = CreateDepthStencil(DepthTest(DepthTest::ALL, LESS), StencilTest());
				_Viewport = Viewports.GetViewport(ViewportCollection::FULLSCREEN);;
				_Sampler = Samplers.GetSampler(SamplerCollection::BILINEAR);
				_BlendState = BlendStates.GetBlendState(BlendStateCollection::ALPHA);
			}

			ContextCollection& GetContexts()
			{
				return _Contexts;
			}

			RenderTargetCollection& GetRenderTargetCollection()
			{
				return _RenderTargetCollection;
			}

			Constant* GetFrameConstants()
			{
				ETERNAL_ASSERT(_CommonConstants);
				return _CommonConstants;
			}

			Constant* GetObjectConstants()
			{
				ETERNAL_ASSERT(_ObjectConstants);
				return _ObjectConstants;
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

			StateSharedData* GetSharedData()
			{
				return _SharedData;
			}

			TransformStack& GetTransformStack()
			{
				return _TransformStack;
			}

		private:
			StateSharedData* _SharedData = nullptr;
			ContextCollection& _Contexts;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
			Shader* _VS = nullptr;
			Shader* _PS = nullptr;
			Constant* _CommonConstants = nullptr;
			Constant* _ObjectConstants = nullptr;
			Viewport* _Viewport = nullptr;
			Sampler* _Sampler = nullptr;
			BlendState* _BlendState = nullptr;
			DepthStencil* _DepthStencil = nullptr;

			TransformStack _TransformStack;
		};
	}
}

OpaqueTask::OpaqueTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
{
	_OpaqueTaskData = new OpaqueTaskData(Contexts, RenderTargets, Samplers, Viewports, BlendStates, SharedData);
}

OpaqueTask::~OpaqueTask()
{
	delete _OpaqueTaskData;
	_OpaqueTaskData = nullptr;
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

	if (!(_OpaqueTaskData->GetSharedData()->GraphicGameObjects && _OpaqueTaskData->GetSharedData()->GraphicGameObjects->size())) // REMOVE THIS
	{
		SetState(DONE);
		return;
	}

	vector<GraphicGameObject*>& GameObjects = *_OpaqueTaskData->GetSharedData()->GraphicGameObjects;
	TransformStack& TransformStackObj = _OpaqueTaskData->GetTransformStack();
	RenderTargetCollection& RenderTargetCollectionObj = _OpaqueTaskData->GetRenderTargetCollection();
	
	RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS
	
	Context& ContextObj = _OpaqueTaskData->GetContexts().Get();

	ContextObj.Begin();
	
	_SetupFrameConstants(ContextObj);

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetViewport(_OpaqueTaskData->GetViewport());
	ContextObj.SetBlendMode(_OpaqueTaskData->GetBlendState());
	ContextObj.SetDepthBuffer(RenderTargetCollectionObj.GetDepthStencilRenderTarget());
	ContextObj.BindDepthStencilState(_OpaqueTaskData->GetDepthStencil());
	ContextObj.SetRenderTargets(RenderTargetCollectionObj.GetRenderTargets(), RenderTargetCollectionObj.GetRenderTargetsCount());
	ContextObj.BindSampler<Context::PIXEL>(0, _OpaqueTaskData->GetSampler());
	ContextObj.BindShader<Context::VERTEX>(_OpaqueTaskData->GetVS());
	ContextObj.BindShader<Context::PIXEL>(_OpaqueTaskData->GetPS());

	ContextObj.BindConstant<Context::VERTEX>(0, _OpaqueTaskData->GetFrameConstants());

	for (int GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex) // Parallel For?
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		Material* CurrentMaterial = GameObjects[GameObjectIndex]->GetMaterialComponent()->GetMaterial();
		CurrentMaterial->Apply(ContextObj);
		TransformStackObj.Push(GameObjects[GameObjectIndex]->GetTransformComponent()->GetLocalToWorldTransform());
		_Draw(ContextObj, *CurrentMesh);
		TransformStackObj.Pop();
		ETERNAL_ASSERT(TransformStackObj.Empty());
		CurrentMaterial->Reset(ContextObj);
	}

	ContextObj.UnbindConstant<Context::VERTEX>(0);

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();
	ContextObj.UnbindSampler<Context::PIXEL>(0);

	ContextObj.SetRenderTargets(NullRenderTargets, ETERNAL_ARRAYSIZE(NullRenderTargets));	// REMOVE THIS
	ContextObj.UnbindDepthStencilState();
	ContextObj.SetDepthBuffer(nullptr);														// REMOVE THIS

	ContextObj.End();

	_OpaqueTaskData->GetContexts().Release(ContextObj);

	SetState(DONE);
}

void OpaqueTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}

void OpaqueTask::_SetupFrameConstants(_In_ Context& ContextObj)
{
	Camera* CameraObj = _OpaqueTaskData->GetSharedData()->Camera->GetCameraComponent()->GetCamera();
	Resource* ConstantResource = _OpaqueTaskData->GetFrameConstants()->GetAsResource();
	Resource::LockedResource FrameConstants = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CameraObj->GetViewProjectionMatrixTransposed(((CB0FrameConstants*)FrameConstants.Data)->ViewProjection);
	ConstantResource->Unlock(ContextObj);
}

void OpaqueTask::_SetupObjectConstants(_In_ Context& ContextObj, _In_ Mesh& MeshObj)
{
	Matrix4x4 ModelMatrix = _OpaqueTaskData->GetTransformStack().Head();
	Resource* ConstantResource = _OpaqueTaskData->GetObjectConstants()->GetAsResource();
	Resource::LockedResource ObjectConstant = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	memcpy(ObjectConstant.Data, &ModelMatrix, sizeof(CB1ObjectConstants));
	ConstantResource->Unlock(ContextObj);
}

void OpaqueTask::_Draw(_In_ Context& ContextObj, _In_ Mesh& MeshObj)
{
	_OpaqueTaskData->GetTransformStack().Push(_OpaqueTaskData->GetTransformStack().Head() * MeshObj.GetTransform().GetModelMatrix());

	if (MeshObj.IsValidNode())
	{
		MeshObj.InitializeBuffers(); // REMOVE THIS
		_SetupObjectConstants(ContextObj, MeshObj); // CODE IS WRONG
		ContextObj.BindConstant<Context::VERTEX>(1, _OpaqueTaskData->GetObjectConstants());
		ContextObj.DrawIndexed(MeshObj.GetVertexBuffer(), MeshObj.GetIndexBuffer());
		ContextObj.UnbindConstant<Context::VERTEX>(1);
	}

	for (int SubMeshIndex = 0; SubMeshIndex < MeshObj.GetSubMeshesCount(); ++SubMeshIndex)
	{
		Mesh& SubMesh = MeshObj.GetSubMesh(SubMeshIndex);
		_Draw(ContextObj, SubMesh);
	}

	_OpaqueTaskData->GetTransformStack().Pop();
}
