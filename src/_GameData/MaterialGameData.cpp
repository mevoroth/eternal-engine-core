#include "GameData/MaterialGameData.hpp"

#include "Core/MaterialComponent.hpp"
#include "GraphicData/Material.hpp"

#include "Log/Log.hpp"

using namespace Eternal::GameData;
using namespace Eternal::Core;

void MaterialGameData::Prepare(_Inout_ void* Parent /*= nullptr*/)
{
	ETERNAL_BREAK();
}
void MaterialGameData::GetData(_Out_ uint8_t* Data) const
{
	ETERNAL_BREAK();
}
size_t MaterialGameData::GetSize() const
{
	ETERNAL_BREAK();
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
	//vector<MaterialProperty*>* Properties = new vector<MaterialProperty*>();
	//MaterialObj->SetProperties(Properties);
	//ETERNAL_BREAK();
	Eternal::Log::Log::Get()->Write(Eternal::Log::Log::Warning, Eternal::Log::Log::Save, "[MaterialGameData::Load]Material properties not implemented!");
	MaterialComponentObj->SetMaterial(MaterialObj);
	return MaterialComponentObj;
}
