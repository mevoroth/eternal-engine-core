#ifndef _MESH_GAME_DATA_HPP_
#define _MESH_GAME_DATA_HPP_

#include "SaveSystem/GameData.hpp"
#include "Core/MeshComponent.hpp"

namespace Eternal
{
	namespace GameData
	{
		using namespace Eternal::SaveSystem;
		using namespace Eternal::Core;

		class MeshGameData : public TemplatedGameData<MeshComponent>
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
