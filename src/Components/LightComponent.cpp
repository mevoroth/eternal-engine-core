#include "Components/LightComponent.hpp"

#include "Log/Log.hpp"
#include "Light/Light.hpp"
#include "Camera/OrthographicCamera.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"

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
			SetHasBehavior();
		}

		void LightComponent::Begin()
		{
			ETERNAL_ASSERT(_Light);
			ETERNAL_ASSERT(_Light->GetLightType() != LightType::LIGHT_TYPE_INVALID);

			System& EngineSystem = GetWorld()->GetGame().GetSystem();
			EngineSystem.GetGameFrame().Lights.AddObject(_Light, nullptr);

			if (_Light->GetLightType() == LightType::LIGHT_TYPE_DIRECTIONAL)
			{
				LogWrite(LogInfo, LogComponents, "Hack on directional light");

				const_cast<Shadow&>(_Light->GetShadow()).ShadowCamera = new OrthographicCamera(
					0.0f, 25600.0f,
					8.0f, 8.0f
				);

				GetParent()->GetComponent<TransformComponent>()->SetIsDirty();
			}
		}
	}
}
