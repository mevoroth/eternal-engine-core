#include "GameData/LevelGameData.hpp"
#include "Core/Level.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "GameData/TransformGameData.hpp"
#include "GameData/MeshGameData.hpp"
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
						//CameraItem.
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
								MeshObject->AddComponent(GameData.Load<TransformComponent>(TransformItem));
								Request->ComponentsToUpdate.push_back(MeshObject->AddComponent<MeshComponent>());
								OutLevel->AddGameObject(MeshObject);
							}
						);

						OutLevelPayload->Add(Request);
					}
				);
			}

			// Light
			{
				ETERNAL_PROFILER(DETAIL)("Lights");
			}

			return OutLevelPayload;
		}
	}
}
