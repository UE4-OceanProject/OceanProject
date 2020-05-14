// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class OceanProjectEditorTarget : TargetRules
{
	public OceanProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		//Uncomment for 4.24
		//DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "OceanProject" } );
	}
}
