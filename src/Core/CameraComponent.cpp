#include "Core/CameraComponent.hpp"

#include "Camera/Camera.hpp"
#include "Resources/Pool.hpp"
#include "Core/TransformComponent.hpp"
#include "Transform/Transform.hpp"

using namespace Eternal::Core;
using namespace Eternal::Resources;

Pool<CameraComponent, 4>* g_CameraComponentPool = nullptr;

Camera* CameraComponent::GetCamera()
{
	return _Camera;
}

void CameraComponent::SetCamera(_In_ Camera* CameraObj)
{
	_Camera = CameraObj;
}

CameraComponent::CameraComponent()
{
}

CameraComponent::~CameraComponent()
{

}

void CameraComponent::Initialize()
{
	g_CameraComponentPool = new Pool<CameraComponent, 4>();
}

void CameraComponent::Release()
{
	delete g_CameraComponentPool;
	g_CameraComponentPool = nullptr;
}

void CameraComponent::Begin()
{

}

void CameraComponent::Update(_In_ const TimeSecondsT& ElapsedSeconds)
{
	const Vector3& Position = _TransformComponent->GetTransform().GetTranslation();

	Transform TempTransform = _TransformComponent->GetTransform();
	TempTransform.SetTranslation(Vector3(0.f, 0.f, 0.f));
	Vector3 TempForward3 = Vector3(0.f, 0.f, 0.f);
	Vector3 TempUp3 = Vector3(0.f, 0.f, 0.f);
	
	// Compute rotation
	Vector4 Forward = TempTransform.GetModelMatrix() * Vector4(0.f, 0.f, 1.f, 1.f);
	Vector4 Up = TempTransform.GetModelMatrix() * Vector4(0.f, 1.f, 0.f, 1.f);

	TempForward3.x = Forward.x;
	TempForward3.y = Forward.y;
	TempForward3.z = Forward.z;

	TempUp3.x = Up.x;
	TempUp3.y = Up.y;
	TempUp3.z = Up.z;

	_Camera->UpdateViewMatrix(
		Position,
		TempForward3,
		TempUp3
	);

	//static Vector3 debugPos(0, 0, 0);
	//static Vector3 debugFwd(0, 1, 0);
	//static Vector3 debugUp(0, 0, 1);

	//_Camera->UpdateViewMatrix(
	//	debugPos,
	//	debugFwd,
	//	debugUp
	//);
}

void CameraComponent::End()
{

}

void CameraComponent::AttachTo(_In_ TransformComponent* TransformComponentObj)
{
	_TransformComponent = TransformComponentObj;
}
