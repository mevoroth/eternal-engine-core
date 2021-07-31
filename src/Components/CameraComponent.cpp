#include "Components/CameraComponent.hpp"
#include "Camera/Camera.hpp"
#include "Core/System.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"

namespace Eternal
{
	namespace Core
	{
		static ComponentPool<CameraComponent> CameraComponentsPool;

		void CameraOnTransformChanged::OnTransformChanged(_In_ const Transform& InTransform)
		{
			_CameraComponent->GetCamera().UpdateWorldToView(InTransform);
		}

		CameraComponent::CameraComponent()
			: _OnTransformChangedReceiver(this)
		{
			_ComponentState.ComponentHasBehavior = true;
			_ComponentState.ComponentUpdatesEveryFrame = true;
			_ComponentState.ComponentIsDirty = true;
		}

		void CameraComponent::Update(_In_ TimeSecondsT InDeltaSeconds)
		{
			System& SystemEngine = GetWorld()->GetGame().GetSystem();
			SystemEngine.GetGameFrame().PendingView = _Camera;
		}
	}
}
