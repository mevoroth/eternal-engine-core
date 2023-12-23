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

			TransformComponent* Component = GetComponent<TransformComponent>();

			Transform CameraTransform = Component->GetTransform();
			CameraTransform.SetTranslation(Vector3::Zero);

			Matrix4x4 ViewToWorld = CameraTransform.GetLocalToWorld();
			Vector4 Forward	= ViewToWorld * Vector4::ForwardPosition;
			Vector4 Right	= ViewToWorld * Vector4::RightPosition;

			Vector3 ForwardVector(Forward.x, Forward.y, Forward.z);
			Vector3 RightVector(Right.x, Right.y, Right.z);

			float LXWithDeadZone = InputSystem.GetAxisWithDeadZone(Input::JOY0_LX);
			float LYWithDeadZone = InputSystem.GetAxisWithDeadZone(Input::JOY0_LY);
			float RXWithDeadZone = InputSystem.GetAxisWithDeadZone(Input::JOY0_RX);
			float RYWithDeadZone = InputSystem.GetAxisWithDeadZone(Input::JOY0_RY);

			Component->GetTransform().Translate(
				RightVector * LXWithDeadZone * 10.0f +
				ForwardVector * LYWithDeadZone * 10.0f +
				Vector3::Up * RYWithDeadZone * 10.0f
			);

			Component->GetTransform().Rotate(
				Euler(0.0f, RXWithDeadZone * 0.1f, 0.0f /*RYWithDeadZone * 0.1f*/)
			);
		}

		void CameraGameObject::UpdateDebug()
		{
			Input& InputSystem = GetWorld()->GetGame().GetSystem().GetInput();

			TransformComponent* Component = GetComponent<TransformComponent>();

			ImGui::Begin("Camera");
			ImGui::Text("Input X: [%.3f] / Input Y: [%.3f]", InputSystem.GetAxisWithDeadZone(Input::JOY0_LX), InputSystem.GetAxisWithDeadZone(Input::JOY0_LY));
			ImGui::Text("Input RX: [%.3f] / Input RY: [%.3f]", InputSystem.GetAxisWithDeadZone(Input::JOY0_RX), InputSystem.GetAxisWithDeadZone(Input::JOY0_RY));
			ImGui::InputFloat3("Camera Position:", &Component->GetTransform().GetTranslation().x);
			ImGui::End();
		}
	}
}
