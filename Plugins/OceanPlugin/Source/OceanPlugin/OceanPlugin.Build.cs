using UnrealBuildTool;
using System.IO;

public class OceanPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public OceanPlugin(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "OceanPlugin/Private" });
       // PublicIncludePaths.AddRange(new string[] { "OceanPlugin/Public" });

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "PhysX", "APEX" });
    }
}