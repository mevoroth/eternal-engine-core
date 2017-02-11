#include "Task/Graphics/RenderObjectsTask.hpp"

#include "Types/Types.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/StructuredBuffer.hpp"
#include "Graphics/StructuredBufferFactory.hpp"
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
#include "GraphicData/GraphicTaskConfig.hpp"
#include "Core/MeshComponent.hpp"
#include "Core/MaterialComponent.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/TransformComponent.hpp"
#include "Core/StateSharedData.hpp"
#include "Core/CameraGameObject.hpp"
#include "Core/LightGameObject.hpp"
#include "Core/LightComponent.hpp"
#include "Light/Shadow.hpp"
#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"
#include "Container/Stack.hpp"
#include "ShadersStructs/CB0FrameConstants.hpp"
#include "ShadersStructs/CB1ObjectConstants.hpp"
#include "ShadersStructs/SB5InstanceStructuredBuffer.hpp"

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

		class RenderObjectsTaskData
		{
		public:
			RenderObjectsTaskData(_In_ const GraphicTaskConfig& Config, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargetCollectionObj, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
				: _Contexts(Contexts)
				, _RenderTargetCollection(RenderTargetCollectionObj)
				, _SharedData(SharedData)
			{
				ETERNAL_ASSERT(_SharedData);

				InputLayout::VertexDataType DataTypes[] = {
					InputLayout::POSITION_T,
					InputLayout::NORMAL_T,
					InputLayout::TANGENT_T,
					InputLayout::BINORMAL_T,
					InputLayout::UV_T
				};

				_VS = ShaderFactory::Get()->CreateVertexShader("Opaque", "opaque.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_PS = ShaderFactory::Get()->CreatePixelShader(Config.PS, Config.PSFile, Config.Defines);

				//InputLayout::VertexDataType DataTypes[] = {
				//	InputLayout::POSITION_T,
				//	InputLayout::COLOR_T
				//};

				//_VS = ShaderFactory::Get()->CreateVertexShader("VertexColor", "vertexcolor.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				//_PS = ShaderFactory::Get()->CreatePixelShader("VertexColor", "vertexcolor.ps.hlsl");

				_FrameConstants = CreateConstant(sizeof(CB0FrameConstants), Resource::DYNAMIC, Resource::WRITE);
				_ObjectConstants = CreateConstant(sizeof(CB1ObjectConstants), Resource::DYNAMIC, Resource::WRITE);
				_InstanceStructuredBuffer = CreateStructuredBuffer(Resource::DYNAMIC, Resource::WRITE, sizeof(Matrix4x4), RenderObjectsTask::MAX_INSTANCES);
				_DepthStencil = CreateDepthStencil(DepthTest(DepthTest::ALL, LESS), StencilTest());
				_Viewport = Viewports.GetViewport(Config.ViewportConfig);
				_Sampler = Samplers.GetSampler(SamplerCollection::BILINEAR);
				_BlendState = BlendStates.GetBlendState(Config.BlendConfig);

				ETERNAL_ASSERT(_FrameConstants);
				ETERNAL_ASSERT(_ObjectConstants);
				ETERNAL_ASSERT(_InstanceStructuredBuffer);
				ETERNAL_ASSERT(_Viewport);
				ETERNAL_ASSERT(_Sampler);
				ETERNAL_ASSERT(_DepthStencil);
				ETERNAL_ASSERT(_BlendState);
				ETERNAL_ASSERT(_VS);
				ETERNAL_ASSERT(_PS);
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
				return _FrameConstants;
			}

			Constant* GetObjectConstants()
			{
				return _ObjectConstants;
			}

			StructuredBuffer* GetInstanceStructuredBuffer()
			{
				return _InstanceStructuredBuffer;
			}

			Viewport* GetViewport()
			{
				return _Viewport;
			}

			Sampler* GetSampler()
			{
				return _Sampler;
			}

			DepthStencil* GetDepthStencil()
			{
				return _DepthStencil;
			}

			BlendState* GetBlendState()
			{
				return _BlendState;
			}

			Shader* GetVS()
			{
				return _VS;
			}

			Shader* GetPS()
			{
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

			void SetCamera(_In_ Camera* CameraObj)
			{
				_Camera = CameraObj;
			}

			Camera* GetCamera()
			{
				return _Camera;
			}

		private:
			TransformStack _TransformStack;

			StateSharedData* _SharedData = nullptr;
			ContextCollection& _Contexts;
			RenderTargetCollection& _RenderTargetCollection;
			GraphicObjects* _Objects = nullptr;
			Shader* _VS = nullptr;
			Shader* _PS = nullptr;
			Constant* _FrameConstants = nullptr;
			Constant* _ObjectConstants = nullptr;
			StructuredBuffer* _InstanceStructuredBuffer = nullptr;
			Viewport* _Viewport = nullptr;
			Sampler* _Sampler = nullptr;
			BlendState* _BlendState = nullptr;
			DepthStencil* _DepthStencil = nullptr;

			Camera* _Camera = nullptr;
		};
	}
}

RenderObjectsTask::RenderObjectsTask(_In_ const GraphicTaskConfig& Config, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
{
	_RenderObjectsTaskData = new RenderObjectsTaskData(Config, Contexts, RenderTargets, Samplers, Viewports, BlendStates, SharedData);
}

RenderObjectsTask::~RenderObjectsTask()
{
	delete _RenderObjectsTaskData;
	_RenderObjectsTaskData = nullptr;
}

void RenderObjectsTask::DoExecute()
{
	if (!(_RenderObjectsTaskData->GetSharedData()->GraphicGameObjects && _RenderObjectsTaskData->GetSharedData()->GraphicGameObjects->size())) // REMOVE THIS
	{
		return;
	}

	vector<GraphicGameObject*>& GameObjects = *_RenderObjectsTaskData->GetSharedData()->GraphicGameObjects;
	TransformStack& TransformStackObj = _RenderObjectsTaskData->GetTransformStack();
	RenderTargetCollection& RenderTargetCollectionObj = _RenderObjectsTaskData->GetRenderTargetCollection();
	
	//RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS
	RenderTarget* NullRenderTargets[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; // REMOVE THIS
	
	Context& ContextObj = _RenderObjectsTaskData->GetContexts().Get();

	ContextObj.Begin();
	
	_SetupFrameConstants(ContextObj, _RenderObjectsTaskData->GetCamera()); // REMOVE THIS

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetViewport(_RenderObjectsTaskData->GetViewport());
	ContextObj.SetBlendMode(_RenderObjectsTaskData->GetBlendState());
	ContextObj.SetDepthBuffer(RenderTargetCollectionObj.GetDepthStencilRenderTarget());
	ContextObj.BindDepthStencilState(_RenderObjectsTaskData->GetDepthStencil());

	if (RenderTargetCollectionObj.GetRenderTargetsCount())
		ContextObj.SetRenderTargets(RenderTargetCollectionObj.GetRenderTargets(), RenderTargetCollectionObj.GetRenderTargetsCount());
	
	ContextObj.BindSampler<Context::PIXEL>(0, _RenderObjectsTaskData->GetSampler());
	ContextObj.BindShader<Context::VERTEX>(_RenderObjectsTaskData->GetVS());
	ContextObj.BindShader<Context::PIXEL>(_RenderObjectsTaskData->GetPS());

	ContextObj.BindConstant<Context::VERTEX>(0, _RenderObjectsTaskData->GetFrameConstants());

	TransformStackObj.Push(NewIdentity());
	for (int GameObjectIndex = 0; GameObjectIndex < GameObjects.size(); ++GameObjectIndex) // Parallel For?
	{
		Mesh* CurrentMesh = GameObjects[GameObjectIndex]->GetMeshComponent()->GetMesh();
		Material* CurrentMaterial = GameObjects[GameObjectIndex]->GetMaterialComponent()->GetMaterial();
		CurrentMaterial->Apply(ContextObj);
		
		_SetupInstanceStructuredBuffer(ContextObj, GameObjects[GameObjectIndex]); // REMOVE THIS
		ContextObj.BindBuffer<Context::VERTEX>(5, _RenderObjectsTaskData->GetInstanceStructuredBuffer()->GetAsResource());

		_Draw(ContextObj, *CurrentMesh, GameObjects[GameObjectIndex]->GetInstanceCount());

		ContextObj.UnbindBuffer<Context::VERTEX>(5);
		CurrentMaterial->Reset(ContextObj);
	}
	TransformStackObj.Pop();
	ETERNAL_ASSERT(TransformStackObj.Empty());

	ContextObj.UnbindConstant<Context::VERTEX>(0);

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();
	ContextObj.UnbindSampler<Context::PIXEL>(0);
	
	if (RenderTargetCollectionObj.GetRenderTargetsCount())
		ContextObj.SetRenderTargets(NullRenderTargets, ETERNAL_ARRAYSIZE(NullRenderTargets));	// REMOVE THIS
	
	ContextObj.UnbindDepthStencilState();
	ContextObj.SetDepthBuffer(nullptr);														// REMOVE THIS

	ContextObj.End();

	_RenderObjectsTaskData->GetContexts().Release(ContextObj);
}

void RenderObjectsTask::DoReset()
{
}

void RenderObjectsTask::_SetupFrameConstants(_In_ Context& ContextObj, _In_ Camera* CameraObj)
{
	Resource* ConstantResource = _RenderObjectsTaskData->GetFrameConstants()->GetAsResource();
	Resource::LockedResource FrameConstants = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CameraObj->GetViewProjectionMatrixTransposed(((CB0FrameConstants*)FrameConstants.Data)->ViewProjection);
	ConstantResource->Unlock(ContextObj);
}

void RenderObjectsTask::_SetupObjectConstants(_In_ Context& ContextObj, _In_ Mesh& MeshObj)
{
	Matrix4x4 ModelMatrix = _RenderObjectsTaskData->GetTransformStack().Head();
	Resource* ConstantResource = _RenderObjectsTaskData->GetObjectConstants()->GetAsResource();

	Transpose(ModelMatrix);

	Resource::LockedResource ObjectConstantRaw = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CB1ObjectConstants* ObjectConstant = (CB1ObjectConstants*)ObjectConstantRaw.Data;
	ObjectConstant->ObjectModel = ModelMatrix;
	ConstantResource->Unlock(ContextObj);
}

void RenderObjectsTask::_SetupInstanceStructuredBuffer(_In_ Context& ContextObj, _In_ GraphicGameObject* GraphicObject)
{
	ETERNAL_ASSERT(GraphicObject->GetInstanceCount() < MAX_INSTANCES);

	Resource::LockedResource InstancesRaw = _RenderObjectsTaskData->GetInstanceStructuredBuffer()->GetAsResource()->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	SB5InstanceStructuredBuffer* Instances = (SB5InstanceStructuredBuffer*)InstancesRaw.Data;

	for (int InstanceIndex = 0, InstanceCount = GraphicObject->GetInstanceCount(); InstanceIndex < InstanceCount; ++InstanceIndex)
	{
		Transform& InstanceTransform = GraphicObject->GetInstance(InstanceIndex)->GetTransformComponent()->GetTransform();
		Matrix4x4 InstanceModel = InstanceTransform.GetModelMatrix();
		Transpose(InstanceModel);
		Instances[InstanceIndex].Model = InstanceModel;
	}

	_RenderObjectsTaskData->GetInstanceStructuredBuffer()->GetAsResource()->Unlock(ContextObj);
}

void RenderObjectsTask::_Draw(_In_ Context& ContextObj, _In_ Mesh& MeshObj, _In_ int InstanceCount)
{
	_RenderObjectsTaskData->GetTransformStack().Push(_RenderObjectsTaskData->GetTransformStack().Head() * MeshObj.GetTransform().GetModelMatrix());

	if (MeshObj.IsValidNode())
	{
		_SetupObjectConstants(ContextObj, MeshObj);

		ContextObj.BindConstant<Context::PIXEL>(1, _RenderObjectsTaskData->GetObjectConstants());
		ContextObj.BindConstant<Context::VERTEX>(1, _RenderObjectsTaskData->GetObjectConstants());

		// REMOVE THIS
		ContextObj.BindBuffer<Context::PIXEL>(0, MeshObj._Diffuse->GetAsResource());
		ContextObj.BindBuffer<Context::PIXEL>(1, MeshObj._Specular->GetAsResource());
		ContextObj.BindBuffer<Context::PIXEL>(3, MeshObj._Normal->GetAsResource());
		
		ContextObj.DrawIndexedInstanced(MeshObj.GetVertexBuffer(), MeshObj.GetIndexBuffer(), InstanceCount);

		ContextObj.UnbindConstant<Context::PIXEL>(1);
		ContextObj.UnbindConstant<Context::VERTEX>(1);
		
		ContextObj.UnbindBuffer<Context::PIXEL>(0);
		ContextObj.UnbindBuffer<Context::PIXEL>(1);
		ContextObj.UnbindBuffer<Context::PIXEL>(3);
	}

	for (int SubMeshIndex = 0; SubMeshIndex < MeshObj.GetSubMeshesCount(); ++SubMeshIndex)
	{
		Mesh& SubMesh = MeshObj.GetSubMesh(SubMeshIndex);
		_Draw(ContextObj, SubMesh, InstanceCount);
	}

	_RenderObjectsTaskData->GetTransformStack().Pop();
}

RenderOpaqueObjectsTask::RenderOpaqueObjectsTask(_In_ const GraphicTaskConfig& Config, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
	: RenderObjectsTask(Config, Contexts, RenderTargets, Samplers, Viewports, BlendStates, SharedData)
{
}

void RenderOpaqueObjectsTask::DoSetup()
{
	StateSharedData* SharedData = _RenderObjectsTaskData->GetSharedData();
	if (SharedData->Camera)
		_RenderObjectsTaskData->SetCamera(SharedData->Camera->GetCameraComponent()->GetCamera());
}

RenderObjectsShadowTask::RenderObjectsShadowTask(_In_ const GraphicTaskConfig& Config, _In_ ContextCollection& Contexts, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ BlendStateCollection& BlendStates, _In_ StateSharedData* SharedData)
	: RenderObjectsTask(Config, Contexts, RenderTargets, Samplers, Viewports, BlendStates, SharedData)
{
}

void RenderObjectsShadowTask::DoSetup()
{
	StateSharedData* SharedData = _RenderObjectsTaskData->GetSharedData();
	if (SharedData->Lights)
		_RenderObjectsTaskData->SetCamera((Camera*)SharedData->Lights[GetInstanceID()].GetLightComponent()->GetShadow());
}
