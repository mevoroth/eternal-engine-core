#include "GameData/MeshGameData.hpp"

#include "include/json/json.h"
#include "Mesh/MeshFactory.hpp"

using namespace Eternal::GameData;
using namespace Eternal::Components;

void MeshGameData::Prepare(_Inout_ void* Parent /*= nullptr*/)
{

}

void MeshGameData::GetData(_Out_ uint8_t* Data) const
{

}

size_t MeshGameData::GetSize() const
{
	return 0;
}

bool MeshGameData::CanLoad(_In_ const void* SerializedData) const
{
	return false;
}

void* MeshGameData::Load(_In_ const void* SerializedData)
{
	Json::Value& MeshValue = *(Json::Value*)SerializedData;
	MeshComponent* MeshComponentObj = new MeshComponent();
	MeshComponentObj->SetMesh(CreateMesh(MeshValue.asString().c_str()));
	return MeshComponentObj;
}
