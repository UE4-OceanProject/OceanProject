
namespace UnrealBuildTool.Rules 
{
    public class OceanPlugin : ModuleRules {

        public OceanPlugin(TargetInfo Target) {

            PublicIncludePaths.AddRange(new string[] { });

            PrivateIncludePaths.AddRange(new string[] { "OceanPlugin/Private" });

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

            PrivateDependencyModuleNames.AddRange(new string[] { });

            DynamicallyLoadedModuleNames.AddRange(new string[] { });
        }
    }
}