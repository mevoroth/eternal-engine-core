#pragma once

namespace Eternal
{
	namespace Core
	{
		class World;

		class WorldObject
		{
		public:
			WorldObject(_In_ World* InWorld = nullptr)
				: _World(InWorld)
			{
			}
			virtual ~WorldObject() {}

			virtual void SetWorld(_In_ World* InWorld)
			{
				_World = InWorld;
			}

			inline World* GetWorld()
			{
				return _World;
			}

		private:
			World* _World = nullptr;
		};
	}
}
