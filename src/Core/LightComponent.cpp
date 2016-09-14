#include "Core/LightComponent.hpp"

#include "Light/Light.hpp"

using namespace Eternal::Core;

Light* LightComponent::GetLight()
{
	return _Light;
}
