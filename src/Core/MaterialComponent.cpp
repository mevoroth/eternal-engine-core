#include "Core/MaterialComponent.hpp"

using namespace Eternal::Core;

void MaterialComponent::Begin()
{

}

void MaterialComponent::Update(_In_ const TimeT& ElapsedMicroSeconds)
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
