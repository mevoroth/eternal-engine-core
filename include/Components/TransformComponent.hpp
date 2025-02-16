#pragma once

#include "Core/Component.hpp"
#include "Transform/Transform.hpp"
#include "Event/EventSender.hpp"

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

			void Begin() {}
			void Update(_In_ TimeSecondsT InDeltaSeconds);
			void End() {}
			inline const Transform& GetTransform() const { return _Transform; }
			inline Transform& GetTransform()
			{
				_ComponentState.ComponentIsDirty = true;
				return _Transform;
			}

			void SetTranslation(_In_ const Vector3& InTranslation)
			{
				GetTransform().SetTranslation(InTranslation);
			}

			const Vector3& GetTranslation() const
			{
				return GetTransform().GetTranslation();
			}

			Vector3& GetTranslation()
			{
				return GetTransform().GetTranslation();
			}

			void SetRotation(_In_ const Quaternion& InRotation)
			{
				GetTransform().SetRotation(InRotation);
			}

			const Quaternion& GetRotation() const
			{
				return GetTransform().GetRotation();
			}

			Quaternion& GetRotation()
			{
				return GetTransform().GetRotation();
			}

			void SetScaling(_In_ const Vector3& InScaling)
			{
				GetTransform().SetScaling(InScaling);
			}

			const Vector3& GetScaling() const
			{
				return GetTransform().GetScaling();
			}

			Vector3& GetScaling()
			{
				return GetTransform().GetScaling();
			}

			OnTransformChangedEventSender& OnTransformChanged() { return _OnTransformChanged; }

		private:
			Transform _Transform;
			OnTransformChangedEventSender _OnTransformChanged;
		};
	}
}
