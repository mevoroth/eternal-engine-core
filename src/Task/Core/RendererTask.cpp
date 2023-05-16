#include "Task/Core/RendererTask.hpp"
#include "Core/System.hpp"
#include "Parallel/AtomicS32.hpp"
#include "Parallel/Sleep.hpp"
#include "GraphicsEngine/Renderer.hpp"
#include "Log/Log.hpp"

namespace Eternal
{
	namespace Tasks
	{
		using namespace Eternal::LogSystem;
		const Log::LogCategory LogRenderer("Renderer");

		RendererTask::RendererTask(_In_ const TaskCreateInformation& InTaskCreateInformation, _In_ System& InSystem)
			: Task(InTaskCreateInformation)
			, _System(InSystem)
		{
		}

		void RendererTask::DoExecute()
		{
			ETERNAL_PROFILER(BASIC)();
			_WaitForData();
			//LogWrite(LogInfo, LogRenderer, "Rendering...");
			_System.Render();
			_AdvanceFrame();
		}

		void RendererTask::_WaitForData()
		{
			ETERNAL_PROFILER(BASIC)("WaitForSystemCanBeRenderered");
			while (_System.GetRenderFrame().SystemState->Load() != SystemCanBeRendered)
			{
				Sleep(1);
			}
		}

		void RendererTask::_AdvanceFrame()
		{
			_System.GetRenderFrame().SystemState->Store(SystemCanBeWritten);
			_System.AdvanceRender();
		}
	}
}
