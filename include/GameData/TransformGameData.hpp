#ifndef _TRANSFORM_GAME_DATA_HPP_
#define _TRANSFORM_GAME_DATA_HPP_

#include "SaveSystem/GameData.hpp"

namespace Eternal
{
	namespace Core
	{
		class TransformComponent;
	}
	namespace GameData
	{
		using namespace Eternal::SaveSystem;
		using namespace Eternal::Core;

		class TransformGameDataCache;

		class TransformGameData : public GameData
		{
		public:
			TransformGameData(_In_ TransformComponent& Component);
			~TransformGameData();
			virtual void Prepare(_In_ void* Parent) override;
			virtual void GetData(_Out_ uint8_t* Data) const override;
			virtual size_t GetSize() const override;

		private:
			TransformGameDataCache* _Cache;
			TransformComponent& _Component;
		};
	}
}

#endif
