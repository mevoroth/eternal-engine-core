#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include "Time/Time.hpp"

namespace Eternal
{
	namespace Core
	{
		using namespace std;
		using namespace Eternal::Time;

		class GameObjectInstance;

		class GameObject
		{
		public:
			virtual void Begin() = 0;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) = 0;
			virtual void End() = 0;
		};

		template<class GameObjectInstanceType>
		class InstanciableGameObject : public GameObject
		{
		public:
			GameObjectInstanceType* GetInstance(_In_ int Index)
			{
				ETERNAL_ASSERT(Index < _Instances.size());
				return (GameObjectInstanceType*)_Instances[Index];
			}

			void AddInstance(_In_ GameObjectInstanceType* Instance)
			{
				_Instances.push_back(Instance);
			}

			void RemoveInstance(_In_ GameObjectInstanceType* Instance)
			{
				ETERNAL_BREAK(); // Not implemented
			}

			size_t GetInstanceCount() const
			{
				return _Instances.size();
			}

		private:
			vector<GameObjectInstance*> _Instances;
		};
		
		class GameObjectInstance
		{
		public:
			virtual void Begin() = 0;
			virtual void Update(_In_ const TimeSecondsT& ElapsedSeconds) = 0;
			virtual void End() = 0;
		};
	}
}

#endif
