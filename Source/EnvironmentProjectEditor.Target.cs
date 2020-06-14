// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

using UnrealBuildTool;
using System.Collections.Generic;

public class EnvironmentProjectEditorTarget : TargetRules
{
	public EnvironmentProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("EnvironmentProject");
		DefaultBuildSettings = BuildSettingsVersion.V2;
	}
}
