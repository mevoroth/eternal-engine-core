#pragma once

#include "Parallel/Task.hpp"

namespace Eternal
{
	namespace Core
	{
		class System;
	}
	namespace Tasks
	{
		using namespace Eternal::Parallel;
		using namespace Eternal::Core;

		class RendererTask final : public Task
		{
		public:
			using Task::Task;

			virtual void DoExecute() override final;

		private:
			void _WaitForData();
			void _AdvanceFrame();
		};
	}
}
