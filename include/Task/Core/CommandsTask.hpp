#ifndef _COMMANDS_TASK_HPP_
#define _COMMANDS_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Core
	{
		class StateSharedData;
	}

	namespace Graphics
	{
		class Device;
	}

	namespace GraphicData
	{
		class GraphicResources;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;
		using namespace Eternal::Core;
		using namespace Eternal::GraphicData;

		class CommandsTaskData;
		class CommandsTask : public Task
		{
		public:
			CommandsTask(_In_ Device& DeviceObj, _In_ GraphicResources* Resources, _In_ StateSharedData* SharedData);

			virtual void DoSetup() override;
			virtual void DoReset() override;
			virtual void DoExecute() override;

		private:
			CommandsTaskData* _CommandsTaskData = nullptr;
		};
	}
}

#endif
