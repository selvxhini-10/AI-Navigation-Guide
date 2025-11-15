// Copyright © 2024 Smart Navigation Cane. All Rights Reserved.

using UnrealBuildTool;

public class SmartNavigationCane : ModuleRules
{
	public SmartNavigationCane(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"HTTP",
			"Json",
			"JsonUtilities",
			"MediaIOFramework",
			"OnlineSubsystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
		});
	}
}
