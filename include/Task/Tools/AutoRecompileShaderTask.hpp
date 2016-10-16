#ifndef _AUTO_RECOMPILE_SHADER_TASK_HPP_
#define _AUTO_RECOMPILE_SHADER_TASK_HPP_

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Task
	{
		using namespace Eternal::Parallel;

		class AutoRecompileShaderTask : public Task
		{
		public:
			AutoRecompileShaderTask();
			~AutoRecompileShaderTask();

			virtual void Setup() override;
			virtual void Reset() override;
			virtual void Execute() override;

		private:
			void** _FilesWatchHandles = nullptr;
			void** _DirectoryHandles = nullptr;
			int _FilesWatchHandlesCount = 0;
		};
	}
}

#endif
