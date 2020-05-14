// 2017 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class OceanProjectTarget : TargetRules
{
	public OceanProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		//Uncomment for 4.24
		//DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "OceanProject" } );
	}
}
