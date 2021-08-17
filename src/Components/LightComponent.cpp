#include "Components/LightComponent.hpp"
#include "Light/Light.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"

namespace Eternal
{
	namespace Core
	{
		void LightOnTransformChanged::OnTransformChanged(_In_ const Transform& InTransform)
		{
			_LightComponent->GetLight().UpdateWorldToView(InTransform);
		}

		LightComponent::LightComponent(_In_ World* InWorld /* = nullptr */)
			: Component(InWorld)
			, _OnTransformChangedReceiver(this)
		{
			_ComponentState.ComponentHasBehavior = true;
		}

		void LightComponent::Begin()
		{
			System& EngineSystem = GetWorld()->GetGame().GetSystem();
			EngineSystem.GetGameFrame().Lights.Add(_Light);
		}
	}
}
