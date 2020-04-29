// 2017 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class EnvironmentProjectTarget : TargetRules
{
	public EnvironmentProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.AddRange( new string[] { "EnvironmentProject" } );
	}
}
