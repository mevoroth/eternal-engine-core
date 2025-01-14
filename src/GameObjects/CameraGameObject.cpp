#include "GameObjects/CameraGameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Core/World.hpp"
#include "Core/CoreHelper.hpp"
#include "Input/InputDefines.hpp"
#include "Input/InputMapping.hpp"
#include "Math/Math.hpp"

namespace Eternal
{
	namespace GameObjects
	{
		using namespace Eternal::Core;
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
			(void)InDeltaSeconds;
			InputMapping& Mapping = GetInputMapping(GetWorld());

			TransformComponent* Component = GetComponent<TransformComponent>();

			Transform CameraTransform = Component->GetTransform();
			CameraTransform.SetTranslation(Vector3::Zero);

			Matrix4x4 ViewToWorld = CameraTransform.GetLocalToWorld();
			Vector4 Forward	= ViewToWorld * Vector4::ForwardPosition;
			Vector4 Right	= ViewToWorld * Vector4::RightPosition;

			Vector3 ForwardVector(Forward.x, Forward.y, Forward.z);
			Vector3 RightVector(Right.x, Right.y, Right.z);

			float LXWithDeadZone = Mapping.GetActionFloat("move_left_right");
			float LYWithDeadZone = Mapping.GetActionFloat("move_forward_backward");
			//float LYWithDeadZone = InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_LY);
			//float RXWithDeadZone = InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_RX);
			//float RYWithDeadZone = InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_RY);

			static float Multiplier = 100000.0f;

			Component->GetTransform().Translate(
				RightVector * LXWithDeadZone * Multiplier * InDeltaSeconds +
				ForwardVector * LYWithDeadZone * Multiplier * InDeltaSeconds /*+
				Vector3::Up * RYWithDeadZone * Multiplier * InDeltaSeconds*/
			);

			//Component->GetTransform().Rotate(
			//	Euler(0.0f, RXWithDeadZone * 0.1f, 0.0f /*RYWithDeadZone * 0.1f*/)
			//);
		}

		void CameraGameObject::UpdateDebug()
		{
			InputMapping& Mapping = GetInputMapping(GetWorld());

			TransformComponent* Component = GetComponent<TransformComponent>();

			//ImGui::Begin("Camera");
			//ImGui::Text("Input X: [%.3f] / Input Y: [%.3f]", InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_LX), InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_LY));
			//ImGui::Text("Input RX: [%.3f] / Input RY: [%.3f]", InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_RX), InputSystem.GetAxisWithDeadZone(InputAxis::AXIS_JOY0_RY));
			//ImGui::InputFloat3("Camera Position:", &Component->GetTransform().GetTranslation().x);
			//ImGui::End();
		}
	}
}
