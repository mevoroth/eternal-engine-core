#include "Core/LightComponent.hpp"

#include "Resources/Pool.hpp"
#include "Light/Light.hpp"
#include "Core/TransformComponent.hpp"

using namespace Eternal::Core;
using namespace Eternal::Resources;
using namespace Eternal::Components;

Pool<LightComponent, 128>* g_LightComponentPool = nullptr;

void LightComponent::Initialize()
{
	g_LightComponentPool = new Pool<LightComponent, 128>();
}

void LightComponent::Release()
{
	delete g_LightComponentPool;
	g_LightComponentPool = nullptr;
}

void LightComponent::SetLight(_In_ Light* LightObj)
{
	_Light = LightObj;
}

Light* LightComponent::GetLight()
{
	return _Light;
}

void LightComponent::SetShadow(_In_ Shadow* ShadowObj)
{
	_Shadow = ShadowObj;
}

Shadow* LightComponent::GetShadow()
{
	return _Shadow;
}

bool LightComponent::GetShadowEnabled() const
{
	return _Shadow != nullptr;
}

void LightComponent::AttachTo(_In_ TransformComponent* TransformComponentObj)
{
	_TransformComponent = TransformComponentObj;
}

void LightComponent::Begin()
{

}

void LightComponent::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
	_Shadow->UpdateView(_TransformComponent->GetTransform());
}

void LightComponent::End()
{

}