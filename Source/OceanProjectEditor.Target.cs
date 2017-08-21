// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class OceanProjectEditorTarget : TargetRules
{
	public OceanProjectEditorTarget(TargetInfo Target) : base (Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("OceanProject") ;
	}
}
