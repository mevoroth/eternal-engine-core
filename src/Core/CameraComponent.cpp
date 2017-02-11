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
	_Camera->UpdateView(_TransformComponent->GetTransform());
}

void CameraComponent::End()
{

}

void CameraComponent::AttachTo(_In_ TransformComponent* TransformComponentObj)
{
	_TransformComponent = TransformComponentObj;
}
