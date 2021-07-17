#ifndef _TRANSFORM_GAME_DATA_HPP_
#define _TRANSFORM_GAME_DATA_HPP_

#include "SaveSystem/GameData.hpp"
#include "Core/TransformComponent.hpp"

namespace Eternal
{
	namespace GameData
	{
		using namespace Eternal::SaveSystem;
		using namespace Eternal::Core;

		class TransformGameDataCache;

		class TransformGameData : public TemplatedGameData<TransformComponent>
		{
		public:
			TransformGameData(_In_ TransformComponent& Component);
			~TransformGameData();
			virtual void Prepare(_In_ void* Parent) override;
			virtual void GetData(_Out_ uint8_t* Data) const override;
			virtual size_t GetSize() const override;
			virtual bool CanLoad(_In_ const void* SerializedData) const override;
			virtual void* Load(_In_ const void* SerializedData) override;

		private:
			TransformGameDataCache* _Cache;
			TransformComponent& _Component;
		};
	}
}

#endif
