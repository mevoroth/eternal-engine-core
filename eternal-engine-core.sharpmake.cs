using Sharpmake;

[module: Sharpmake.Include(@"..\eternal-engine\eternal-engine-project.sharpmake.cs")]

namespace EternalEngine
{
	public class EternalEngineCoreProjectUtils
	{
		public static void ConfigureAll(Project.Configuration InConfiguration, ITarget InTarget, System.Type InTargetType)
		{
			// Include paths
			InConfiguration.IncludePaths.AddRange(new string[] {
				@"$(SolutionDir)eternal-engine-core\CorePrivate\include",
				@"$(SolutionDir)eternal-engine-components\include",
				@"$(SolutionDir)eternal-engine-graphics\include",
				@"$(SolutionDir)eternal-engine-graphics\GraphicsPrivate\include",
				@"$(SolutionDir)eternal-engine-extern\imgui",
				@"$(SolutionDir)eternal-engine-extern\rapidjson\include",
				@"$(SolutionDir)eternal-engine-extern\renderdoc",
				@"$(SolutionDir)eternal-engine-shaders",
				EternalEngineSettings.FBXSDKPath + @"\include",
			});

			if (InTargetType == typeof(Target))
			{
				InConfiguration.AddPublicDependency<EternalEngineExternProject>(InTarget);
				InConfiguration.AddPublicDependency<EternalEngineUtilsProject>(InTarget);
			}

			if (InTargetType == typeof(AndroidTarget))
			{
				InConfiguration.AddPublicDependency<EternalEngineExternAndroidProject>(InTarget);
				InConfiguration.AddPublicDependency<EternalEngineUtilsAndroidProject>(InTarget);
			}
		}
	}

	[Sharpmake.Generate]
	public class EternalEngineCoreProject : EternalEngineBaseProject
	{
		public EternalEngineCoreProject()
			: base(
				typeof(Target),
				"core",
				new EternalEngineProjectSettings(
					EternalEngineProjectSettingsFlags.EEPSF_IncludeSettingsHeader |
					EternalEngineProjectSettingsFlags.EEPSF_IncludeHLSLReflection
				)
			)
		{
		}

		public override void ConfigureAll(Configuration InConfiguration, ITarget InTarget)
		{
			base.ConfigureAll(InConfiguration, InTarget);
			EternalEngineCoreProjectUtils.ConfigureAll(InConfiguration, InTarget, Targets.TargetType);
		}
	}

	[Sharpmake.Generate]
	public class EternalEngineCoreAndroidProject : EternalEngineBaseAndroidProject
	{
		public EternalEngineCoreAndroidProject()
			: base(
				typeof(AndroidTarget),
				"core",
				new EternalEngineProjectSettings(
					EternalEngineProjectSettingsFlags.EEPSF_IncludeSettingsHeader |
					EternalEngineProjectSettingsFlags.EEPSF_IncludeHLSLReflection
				)
			)
		{
		}

		public override void ConfigureAll(Configuration InConfiguration, ITarget InTarget)
		{
			base.ConfigureAll(InConfiguration, InTarget);
			EternalEngineCoreProjectUtils.ConfigureAll(InConfiguration, InTarget, Targets.TargetType);
		}
	}
}
