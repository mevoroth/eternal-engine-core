#ifndef _SOLIDGBUFFER_TASK_
#define _SOLIDGBUFFER_TASK_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Graphics
	{
		class Context;
	}

	namespace Task
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Graphics;

		class SolidGBufferData;

		class SolidGBufferTask : public Task
		{
		public:
			SolidGBufferTask(_In_ Context& ContextObj);

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			SolidGBufferData* _SolidGBufferData = nullptr;
		};
	}
}

#endif
