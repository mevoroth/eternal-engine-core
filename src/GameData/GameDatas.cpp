#include "GameData/GameDatas.hpp"

#include "Macros/Macros.hpp"
#include "Log/Log.hpp"
#include "GameData/GraphicGameObjectGameData.hpp"
#include "GameData/GraphicGameObjectInstanceGameData.hpp"
#include "Core/TransformComponent.hpp"
#include "GameData/TransformGameData.hpp"
#include "GameData/MeshGameData.hpp"
#include "GameData/MaterialGameData.hpp"
#include "SaveSystem/GameDataCollection.hpp"

using namespace Eternal::GameData;

GameDatas* GameDatas::_Inst = nullptr;

GameDatas::GameDatas()
{
	ETERNAL_ASSERT(!_Inst);

	for (int GameDataIndex = 0; GameDataIndex < GAME_DATA_COUNT; ++GameDataIndex)
	{
		_GameDatas[GameDataIndex] = nullptr;
	}

	GraphicGameObject* EmptyGameObject = new GraphicGameObject();
	_GameDatas[GRAPHIC_GAME_OBJECT] = new GraphicGameObjectGameData(*EmptyGameObject);

	TransformComponent* DefaultTransformComponent = new TransformComponent();
	_GameDatas[TRANSFORM_COMPONENT] = new TransformGameData(*DefaultTransformComponent);

	//MeshComponent* EmptyMeshComponent = new MeshComponent();
	_GameDatas[MESH_COMPONENT] = new MeshGameData();

	_GameDatas[MATERIAL_COMPONENT] = new MaterialGameData();

	_GameDatas[GRAPHIC_GAME_OBJECT_INSTANCE] = new GraphicGameObjectInstanceGameData();

	_GameDatas[GAME_DATA_COLLECTION] = new GameDataCollection();

	_Inst = this;
}

GameDatas::~GameDatas()
{
	ETERNAL_ASSERT(_Inst);
	_Inst = nullptr;
}

GameDatas* GameDatas::Get()
{
	ETERNAL_ASSERT(_Inst);
	return _Inst;
}

Eternal::SaveSystem::GameData* GameDatas::Get(const GameDatasKey& Key)
{
	ETERNAL_ASSERT(Key < GAME_DATA_COUNT);
	ETERNAL_ASSERT(_GameDatas[Key]);
	return _GameDatas[Key];
}

Eternal::SaveSystem::GameData* GameDatas::FindLoader(_In_ const void* SerializedData)
{
	for (int GameDataIndex = 0; GameDataIndex < GAME_DATA_COUNT; ++GameDataIndex)
	{
		if (_GameDatas[GameDataIndex]->CanLoad(SerializedData))
		{
			return _GameDatas[GameDataIndex];
		}
	}
	Log::Log::Get()->Write(Log::Log::Error, Log::Log::Save, "[GameDatas::FindLoader][1/2]No loader found");
	Log::Log::Get()->Write(Log::Log::Error, Log::Log::Save, "[GameDatas::FindLoader][2/2]Data might be corrupted");
	ETERNAL_ASSERT(false); // Unknown type
	return nullptr;
}
