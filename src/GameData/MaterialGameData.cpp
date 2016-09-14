#include "GameData/MaterialGameData.hpp"

#include "Macros/Macros.hpp"
#include "Core/MaterialComponent.hpp"
#include "GraphicData/Material.hpp"

using namespace Eternal::GameData;
using namespace Eternal::Core;

void MaterialGameData::Prepare(_Inout_ void* Parent /*= nullptr*/)
{
	ETERNAL_ASSERT(false);
}
void MaterialGameData::GetData(_Out_ uint8_t* Data) const
{
	ETERNAL_ASSERT(false);
}
size_t MaterialGameData::GetSize() const
{
	ETERNAL_ASSERT(false);
	return 0;
}
bool MaterialGameData::CanLoad(_In_ const void* SerializedData) const
{
	return false;
}
void* MaterialGameData::Load(_In_ const void* SerializedData)
{
	MaterialComponent* MaterialComponentObj = new MaterialComponent();
	Material* MaterialObj = new Material();
	MaterialComponentObj->SetMaterial(MaterialObj);
	return MaterialComponentObj;
}
