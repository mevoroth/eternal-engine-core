#include "Task/Graphics/LightingTask.hpp"

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
#include "Core/CameraGameObject.hpp"
#include "Core/CameraComponent.hpp"
#include "Core/LightGameObject.hpp"
#include "Core/LightComponent.hpp"
#include "Core/TransformComponent.hpp"
#include "Core/StateSharedData.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "Transform/Transform.hpp"

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

			LightingTaskData(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& ShadowRenderTargets, _In_ RenderTargetCollection& LightRenderTargets,
				_In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
				: _Contexts(Contexts)
				, _OpaqueRenderTargets(OpaqueRenderTargets)
				, _ShadowRenderTargets(ShadowRenderTargets)
				, _LightRenderTargets(LightRenderTargets)
				, _SharedData(SharedData)
			{
				_VS = ShaderFactory::Get()->CreateVertexShader("Lighting", "lighting.vs.hlsl");
				_PS = ShaderFactory::Get()->CreatePixelShader("Lighting", "lighting.ps.hlsl");

				//_DepthStencil = CreateDepthStencil(DepthTest(), StencilTest(
				//	StencilTest::FaceOperator(StencilTest::KEEP, StencilTest::KEEP, StencilTest::KEEP, Comparison::NEVER),
				//	StencilTest::FaceOperator(StencilTest::REPLACE, StencilTest::REPLACE, StencilTest::REPLACE, Comparison::ALWAYS),
				//	0xFF, 0xFF
				//)); // Disable depth test
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

				delete _FrameConstants;
				_FrameConstants = nullptr;

				delete _LightConstants;
				_LightConstants = nullptr;

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

			RenderTargetCollection& GetLightRenderTargets()
			{
				return _LightRenderTargets;
			}

			RenderTargetCollection& GetShadowRenderTargets()
			{
				return _ShadowRenderTargets;
			}

		private:
			ContextCollection& _Contexts;

			Shader* _VS = nullptr;
			Shader* _PS = nullptr;

			RenderTargetCollection& _OpaqueRenderTargets;
			RenderTargetCollection& _LightRenderTargets;
			RenderTargetCollection& _ShadowRenderTargets;
			Viewport* _Viewport = nullptr;

			DepthStencil* _DepthStencil = nullptr;

			Constant* _FrameConstants = nullptr;
			Constant* _LightConstants = nullptr;

			Sampler* _Sampler = nullptr;

			StateSharedData* _SharedData = nullptr;
		};
	}
}

LightingTask::LightingTask(_In_ ContextCollection& Contexts, _In_ RenderTargetCollection& OpaqueRenderTargets, _In_ RenderTargetCollection& ShadowRenderTargets, _In_ RenderTargetCollection& LightRenderTargets,
	_In_ SamplerCollection& Samplers, _In_ ViewportCollection& Viewports, _In_ StateSharedData* SharedData)
{
	_LightingTaskData = new LightingTaskData(Contexts, OpaqueRenderTargets, ShadowRenderTargets, LightRenderTargets, Samplers, Viewports, SharedData);
}

LightingTask::~LightingTask()
{
	delete _LightingTaskData;
	_LightingTaskData = nullptr;
}

void LightingTask::DoSetup()
{
}

void LightingTask::DoExecute()
{
	if (!_LightingTaskData->GetSharedData()->Camera)
	{
		SetState(DONE);
		return;
	}

	RenderTargetCollection& LightRenderTargets = _LightingTaskData->GetLightRenderTargets();
	RenderTargetCollection& OpaqueRenderTargets = _LightingTaskData->GetOpaqueRenderTargets();
	RenderTargetCollection& ShadowRenderTargets = _LightingTaskData->GetShadowRenderTargets();
	//RenderTarget* NullRenderTarget[] = { nullptr };
	RenderTarget* NullRenderTarget[] = { nullptr, nullptr };

	Context& ContextObj = _LightingTaskData->GetContexts().Get();
	ContextObj.Begin();

	_SetupFrameConstants(ContextObj);
	_SetupLightConstants(ContextObj);

	ContextObj.SetTopology(Context::TRIANGLELIST);
	ContextObj.SetViewport(_LightingTaskData->GetViewport());

	ContextObj.BindDepthStencilState(_LightingTaskData->GetDepthStencil());

	ContextObj.BindConstant<Context::PIXEL>(0, _LightingTaskData->GetFrameConstants());
	ContextObj.BindConstant<Context::PIXEL>(1, _LightingTaskData->GetLightConstants());
	ContextObj.BindSampler<Context::PIXEL>(0, _LightingTaskData->GetSampler());

	ContextObj.BindBuffer<Context::PIXEL>(0, OpaqueRenderTargets.GetDepthStencilRenderTarget()->GetAsResource());
	ContextObj.BindBuffer<Context::PIXEL>(1, OpaqueRenderTargets.GetRenderTargets()[0]->GetAsResource());
	ContextObj.BindBuffer<Context::PIXEL>(2, OpaqueRenderTargets.GetRenderTargets()[1]->GetAsResource());
	ContextObj.BindBuffer<Context::PIXEL>(3, OpaqueRenderTargets.GetRenderTargets()[3]->GetAsResource());
	ContextObj.BindBuffer<Context::PIXEL>(4, OpaqueRenderTargets.GetRenderTargets()[4]->GetAsResource());
	ContextObj.BindBuffer<Context::PIXEL>(5, OpaqueRenderTargets.GetRenderTargets()[5]->GetAsResource());

	ContextObj.BindBuffer<Context::PIXEL>(6, ShadowRenderTargets.GetDepthStencilRenderTarget()->GetAsResource());

	ContextObj.BindBuffer<Context::PIXEL>(10, OpaqueRenderTargets.GetRenderTargets()[4]->GetAsResource()); // Debug World Position

	ContextObj.BindShader<Context::VERTEX>(_LightingTaskData->GetVS());
	ContextObj.BindShader<Context::PIXEL>(_LightingTaskData->GetPS());

	ContextObj.SetRenderTargets(LightRenderTargets.GetRenderTargets(), LightRenderTargets.GetRenderTargetsCount()); // REMOVE THIS
	ContextObj.DrawPrimitive(6);
	ContextObj.SetRenderTargets(NullRenderTarget, ETERNAL_ARRAYSIZE(NullRenderTarget)); // REMOVE THIS

	ContextObj.UnbindShader<Context::VERTEX>();
	ContextObj.UnbindShader<Context::PIXEL>();

	ContextObj.UnbindBuffer<Context::PIXEL>(0);
	ContextObj.UnbindBuffer<Context::PIXEL>(1);
	ContextObj.UnbindBuffer<Context::PIXEL>(2);
	ContextObj.UnbindBuffer<Context::PIXEL>(3);
	ContextObj.UnbindBuffer<Context::PIXEL>(4);
	ContextObj.UnbindBuffer<Context::PIXEL>(5);

	ContextObj.UnbindBuffer<Context::PIXEL>(6);

	ContextObj.UnbindBuffer<Context::PIXEL>(10);

	ContextObj.UnbindSampler<Context::PIXEL>(0);
	ContextObj.UnbindConstant<Context::PIXEL>(0);
	ContextObj.UnbindConstant<Context::PIXEL>(1);

	ContextObj.UnbindDepthStencilState();

	ContextObj.End();
	_LightingTaskData->GetContexts().Release(ContextObj);
}

void LightingTask::DoReset()
{
}

void LightingTask::_SetupFrameConstants(_In_ Context& ContextObj)
{
	Camera* CameraObj = _LightingTaskData->GetSharedData()->Camera->GetCameraComponent()->GetCamera();
	const Vector3& CameraPosition = CameraObj->GetPosition();
	Resource* ConstantResource = _LightingTaskData->GetFrameConstants()->GetAsResource();
	Resource::LockedResource FrameConstantsRaw = ConstantResource->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CB0FrameConstants& FrameConstants = *(CB0FrameConstants*)FrameConstantsRaw.Data;
	CameraObj->GetViewProjectionMatrixTransposed(FrameConstants.ViewProjection);
	CameraObj->GetViewProjectionMatrixInverseTransposed(FrameConstants.ViewProjectionInverse);
	FrameConstants.CameraPosition.x = CameraPosition.x;
	FrameConstants.CameraPosition.y = CameraPosition.y;
	FrameConstants.CameraPosition.z = CameraPosition.z;
	FrameConstants.CameraPosition.w = 1.f;
	ConstantResource->Unlock(ContextObj);
}

void LightingTask::_SetupLightConstants(_In_ Context& ContextObj)
{
	Light* PointLight = _LightingTaskData->GetSharedData()->Lights->GetLightComponent()->GetLight();
	Shadow* ShadowObj = _LightingTaskData->GetSharedData()->Lights->GetLightComponent()->GetShadow();
	const Transform& TransformObj = _LightingTaskData->GetSharedData()->Lights->GetTransformComponent()->GetTransform();

	Matrix4x4 ShadowMatrix;
	Matrix4x4 InvCameraMatrix;

	_LightingTaskData->GetSharedData()->Camera->GetCameraComponent()->GetCamera()->GetViewProjectionMatrixInverse(InvCameraMatrix);
	ShadowObj->GetViewProjectionMatrix(ShadowMatrix);
	
	ShadowMatrix = InvCameraMatrix * ShadowMatrix;
	Transpose(ShadowMatrix);

	Resource* ConstantObj = _LightingTaskData->GetLightConstants()->GetAsResource();
	Resource::LockedResource LightConstantsRaw = ConstantObj->Lock(ContextObj, Resource::LOCK_WRITE_DISCARD);
	CB1LightConstants& LightConstants = *(CB1LightConstants*)LightConstantsRaw.Data;
	
	const Vector3& Color = PointLight->GetColor();
	const Vector3& Position = TransformObj.GetTranslation();

	// TODO: MAKE IT GENERIC
	LightConstants.Lights[0].Position = Vector4(Position.x, Position.y, Position.z, 1.0f);
	LightConstants.Lights[0].Color = Vector4(Color.x, Color.y, Color.z, 1.0f);
	LightConstants.Lights[0].Intensity = PointLight->GetIntensity();
	LightConstants.Lights[0].Distance = PointLight->GetDistance();

	LightConstants.Lights[0].LightToCamera = ShadowMatrix;

	LightConstants.LightsCount = 1;

	ConstantObj->Unlock(ContextObj);
}
