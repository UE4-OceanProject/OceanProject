
namespace UnrealBuildTool.Rules 
{
    public class OceanPlugin : ModuleRules {

        public OceanPlugin(TargetInfo Target) {

            PublicIncludePaths.AddRange(new string[] { "OceanPlugin/Classes/Fish" });

            PrivateIncludePaths.AddRange(new string[] { "OceanPlugin/Private", "OceanPlugin/Private/Fish" });

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

            PrivateDependencyModuleNames.AddRange(new string[] { });

            DynamicallyLoadedModuleNames.AddRange(new string[] { });
        }
    }
}