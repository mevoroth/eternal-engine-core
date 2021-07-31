#include "Components/TransformComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		static ComponentPool<TransformComponent> TransformComponentsPool;

		TransformComponent::TransformComponent(_In_ World* InWorld)
			: Component(InWorld)
			, _OnTransformChanged(&OnTransformChangedEvent::OnTransformChanged)
		{
		}

		void TransformComponent::Update(_In_ TimeSecondsT InDeltaSeconds)
		{
			_OnTransformChanged.Notify(static_cast<const Transform&>(_Transform));
		}
	}
}
