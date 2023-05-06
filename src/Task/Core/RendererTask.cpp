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

		void RendererTask::DoExecute()
		{
			ETERNAL_PROFILER(BASIC)();
			_WaitForData();
			//LogWrite(LogInfo, LogRenderer, "Rendering...");
			GetSystem().Render();
			_AdvanceFrame();
		}

		void RendererTask::_WaitForData()
		{
			ETERNAL_PROFILER(BASIC)("WaitForSystemCanBeRenderered");
			while (GetSystem().GetRenderFrame().SystemState->Load() != SystemCanBeRendered)
			{
				Sleep(1);
			}
		}

		void RendererTask::_AdvanceFrame()
		{
			GetSystem().GetRenderFrame().SystemState->Store(SystemCanBeWritten);
			GetSystem().AdvanceRender();
		}
	}
}
