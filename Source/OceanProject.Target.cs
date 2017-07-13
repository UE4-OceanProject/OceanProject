// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class OceanProjectTarget : TargetRules
{
	public OceanProjectTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "OceanProject" });
    }
}
