#include "Task/Graphics/SolidGBufferTask.hpp"

#include "Graphics/Context.hpp"

namespace Eternal
{
	namespace Task
	{
		class SolidGBufferData
		{
		public:
			SolidGBufferData(_In_ Context& ContextObj)
				: _Context(ContextObj)
			{
			}

		private:
			Context& _Context;
		};
	}
}

using namespace Eternal::Task;

SolidGBufferTask::SolidGBufferTask(_In_ Context& ContextObj)
{
	_SolidGBufferData = new SolidGBufferData(ContextObj);
}

void SolidGBufferTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void SolidGBufferTask::Execute()
{
	ETERNAL_ASSERT(GetState() == Task::SETUP);
	SetState(EXECUTING);

	

	SetState(DONE);
}

void SolidGBufferTask::Reset()
{
	ETERNAL_ASSERT(GetState() == Task::DONE);
	SetState(IDLE);
}
