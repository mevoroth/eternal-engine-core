#include "Task/Graphics/LightingTask.hpp"

#include "Macros/Macros.hpp"
#include "Types/Types.hpp"

#include "Graphics/DepthStencil.hpp"
#include "Graphics/DepthStencilFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Constant.hpp"
#include "Graphics/ConstantFactory.hpp"
#include "Graphics/Resource.hpp"
#include "Graphics/Viewport.hpp"
#include "Graphics/RenderTarget.hpp"
#include "GraphicData/RenderTargetCollection.hpp"
#include "GraphicData/ContextCollection.hpp"
#include "GraphicData/SamplerCollection.hpp"
#include "GraphicData/ViewportCollection.hpp"
#include "ShadersStructs/CB0FrameConstants.hpp"
#include "ShadersStructs/CB1LightConstants.hpp"
#include "Camera/Camera.hpp"
#include "Core/CameraGameObject.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/StateSharedData.hpp"

using namespace Eternal::Task;
using namespace Eternal::GraphicData;
using namespace Eternal::Shaders;
using namespace Eternal::Components;

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Types;
		using namespace Eternal::Graphics;

		class LightingTaskData
		{
		public:
			struct Light
			{
				Vector4 Position;
				Vector4 Color;
				float Distance;
				float Intensity;
			};

			struct LightsConstants
			{
				Light Lights[8];
			};

			LightingTaskData(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
				: _Contexts(Contexts)
				, _OpaqueRenderTargets(OpaqueRenderTargets)
				, _RenderTargets(RenderTargets)
				, _SharedData(SharedData)
			{
				_VS = ShaderFactory::Get()->CreateVertexShader("Lighting", "lighting.vs.hlsl");
				_PS = ShaderFactory::Get()->CreatePixelShader("Lighting", "lighting.ps.hlsl");

				_DepthStencil = CreateDepthStencil(DepthTest(), StencilTest()); // Disable depth test
				_FrameConstants = CreateConstant(sizeof(CB0FrameConstants), Resource::DYNAMIC, Resource::WRITE);
				_LightConstants = CreateConstant(sizeof(CB1LightConstants), Resource::DYNAMIC, Resource::WRITE);

				_Sampler = Samplers.GetSampler(SamplerCollection::BILINEAR);
				_Viewport = Viewports.GetViewport(ViewportCollection::FULLSCREEN);
			}

			~LightingTaskData()
			{
				delete _VS;
				_VS = nullptr;
				delete _PS;
				_PS = nullptr;
				delete _DepthStencil;
				_DepthStencil = nullptr;

				_SharedData = nullptr;
			}

			ContextCollection& GetContexts()
			{
				return _Contexts;
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

			DepthStencil* GetDepthStencil()
			{
				ETERNAL_ASSERT(_DepthStencil);
				return _DepthStencil;
			}

			StateSharedData* GetSharedData()
			{
				ETERNAL_ASSERT(_SharedData);
				return _SharedData;
			}

			Constant* GetLightConstants()
			{
				ETERNAL_ASSERT(_LightConstants);
				return _LightConstants;
			}

			Constant* GetFrameConstants()
			{
				ETERNAL_ASSERT(_FrameConstants);
				return _FrameConstants;
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

			RenderTargetCollection& GetOpaqueRenderTargets()
			{
				return _OpaqueRenderTargets;
			}

			RenderTargetCollection& GetRenderTargets()
			{
				return _RenderTargets;
			}

		private:
			ContextCollection& _Contexts;

			Shader* _VS = nullptr;
			Shader* _PS = nullptr;

			RenderTargetCollection& _OpaqueRenderTargets;
			RenderTargetCollection& _RenderTargets;
			Viewport* _Viewport = nullptr;

			DepthStencil* _DepthStencil = nullptr;

			Constant* _FrameConstants = nullptr;
			Constant* _LightConstants = nullptr;

			Sampler* _Sampler = nullptr;

			StateSharedData* _SharedData = nullptr;
		};
	}
}

LightingTask::LightingTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& RenderTargets, _In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
{
	_LightingTaskData = new LightingTaskData(Contexts, OpaqueRenderTargets, RenderTargets, Samplers, Viewports, SharedData);
}

LightingTask::~LightingTask()
{
	delete _LightingTaskData;
	_LightingTaskData = nullptr;
}

void LightingTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void LightingTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	RenderTargetCollection& RenderTargets = _LightingTaskData->GetRenderTargets();
	RenderTargetCollection& OpaqueRenderTargets = _LightingTaskData->GetOpaqueRenderTargets();
	RenderTarget* NullRenderTarget = nullptr;

	Context& ContextObj = _LightingTaskData->GetContexts().Get();
	ContextObj.Begin();

	_SetupFrameConstants(ContextObj);
	_SetupLightConstants(ContextObj);

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetViewport(_LightingTaskData->GetViewport());
	
	ContextObj.BindConstant<Context::PIXEL>(0, _LightingTaskData->GetFrameConstants());
	ContextObj.BindConstant<Context::PIXEL>(1, _LightingTaskData->GetLightConstants());
	ContextObj.BindSampler<Context::PIXEL>(0, _LightingTaskData->GetSampler());

	ContextObj.BindBuffer<Context::PIXEL>(0, RenderTargets.GetDepthStencilRenderTarget()->GetAsResource());

	ContextObj.BindShader<Context::VERTEX>(_LightingTaskData->GetVS());
	ContextObj.BindShader<Context::PIXEL>(_LightingTaskData->GetPS());

	ContextObj.SetRenderTargets(RenderTargets.GetRenderTargets(), RenderTargets.GetRenderTargetsCount()); // REMOVE THIS
	ContextObj.DrawPrimitive(6);
	ContextObj.SetRenderTargets(&NullRenderTarget, 1); // REMOVE THIS

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();

	ContextObj.UnbindBuffer<Context::PIXEL>(0);

	ContextObj.UnbindSampler<Context::PIXEL>(0);
	ContextObj.UnbindConstant<Context::PIXEL>(0);
	ContextObj.UnbindConstant<Context::PIXEL>(1);

	ContextObj.End();
	_LightingTaskData->GetContexts().Release(ContextObj);

	SetState(DONE);
}

void LightingTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}

void LightingTask::_SetupFrameConstants(_In_ Context& ContextObj)
{
	Camera* CameraObj = _LightingTaskData->GetSharedData()->Camera->GetCameraComponent()->GetCamera();
	Resource* ConstantResource = _LightingTaskData->GetFrameConstants()->GetAsResource();
	Resource::LockedResource FrameConstants = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CameraObj->GetViewProjectionMatrixTransposed(((CB0FrameConstants*)FrameConstants.Data)->ViewProjection);
	ConstantResource->Unlock(ContextObj);
}

void LightingTask::_SetupLightConstants(_In_ Context& ContextObj)
{
	Resource* ConstantObj = _LightingTaskData->GetLightConstants()->GetAsResource();
	Resource::LockedResource LightConstantsRaw = ConstantObj->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CB1LightConstants LightConstants = *(CB1LightConstants*)LightConstantsRaw.Data;
	
	// TODO: MAKE IT GENERIC
	LightConstants.Lights[0].Position = Vector4(210.0f, 5.0f, 600.0f, 1.0f);
	LightConstants.Lights[0].Color = Vector4(1.0f, 0.5f, 80.0f / 255.0f, 1.0);
	LightConstants.Lights[0].Intensity = 10.0f;
	LightConstants.Lights[0].Distance = 100.0f;

	LightConstants.LightsCount = 1;

	ConstantObj->Unlock(ContextObj);
}
