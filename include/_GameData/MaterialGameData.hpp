#ifndef _MATERIAL_GAME_DATA_HPP_
#define _MATERIAL_GAME_DATA_HPP_

#include "SaveSystem/GameData.hpp"

namespace Eternal
{
	namespace GameData
	{
		using namespace Eternal::SaveSystem;

		class MaterialGameData : public GameData
		{
		public:
			virtual void Prepare(_Inout_ void* Parent = nullptr) override;
			virtual void GetData(_Out_ uint8_t* Data) const override;
			virtual size_t GetSize() const override;
			virtual bool CanLoad(_In_ const void* SerializedData) const override;
			virtual void* Load(_In_ const void* SerializedData) override;
		};
	}
}

#endif
