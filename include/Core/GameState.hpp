#ifndef _GAMESTATE_HPP_
#define _GAMESTATE_HPP_

namespace Eternal
{
	namespace Core
	{
		class StateSharedData;

		class GameState
		{
		public:
			virtual void Begin() = 0;
			virtual void Update() = 0;
			virtual GameState* NextState() = 0;
			virtual void End() = 0;
			void SetSharedData(_In_ StateSharedData* SharedData);
			StateSharedData* GetSharedData();

		private:
			StateSharedData* _StateSharedData = nullptr;
		};
	}
}

#endif
