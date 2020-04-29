// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class EnvironmentProjectEditorTarget : TargetRules
{
	public EnvironmentProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "EnvironmentProject" } );
	}
}
