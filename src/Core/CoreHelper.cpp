#include "Core/CoreHelper.hpp"
#include "Core/Game.hpp"
#include "Core/System.hpp"
#include "Core/World.hpp"

namespace Eternal
{
	namespace Core
	{
		Game& GetGame(_In_ World* InWorld)
		{
			return InWorld->GetGame();
		}

		System& GetSystem(_In_ World* InWorld)
		{
			return GetGame(InWorld).GetSystem();
		}

		AnimationSystem& GetAnimationSystem(_In_ World* InWorld)
		{
			return GetSystem(InWorld).GetAnimationSystem();
		}

		InputMapping& GetInputMapping(_In_ World* InWorld)
		{
			return GetSystem(InWorld).GetInputMapping();
		}
	}
}
