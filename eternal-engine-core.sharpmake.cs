using Sharpmake;

[module: Sharpmake.Include(@"..\eternal-engine\eternal-engine-project.sharpmake.cs")]

namespace EternalEngine
{
	[Sharpmake.Generate]
	public class EternalEngineCoreProject : EternalEngineBaseProject
	{
		public EternalEngineCoreProject()
			: base(
				"core",
				new EternalEngineProjectSettings(
					EternalEngineProjectSettingsFlags.EEPSF_IncludeSettingsHeader |
					EternalEngineProjectSettingsFlags.EEPSF_IncludeHLSLReflection
				)
			)
		{
		}

		public override void ConfigureAll(Configuration InConfiguration, Target InTarget)
		{
			base.ConfigureAll(InConfiguration, InTarget);

			// Include paths
			InConfiguration.IncludePaths.AddRange(new string[] {
				@"$(SolutionDir)eternal-engine-components\include",
				@"$(SolutionDir)eternal-engine-graphics\include",
				@"$(SolutionDir)eternal-engine-extern\imgui",
				@"$(SolutionDir)eternal-engine-extern\rapidjson\include",
				@"$(SolutionDir)eternal-engine-extern\renderdoc",
				@"$(SolutionDir)eternal-engine-shaders",
				EternalEngineSettings.VulkanPath + @"\Include",
				EternalEngineSettings.FBXSDKPath + @"\include",
			});

			if (InConfiguration.Platform != Platform.win64 && InConfiguration.Platform != Platform.win32)
			{
				InConfiguration.IncludePaths.AddRange(new string[] {
					@"$(SolutionDir)eternal-engine-core\CorePrivate\include",
					@"$(SolutionDir)eternal-engine-graphics\GraphicsPrivate\include",
				});
			}

			InConfiguration.AddPublicDependency<EternalEngineExternProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineUtilsProject>(InTarget);
		}
	}
}
