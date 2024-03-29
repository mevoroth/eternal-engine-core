#include "Core/World.hpp"
#include "Core/Level.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/MeshComponent.hpp"

namespace Eternal
{
	namespace Core
	{
		World::World(_In_ Game& InGame)
			: _Game(InGame)
		{
			_Levels.reserve(LevelsInitialPool);
		}

		void World::AddLevel(_In_ Level* InLevel)
		{
			_Levels.push_back(InLevel);
			InLevel->SetWorld(this);
		}

		void World::Update(_In_ TimeSecondsT InDeltaSeconds)
		{
			for (uint32_t LevelIndex = 0; LevelIndex < _Levels.size(); ++LevelIndex)
				_Levels[LevelIndex]->Update(InDeltaSeconds);

			_UpdateComponents(InDeltaSeconds);
		}

		void World::_UpdateComponents(_In_ TimeSecondsT InDeltaSeconds)
		{
			ComponentPool<TransformComponent>::Update(InDeltaSeconds);
			ComponentPool<CameraComponent>::Update(InDeltaSeconds);
			ComponentPool<LightComponent>::Update(InDeltaSeconds);
			ComponentPool<MeshComponent>::Update(InDeltaSeconds);
		}

		void World::UpdateDebug()
		{
			for (uint32_t LevelIndex = 0; LevelIndex < _Levels.size(); ++LevelIndex)
				_Levels[LevelIndex]->UpdateDebug();
		}
	}
}
