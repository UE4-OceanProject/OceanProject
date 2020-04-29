// 2017 - Community based open project

using UnrealBuildTool;

public class EnvironmentProject : ModuleRules
{
	public EnvironmentProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	}
}
