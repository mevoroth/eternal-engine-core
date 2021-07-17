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

		struct RendererTaskCreateInformation
		{
			RendererTaskCreateInformation(_In_ System& InSystem)
				: SystemContext(InSystem)
			{
			}

			System& SystemContext;
		};

		class RendererTask final : public Task
		{
		public:
			RendererTask(_In_ RendererTaskCreateInformation& InRendererTaskCreateInformation);

			virtual void DoExecute() override final;

		private:
			System& GetSystem() { return _RendererTaskCreateInformation.SystemContext; }
			void _WaitForData();
			void _AdvanceFrame();

			RendererTaskCreateInformation _RendererTaskCreateInformation;
		};
	}
}
