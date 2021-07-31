#pragma once

#include "Core/Component.hpp"
#include "Transform/Transform.hpp"
#include "Event/EventSender.h"

namespace Eternal
{
	namespace Core
	{
		using namespace Eternal::Components;
		using namespace Eternal::Event;

		struct OnTransformChangedEvent
		{
			virtual void OnTransformChanged(_In_ const Transform& InTransform) = 0;
		};

		using OnTransformChangedEventFunction = void (OnTransformChangedEvent::*)(_In_ const Transform& InTransform);

		class TransformComponent : public Component
		{
		public:

			using OnTransformChangedEventSender = EventSender<OnTransformChangedEvent, OnTransformChangedEventFunction>;

			TransformComponent(_In_ World* InWorld = nullptr);

			void Update(_In_ TimeSecondsT InDeltaSeconds);
			inline const Transform& GetTransform() const { return _Transform; }
			inline Transform& GetTransform()
			{
				_ComponentState.ComponentIsDirty = true;
				return _Transform;
			}

			OnTransformChangedEventSender& OnTransformChanged() { return _OnTransformChanged; }

		private:
			Transform _Transform;
			OnTransformChangedEventSender _OnTransformChanged;
		};
	}
}
