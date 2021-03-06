#include "Task/Core/RendererTask.hpp"
#include "Core/System.hpp"
#include "Parallel/AtomicS32.hpp"
#include "Parallel/Sleep.hpp"
#include "Log/Log.hpp"

namespace Eternal
{
	namespace Tasks
	{
		using namespace Eternal::LogSystem;
		const Log::LogCategory LogRenderer("Renderer");

		RendererTask::RendererTask(_In_ RendererTaskCreateInformation& InRendererTaskCreateInformation)
			: _RendererTaskCreateInformation(InRendererTaskCreateInformation)
		{
		}

		void RendererTask::DoExecute()
		{
			_WaitForData();
			LogWrite(LogInfo, LogRenderer, "Rendering...");
			SystemFrame& CurrentRenderFrame = GetSystem().GetRenderFrame();
			GraphicsContext& GfxContext = GetSystem().GetGraphicsContext();
			GfxContext.BeginFrame();

			GetSystem().GetImgui().SetContext(CurrentRenderFrame.ImguiFrameContext);
			GetSystem().GetImgui().Render(GfxContext);

			GfxContext.EndFrame();
			_AdvanceFrame();
		}

		void RendererTask::_WaitForData()
		{
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
