#include "GameData/LightGameData.hpp"

#include "Components/LightComponent.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;

		void* LightGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			LightComponent* OutComponent = new LightComponent();
			InNode.Get(*OutComponent);
			return OutComponent;
		}
	}
}
