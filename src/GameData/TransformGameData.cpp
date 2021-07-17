#include "GameData/TransformGameData.hpp"
#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;

		void* TransformGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			TransformComponent* OutComponent = new TransformComponent();
			InNode.Get(OutComponent->GetTransform());
			return OutComponent;
		}
	}
}
