#pragma once

namespace Eternal
{
	namespace Core
	{
		class World;
		class Level;
		class GameObject;

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

			inline void SetParent(_In_ WorldObject* InWorldObject)
			{
				_Parent = InWorldObject;
				SetWorld(InWorldObject ? InWorldObject->GetWorld() : nullptr);
			}

		protected:

			inline WorldObject* GetParentObject()
			{
				return _Parent;
			}

		private:

			World*			_World	= nullptr;
			WorldObject*	_Parent	= nullptr;
		};
	}
}
