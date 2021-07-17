#include "GameData/GraphicGameObjectInstanceGameData.hpp"

#include "include/json/json.h"
#include "GameData/TransformGameData.hpp"
#include "GameData/GameDatas.hpp"

using namespace Eternal::GameData;

void GraphicGameObjectInstanceGameData::Prepare(void* Parent)
{
}

void GraphicGameObjectInstanceGameData::GetData(uint8_t* Data) const
{
}

size_t GraphicGameObjectInstanceGameData::GetSize() const
{
	return size_t();
}

bool GraphicGameObjectInstanceGameData::CanLoad(_In_ const void* SerializedData) const
{
	Json::Value& GameObjectNode = *(Json::Value*)SerializedData;
	return GameObjectNode["Type"] == GameDatas::GRAPHIC_GAME_OBJECT_INSTANCE;
}

void* GraphicGameObjectInstanceGameData::Load(const void* SerializedData)
{
	Json::Value& GameObjectNode = *(Json::Value*)SerializedData;
	GraphicGameObjectInstance* GraphicObjectInstance = new GraphicGameObjectInstance();

	GameDatas* GameDatasObj = GameDatas::Get();
	TransformComponent* TransformComponentObj = (TransformComponent*)GameDatasObj->Get(GameDatas::TRANSFORM_COMPONENT)->Load(&GameObjectNode["Transform"]);
	
	GraphicObjectInstance->CopyTransformComponent(TransformComponentObj);
	
	delete TransformComponentObj;
	TransformComponentObj = nullptr;

	return GraphicObjectInstance;
}
