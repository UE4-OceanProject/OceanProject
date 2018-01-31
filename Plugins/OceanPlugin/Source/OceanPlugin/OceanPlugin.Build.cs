/*=================================================
* FileName: OceanPlugin.Build.cs
* Project name: OceanProject
* Unreal Engine version: 4.18.3
*
* Last Edited on: 2018/1/30
* Last Edited by: saschaelble
*
* =================================================*/

using UnrealBuildTool;
using System.IO;

public class OceanPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(ModuleDirectory); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public OceanPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "OceanPlugin/Private" });
       // PublicIncludePaths.AddRange(new string[] { "OceanPlugin/Public" });
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "PhysX", "APEX", "ApexDestruction", "Landscape", "RHI", "RenderCore" });
    }
}