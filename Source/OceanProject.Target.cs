// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class OceanProjectTarget : TargetRules
{
	public OceanProjectTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "OceanProject" } );
	}
}
