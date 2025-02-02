#pragma once

namespace Eternal
{
	namespace InputSystem
	{
		class InputMapping;
	}
	namespace Core
	{
		using namespace Eternal::InputSystem;

		class AnimationSystem;
		class Game;
		class System;
		class World;

		Game& GetGame(_In_ World* InWorld);
		System& GetSystem(_In_ World* InWorld);
		AnimationSystem& GetAnimationSystem(_In_ World* InWorld);
		InputMapping& GetInputMapping(_In_ World* InWorld);
	}
}
