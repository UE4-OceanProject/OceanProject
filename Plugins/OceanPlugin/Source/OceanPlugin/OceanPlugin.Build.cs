/*=================================================
* FileName: OceanPlugin.Build.cs
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/20
*
* Last Edited on: 2018/03/15
* Last Edited by: Felipe "Zoc" Silveira
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
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
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });
        // PublicIncludePaths.AddRange(new string[] { "OceanPlugin/Public" });

        // Make sure UBT reminds us of how to keep the project IWYU compliant
        bEnforceIWYU = true;

        //Enable IWYU but keep our PrivatePCH in use
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //Our PrivatePCH that we want to globally #include
        PrivatePCHHeaderFile = "Classes/OceanPluginPrivatePCH.h";
        
        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "PhysX", "APEX", "ApexDestruction", "Landscape", "RHI", "RenderCore" });
    }
}