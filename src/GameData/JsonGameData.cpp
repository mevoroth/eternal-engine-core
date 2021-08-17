#include "GameData/JsonGameData.hpp"
#include "Transform/Transform.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "GameData/CameraGameData.hpp"
#include "GameData/LightGameData.hpp"
#include "Camera/OrthographicCamera.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Light/DirectionalLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/SpotLight.hpp"
#include "Types/Types.hpp"
#include "Math/Math.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Types;
		using namespace Eternal::Core;
		using namespace Eternal::Math;

		//////////////////////////////////////////////////////////////////////////
		// Node

		JsonNode::JsonNode(_In_ const JsonNodeType& InNode)
			: _Node(InNode)
		{
		}

		//////////////////////////////////////////////////////////////////////////
		// Array

		template<typename JsonNodeType>
		ETERNAL_THREAD_LOCAL GameDataSource* JsonArrayNode<JsonNodeType>::TemporarySubNode = nullptr;

		template<typename JsonNodeType>
		JsonArrayNode<JsonNodeType>::~JsonArrayNode()
		{
			if (TemporarySubNode)
			{
				delete TemporarySubNode;
				TemporarySubNode = nullptr;
			}
		}

		template<typename JsonNodeType>
		const GameDataSource& JsonArrayNode<JsonNodeType>::GetSubNode(_In_ const GameDataSourceKey& InKey) const
		{
			if (TemporarySubNode)
			{
				delete TemporarySubNode;
				TemporarySubNode = nullptr;
			}
			TemporarySubNode = new JsonNodeType(_Node.GetArray()[static_cast<uint32_t>(InKey)]);
			return *TemporarySubNode;
		}

		template<typename JsonNodeType>
		void JsonArrayNode<JsonNodeType>::InternalGet(_Out_ void* OutValue) const
		{
			*static_cast<uint32_t*>(OutValue) = static_cast<uint32_t>(_Node.GetArray().Size());
		}

		//////////////////////////////////////////////////////////////////////////
		// Transform collection
		
		JsonTransformCollection::JsonTransformCollection(_In_ const JsonNodeType& InNode)
		{
			_TransformNodes = new JsonArrayNode<JsonTransformNode>(InNode["m_Transforms"]);
		}
		
		JsonTransformCollection::~JsonTransformCollection()
		{
			delete _TransformNodes;
			_TransformNodes = nullptr;
		}

		const GameDataSource& JsonTransformCollection::Get() const
		{
			ETERNAL_ASSERT(_TransformNodes);
			return *_TransformNodes;
		}

		//////////////////////////////////////////////////////////////////////////
		// Camera

		JsonCameraNode::JsonCameraNode(_In_ const JsonNodeType& InNode)
			: JsonNode(InNode)
			, _TransformCollection(InNode)
		{
		}

		const GameDataSource& JsonCameraNode::GetSubNode(_In_ const GameDataSourceKey& InKey) const
		{
			ETERNAL_ASSERT(InKey == GameDataSourceKey::TRANSFORM);
			return _TransformCollection.Get();
		}

		void JsonCameraNode::InternalGet(_Out_ void* OutValue) const
		{
			GameObject* CameraGameObject = static_cast<GameObject*>(OutValue);
			CameraComponent* Component = CameraGameObject->AddComponent<CameraComponent>();
			TransformComponent* CameraTransformComponent = CameraGameObject->AddComponent<TransformComponent>(
				[Component](_Inout_ TransformComponent* InOutComponent)
				{
					InOutComponent->SetHasBehavior();
					InOutComponent->SetUpdatesEveryFrame();
					InOutComponent->OnTransformChanged().Receivers.push_back(&Component->OnTransformChangedReceiver());
				}
			);

			const GameDataSource& Transforms = GetSubNode(GameDataSourceKey::TRANSFORM);
			Transforms.GetSubNode(0).Get(CameraTransformComponent->GetTransform());

			CameraProjection ProjectionType = static_cast<CameraProjection>(_Node["m_Projection"].GetInt());
			Camera* CameraData = nullptr;

			switch (ProjectionType)
			{
			case CameraProjection::CAMERA_PROJECTION_ORTHOGRAPHIC:
			{
				CameraData = new OrthographicCamera(
					_Node["m_Near"].GetFloat(),
					_Node["m_Far"].GetFloat(),
					_Node["m_Width"].GetFloat(),
					_Node["m_Height"].GetFloat()
				);
			} break;
			case CameraProjection::CAMERA_PROJECTION_PERSPECTIVE:
			{
				CameraData = new PerspectiveCamera(
					_Node["m_Near"].GetFloat(),
					_Node["m_Far"].GetFloat(),
					_Node["m_FOV"].GetFloat() * DegreesToRadians,
					_Node["m_ScreenRatio"].GetFloat()
				);
			} break;
			}

			CameraData->UpdateWorldToView(CameraTransformComponent->GetTransform());
			Component->SetCamera(CameraData);
		}

		//////////////////////////////////////////////////////////////////////////
		// Mesh

		JsonMeshNode::JsonMeshNode(_In_ const JsonNodeType& InNode)
			: JsonNode(InNode)
			, _TransformCollection(InNode)
		{
		}

		const GameDataSource& JsonMeshNode::GetSubNode(_In_ const GameDataSourceKey& InKey) const
		{
			ETERNAL_ASSERT(InKey == GameDataSourceKey::TRANSFORM);
			return _TransformCollection.Get();
		}

		void JsonMeshNode::InternalGet(_Out_ void* OutValue) const
		{
			std::string& OutPath = *static_cast<std::string*>(OutValue);
			OutPath = _Node["m_Path"].GetString();
		}

		//////////////////////////////////////////////////////////////////////////
		// Light
		
		JsonLightNode::JsonLightNode(_In_ const JsonNodeType& InNode)
			: JsonNode(InNode)
			, _TransformCollection(InNode)
		{
		}

		const GameDataSource& JsonLightNode::GetSubNode(_In_ const GameDataSourceKey& InKey) const
		{
			ETERNAL_ASSERT(InKey == GameDataSourceKey::TRANSFORM);
			return _TransformCollection.Get();
		}

		void JsonLightNode::InternalGet(_Out_ void* OutValue) const
		{
			LightComponent* OutComponent = static_cast<LightComponent*>(OutValue);
			Light* OutLight = nullptr;

			LightType Type = static_cast<LightType>(_Node["m_LightType"].GetInt());

			switch (Type)
			{
			case LightType::LIGHT_TYPE_DIRECTIONAL:
			{
				OutLight = new DirectionalLight();
			} break;
			case LightType::LIGHT_TYPE_POINT:
			{
				OutLight = new PointLight();
			} break;
			case LightType::LIGHT_TYPE_SPOTLIGHT:
			//{

			//} break;
			case LightType::LIGHT_TYPE_AREA:
			default:
			{
				ETERNAL_BREAK();
			} break;
			}

			Vector3 Color(
				_Node["m_LightColor"]["r"].GetFloat(),
				_Node["m_LightColor"]["g"].GetFloat(),
				_Node["m_LightColor"]["b"].GetFloat()
			);
			OutLight->SetColor(Color);
			OutLight->SetIntensity(_Node["m_LightIntensity"].GetFloat());

			OutComponent->SetLight(OutLight);
		}

		//////////////////////////////////////////////////////////////////////////
		// Transform

		void JsonTransformNode::InternalGet(_Out_ void* OutValue) const
		{
			Components::Transform& OutTransform = *(Components::Transform*)OutValue;
			
			// Position
			const JsonNodeType& PositionNode = _Node["m_Position"];
			Vector3 Position(
				PositionNode["x"].GetFloat(),
				PositionNode["y"].GetFloat(),
				PositionNode["z"].GetFloat()
			);
			OutTransform.SetTranslation(Position);

			// Rotation
			const JsonNodeType& RotationNode = _Node["m_Rotation"];
			Quaternion Rotation(
				RotationNode["x"].GetFloat(),
				RotationNode["y"].GetFloat(),
				RotationNode["z"].GetFloat(),
				RotationNode["w"].GetFloat()
			);
			OutTransform.SetRotation(Rotation);

			// Scale
			const JsonNodeType& ScaleNode = _Node["m_Scale"];
			Vector3 Scale(
				ScaleNode["x"].GetFloat(),
				ScaleNode["y"].GetFloat(),
				ScaleNode["z"].GetFloat()
			);
			OutTransform.SetScaling(Scale);
		}

		//////////////////////////////////////////////////////////////////////////
		// Scene

		JsonLevel::JsonLevel(_In_ const string& InFileContent)
		{
			_Document.Parse(InFileContent.c_str());
			_CamerasNode = new JsonArrayNode<JsonCameraNode>(_Document["Cameras"]);
			_MeshesNode = new JsonArrayNode<JsonMeshNode>(_Document["Meshes"]);
			_LightsNode = new JsonArrayNode<JsonLightNode>(_Document["Lights"]);
		}

		JsonLevel::~JsonLevel()
		{
			delete _CamerasNode;
			_CamerasNode = nullptr;

			delete _MeshesNode;
			_MeshesNode = nullptr;

			delete _LightsNode;
			_LightsNode = nullptr;
		}

		const GameDataSource& JsonLevel::GetSubNode(_In_ const GameDataSourceKey& InKey) const
		{
			switch (InKey)
			{
			case GameDataSourceKey::CAMERAS:
				return *_CamerasNode;
			case GameDataSourceKey::MESHES:
				return *_MeshesNode;
			case GameDataSourceKey::LIGHTS:
				return *_LightsNode;
			}

			ETERNAL_BREAK();
			return *static_cast<GameDataSource*>(nullptr);
		}
	}
}
