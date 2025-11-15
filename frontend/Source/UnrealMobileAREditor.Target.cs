// Copyright © 2024 AR App. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealMobileAREditorTarget : TargetRules
{
	public UnrealMobileAREditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange(new string[] { "UnrealMobileAR" });
	}
}
