// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimFinal : ModuleRules
{
	public AnimFinal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AnimFinal",
			"AnimFinal/Variant_Platforming",
			"AnimFinal/Variant_Platforming/Animation",
			"AnimFinal/Variant_Combat",
			"AnimFinal/Variant_Combat/AI",
			"AnimFinal/Variant_Combat/Animation",
			"AnimFinal/Variant_Combat/Gameplay",
			"AnimFinal/Variant_Combat/Interfaces",
			"AnimFinal/Variant_Combat/UI",
			"AnimFinal/Variant_SideScrolling",
			"AnimFinal/Variant_SideScrolling/AI",
			"AnimFinal/Variant_SideScrolling/Gameplay",
			"AnimFinal/Variant_SideScrolling/Interfaces",
			"AnimFinal/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
