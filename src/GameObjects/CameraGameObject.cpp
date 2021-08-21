#include "GameObjects/CameraGameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Core/World.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "Input/Input.hpp"
#include "Math/Math.hpp"
#include <algorithm>
namespace Eternal
{
	namespace GameObjects
	{
		using namespace Eternal::Components;
		using namespace Eternal::InputSystem;
		using namespace Eternal::Math;

		CameraGameObject::CameraGameObject()
			: GameObject()
		{
			_ObjectState.ObjectUpdatesEveryFrame = true;
		}

		void CameraGameObject::Update(const TimeSecondsT InDeltaSeconds)
		{
			Input& InputSystem = GetWorld()->GetGame().GetSystem().GetInput();

			TransformComponent* Component = static_cast<TransformComponent*>(GetComponent<TransformComponent>());

			Transform CameraTransform = Component->GetTransform();
			CameraTransform.SetTranslation(Vector3::Zero);

			Matrix4x4 ViewToWorld = CameraTransform.GetViewToWorld();
			Vector4 Forward	= ViewToWorld * Vector4::ForwardPosition;
			Vector4 Right	= ViewToWorld * Vector4::RightPosition;

			Vector3 ForwardVector(Forward.x, Forward.y, Forward.z);
			Vector3 RightVector(Right.x, Right.y, Right.z);

			float LX = InputSystem.GetAxis(Input::JOY0_LX);
			float LY = InputSystem.GetAxis(Input::JOY0_LY);
			float AbsLX = Abs(LX);
			float AbsLY = Abs(LY);
			float LXWithDeadZone = Sign(LX) * (AbsLX < 0.1f ? 0.0f : AbsLX);
			float LYWithDeadZone = Sign(LY) * (AbsLY < 0.1f ? 0.0f : AbsLY);

			Component->GetTransform().Translate(
				RightVector * LXWithDeadZone +
				ForwardVector * LYWithDeadZone
			);

			//Transform->GetTransform().Rotate(

			//);
		}

		void CameraGameObject::UpdateDebug()
		{
			Input& InputSystem = GetWorld()->GetGame().GetSystem().GetInput();

			TransformComponent* Component = static_cast<TransformComponent*>(GetComponent<TransformComponent>());

			ImGui::Begin("Camera");
			ImGui::Text("Input X: [%.3f] / Input Y: [%.3f]", InputSystem.GetAxis(Input::JOY0_LX), InputSystem.GetAxis(Input::JOY0_LY));
			ImGui::InputFloat3("Camera Position:", &Component->GetTransform().GetTranslation().x);
			ImGui::End();
		}
	}
}
