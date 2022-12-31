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

		CameraComponent::CameraComponent(_In_ World* InWorld /* = nullptr */)
			: Component(InWorld)
			, _OnTransformChangedReceiver(this)
		{
			_ComponentState.ComponentHasBehavior = true;
		}

		void CameraComponent::Begin()
		{
			System& SystemEngine = GetWorld()->GetGame().GetSystem();
			SystemEngine.GetGameFrame().PendingViewCamera = _Camera;
		}
	}
}
