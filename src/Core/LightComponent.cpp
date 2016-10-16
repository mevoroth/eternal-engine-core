#include "Core/LightComponent.hpp"

#include "Resources/Pool.hpp"
#include "Light/Light.hpp"

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
