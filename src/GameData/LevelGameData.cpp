#include "GameData/LevelGameData.hpp"
#include "Core/Level.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/LightComponent.hpp"
#include "GameData/TransformGameData.hpp"
#include "GameData/MeshGameData.hpp"
#include "GameData/CameraGameData.hpp"
#include "GameData/LightGameData.hpp"
#include "Resources/LevelPayload.hpp"
#include "Resources/Payload.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;
		using namespace Eternal::Resources;

		void* LevelGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			ETERNAL_PROFILER(BASIC)();
			LevelPayload* OutLevelPayload = new LevelPayload();
			OutLevelPayload->LoadedLevel = new Level();

			// Camera
			{
				ETERNAL_PROFILER(DETAIL)("Cameras");
				const GameDataSource& Cameras = InNode.GetSubNode(GameDataSourceKey::CAMERAS);
				IterateGameDataCollection(
					Cameras,
					[OutLevel = OutLevelPayload->LoadedLevel](_In_ const GameDataSource& CameraItem)
					{
						CameraGameData GameData;
						OutLevel->AddGameObject(GameData.Load<GameObject>(CameraItem));
					}
				);
			}

			// Mesh
			{
				ETERNAL_PROFILER(DETAIL)("Meshes");
				const GameDataSource& Meshes = InNode.GetSubNode(GameDataSourceKey::MESHES);
				IterateGameDataCollection(
					Meshes,
					[OutLevel = OutLevelPayload->LoadedLevel, OutLevelPayload](_In_ const GameDataSource& MeshItem)
					{
						const GameDataSource& Transforms = MeshItem.GetSubNode(GameDataSourceKey::TRANSFORM);

						MeshGameData GameData;
						std::string* MeshPath = GameData.Load<std::string>(MeshItem);
						MeshRequest* Request = new MeshRequest(*MeshPath);
						delete MeshPath;

						uint32_t GameObjectCount = 0;
						Transforms.Get(GameObjectCount);

						Request->ComponentsToUpdate.reserve(GameObjectCount);

						IterateGameDataCollection(
							Transforms,
							[OutLevel, &Request](_In_ const GameDataSource& TransformItem)
							{
								TransformGameData GameData;
								GameObject* MeshObject = new GameObject();
								MeshObject->AddComponent<TransformComponent>(GameData.Load<TransformComponent>(TransformItem));
								Request->ComponentsToUpdate.push_back(MeshObject->AddComponent<MeshComponent>());
								OutLevel->AddGameObject(MeshObject);
							}
						);

						OutLevelPayload->AddRequest(Request);
					}
				);
			}

			// Light
			{
				ETERNAL_PROFILER(DETAIL)("Lights");
				const GameDataSource& Lights = InNode.GetSubNode(GameDataSourceKey::LIGHTS);
				IterateGameDataCollection(
					Lights,
					[OutLevel = OutLevelPayload->LoadedLevel](_In_ const GameDataSource& LightItem)
					{
						const GameDataSource& Transforms = LightItem.GetSubNode(GameDataSourceKey::TRANSFORM);

						LightGameData LightData;

						IterateGameDataCollection(
							Transforms,
							[OutLevel, &LightData, &LightItem](_In_ const GameDataSource& TransformItem)
							{
								TransformGameData TransformData;
								GameObject* LightObject = new GameObject();
								LightComponent* NewLightComponent = LightData.Load<LightComponent>(LightItem);
								LightObject->AddComponent<LightComponent>(NewLightComponent);
								LightObject->AddComponent<TransformComponent>(
									TransformData.Load<TransformComponent>(TransformItem),
									[NewLightComponent](_Inout_ TransformComponent* InOutComponent)
									{
										InOutComponent->SetHasBehavior();
										InOutComponent->SetUpdatesEveryFrame();
										InOutComponent->OnTransformChanged().Receivers.push_back(&NewLightComponent->OnTransformChangedReceiver());
									}
								);
								OutLevel->AddGameObject(LightObject);
							}
						);
					}
				);
			}

			return OutLevelPayload;
		}
	}
}
