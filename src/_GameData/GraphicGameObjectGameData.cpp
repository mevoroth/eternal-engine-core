#include "GameData/GraphicGameObjectGameData.hpp"

#include "include/json/json.h"
#include "GameData/TransformGameData.hpp"
#include "GameData/GameDatas.hpp"

using namespace Eternal::GameData;

GraphicGameObjectGameData::GraphicGameObjectGameData(_In_ GraphicGameObject& GameObject)
	: _GameObject(GameObject)
{
}

void GraphicGameObjectGameData::Prepare(void* Parent)
{
	Json::Value& GameObjectNode = *(Json::Value*)Parent;
	ETERNAL_BREAK(); // IMPLEMENTATION CHANGED OUT OF DATE
	//TransformGameData TransformComponentObject(*_GameObject.GetTransformComponent());
	//TransformComponentObject.Prepare(&GameObjectNode["Transform"]);
}

void GraphicGameObjectGameData::GetData(uint8_t* Data) const
{
}

size_t GraphicGameObjectGameData::GetSize() const
{
	return size_t();
}

bool GraphicGameObjectGameData::CanLoad(_In_ const void* SerializedData) const
{
	Json::Value& GameObjectNode = *(Json::Value*)SerializedData;
	return GameObjectNode["Type"] == GameDatas::GRAPHIC_GAME_OBJECT;
}

void* GraphicGameObjectGameData::Load(const void* SerializedData)
{
	Json::Value& GameObjectNode = *(Json::Value*)SerializedData;
	GraphicGameObject* GameObject = new GraphicGameObject();
	GameDatas* GameDatasObj = GameDatas::Get();
	vector<GraphicGameObjectInstance*>* Instances = (vector<GraphicGameObjectInstance*>*)GameDatasObj->Get(GameDatas::GAME_DATA_COLLECTION)->Load(&GameObjectNode["Instances"]);

	for (int InstanceIndex = 0; InstanceIndex < Instances->size(); ++InstanceIndex)
	{
		GameObject->AddInstance((*Instances)[InstanceIndex]);
	}

	//TransformComponent* TransformComponentObj = (TransformComponent*)GameDatasObj->Get(GameDatas::TRANSFORM_COMPONENT)->Load(&GameObjectNode["Transform"]);
	
	//GameObject->CopyTransformComponent(TransformComponentObj);
	GameObject->SetMeshComponent((MeshComponent*)GameDatasObj->Get(GameDatas::MESH_COMPONENT)->Load(&GameObjectNode["Mesh"]));
	GameObject->SetMaterialComponent((MaterialComponent*)GameDatasObj->Get(GameDatas::MATERIAL_COMPONENT)->Load(&GameObjectNode["Material"]));

	//delete TransformComponentObj;

	return GameObject;
}
