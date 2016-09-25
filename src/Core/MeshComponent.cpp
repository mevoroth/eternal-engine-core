#include "Core/MeshComponent.hpp"

using namespace Eternal::Core;

void MeshComponent::Begin()
{

}

void MeshComponent::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{

}

void MeshComponent::End()
{

}

void MeshComponent::SetMesh(_In_ Mesh* MeshObj)
{
	_Mesh = MeshObj;
}

Mesh* MeshComponent::GetMesh()
{
	return _Mesh;
}
