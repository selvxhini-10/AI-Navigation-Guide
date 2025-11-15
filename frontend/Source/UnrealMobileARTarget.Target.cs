// Copyright © 2024 AR App. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealMobileARTarget : TargetRules
{
	public UnrealMobileARTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange(new string[] { "UnrealMobileAR" });
	}
}
