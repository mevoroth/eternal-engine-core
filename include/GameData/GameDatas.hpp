#ifndef _GAME_DATAS_HPP_
#define _GAME_DATAS_HPP_

#include "SaveSystem/GameDataLoader.hpp"

namespace Eternal
{
	namespace SaveSystem
	{
		class GameData;
	}

	namespace GameData
	{
		using namespace Eternal::SaveSystem;

		class GameDatas : public GameDataLoader
		{
		public:
			enum GameDatasKey
			{
				GRAPHIC_GAME_OBJECT = 0,
				TRANSFORM_COMPONENT = 1,
				MESH_COMPONENT		= 2,
				MATERIAL_COMPONENT	= 3,
				GAME_DATA_COUNT
			};

			static GameDatas* Get();

			GameDatas();
			~GameDatas();

			Eternal::SaveSystem::GameData* Get(const GameDatasKey& Key);
			virtual Eternal::SaveSystem::GameData* FindLoader(_In_ const void* SerializedData) override;

		private:
			static GameDatas* _Inst;

			Eternal::SaveSystem::GameData* _GameDatas[GAME_DATA_COUNT];
		};
	}
}

#endif
