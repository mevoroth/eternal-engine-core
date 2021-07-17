#include "GameData/LevelGameData.hpp"
#include "Core/Level.hpp"
#include "Core/GameObject.hpp"
#include "Components/TransformComponent.hpp"
#include "GameData/TransformGameData.hpp"

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace Eternal::Core;

		void* LevelGameData::InternalLoad(_In_ const GameDataSource& InNode) const
		{
			ETERNAL_PROFILER(BASIC)();
			Level* OutLevel = new Level();

			// Camera
			{
				ETERNAL_PROFILER(DETAIL)("Cameras");
				const GameDataSource& Cameras = InNode.GetSubNode(GameDataSourceKey::CAMERAS);
				IterateGameDataCollection(
					Cameras,
					[OutLevel](_In_ const GameDataSource& CameraItem)
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
					[OutLevel](_In_ const GameDataSource& MeshItem)
					{
						const GameDataSource& Transforms = MeshItem.GetSubNode(GameDataSourceKey::TRANSFORM);

						IterateGameDataCollection(
							Transforms,
							[OutLevel](_In_ const GameDataSource& TransformItem)
							{
								TransformGameData GameData;
								GameObject* MeshObject = new GameObject();
								MeshObject->AddComponent(GameData.Load<TransformComponent>(TransformItem));
								OutLevel->AddGameObject(MeshObject);
							}
						);
					}
				);
			}

			// Light
			{
				ETERNAL_PROFILER(DETAIL)("Lights");
			}

			return OutLevel;
		}
	}
}
