#include "Core/MeshComponent.hpp"

#include "Mesh/Mesh.hpp"

using namespace Eternal::Core;

static void FindMeshMinMax(_In_ Mesh* MeshObj, _Out_ Vector4& Min, _Out_ Vector4& Max)
{
	for (uint32_t MeshIndex = 0; MeshObj->GetSubMeshesCount(); ++MeshIndex)
	{
		FindMeshMinMax(&MeshObj->GetSubMesh(MeshIndex), Min, Max);
	}
	//MeshObj->
}

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

void MeshComponent::SetBoundingBox(_In_ Mesh* BoundingBoxObj)
{
	_BoundingBox = BoundingBoxObj;
}

Mesh* MeshComponent::GetBoundingBox()
{
	return _BoundingBox;
}
