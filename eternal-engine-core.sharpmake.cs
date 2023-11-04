using Sharpmake;

[module: Sharpmake.Include(@"..\eternal-engine\eternal-engine.sharpmake.cs")]

namespace EternalEngine
{
	[Sharpmake.Generate]
	public class EternalEngineCoreProject : EternalEngineProject
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
			InConfiguration.IncludePaths.Add(new string[] {
				@"$(SolutionDir)eternal-engine-components\include",
				@"$(SolutionDir)eternal-engine-graphics\include",
				@"$(SolutionDir)eternal-engine-extern\imgui",
				@"$(SolutionDir)eternal-engine-extern\rapidjson\include",
				@"$(SolutionDir)eternal-engine-extern\renderdoc",
				@"$(SolutionDir)eternal-engine-shaders",
				EternalEngineSettings.VulkanPath + @"\Include",
				EternalEngineSettings.FBXSDKPath + @"\include",
			});

			InConfiguration.AddPublicDependency<EternalEngineExternProject>(InTarget);
			InConfiguration.AddPublicDependency<EternalEngineUtilsProject>(InTarget);
		}
	}
}
