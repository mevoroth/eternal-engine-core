#include "Core/MaterialComponent.hpp"

using namespace Eternal::Core;

void MaterialComponent::Begin()
{

}

void MaterialComponent::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{

}

void MaterialComponent::End()
{

}

Material* MaterialComponent::GetMaterial()
{
	return _Material;
}

void MaterialComponent::SetMaterial(_In_ Material* MaterialObj)
{
	_Material = MaterialObj;
}
