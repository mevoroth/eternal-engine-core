#ifndef _STATE_SHARED_DATA_HPP_
#define _STATE_SHARED_DATA_HPP_

#include "GraphicsSettings.hpp"

#include <cstdint>
#include <vector>
#include "Parallel/AtomicS32.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Context;
		class CommandList;
	}

	namespace Parallel
	{
		class AtomicS32;
	}

	namespace Core
	{
		using namespace std;
		using namespace Eternal::Graphics;
		using namespace Eternal::Parallel;

		class GraphicGameObject;
		class CameraGameObject;
		class LightGameObject;
		class EngineCommand;

		class StateSharedData
		{
		public:
			StateSharedData();
			virtual ~StateSharedData();

			vector<GraphicGameObject*>			GraphicGameObjects;
			vector<GraphicGameObject*>			LoadingQueues[FRAME_LAG];
			vector< vector<EngineCommand*> >	EngineCommands[FRAME_LAG];
			Context*							GfxContexts[FRAME_LAG];
			CommandList*						RecordedCommandLists[FRAME_LAG][128];
			AtomicS32*							CommandListsCount[FRAME_LAG];
			uint32_t							CurrentFrame		= 0u;
			CameraGameObject*					Camera				= nullptr;
			LightGameObject*					Lights				= nullptr;

			/**
			 * Previous frame filled queue
			 */
			vector<GraphicGameObject*>& GetPendingQueue();
			/**
			 * Queue to be filled
			 */
			vector<GraphicGameObject*>& GetActiveQueue();

			void RecordCommandList(_In_ CommandList* NewCommandList);
			void Reset();
		};
	}
}

#endif
