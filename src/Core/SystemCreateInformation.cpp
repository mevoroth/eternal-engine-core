#include "Core/SystemCreateInformation.hpp"
#include "Graphics/GraphicsContext.hpp"

namespace Eternal
{
	namespace Core
	{
		SystemCreateInformation::SystemCreateInformation(_In_ GraphicsContextCreateInformation* InContextInformation)
			: ContextInformation(InContextInformation)
		{
		}

		SystemCreateInformation::SystemCreateInformation(_Inout_ SystemCreateInformation&& InOutSystemCreateInformation)
			: ExecutableInput(std::move(InOutSystemCreateInformation.ExecutableInput))
			, ContextInformation(std::move(InOutSystemCreateInformation.ContextInformation))
			, GameContext(std::move(InOutSystemCreateInformation.GameContext))
			, CreateCustomRendererFunction(std::move(InOutSystemCreateInformation.CreateCustomRendererFunction))
			, ShaderIncludePath(std::move(InOutSystemCreateInformation.ShaderIncludePath))
			, ShaderPDBPath(std::move(InOutSystemCreateInformation.ShaderPDBPath))
			, FBXPath(std::move(InOutSystemCreateInformation.FBXPath))
			, FBXCachePath(std::move(InOutSystemCreateInformation.FBXCachePath))
			, TexturePath(std::move(InOutSystemCreateInformation.TexturePath))
			, LevelPath(std::move(InOutSystemCreateInformation.LevelPath))
			, PipelineCachePath(std::move(InOutSystemCreateInformation.PipelineCachePath))
			, MaterialPath(std::move(InOutSystemCreateInformation.MaterialPath))
			, AnimationPath(std::move(InOutSystemCreateInformation.AnimationPath))
			, SFXSoundPath(std::move(InOutSystemCreateInformation.SFXSoundPath))
			, BGMSoundPath(std::move(InOutSystemCreateInformation.BGMSoundPath))
		{
			InOutSystemCreateInformation.ContextInformation = nullptr;
		}

		SystemCreateInformation::~SystemCreateInformation()
		{
			if (ContextInformation)
				Graphics::DestroyGraphicsContextCreateInformation(ContextInformation);
		}
	}
}
