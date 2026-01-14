// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TakesTwo : ModuleRules
{
	public TakesTwo(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TakesTwo",
			"TakesTwo/Variant_Platforming",
			"TakesTwo/Variant_Platforming/Animation",
			"TakesTwo/Variant_Combat",
			"TakesTwo/Variant_Combat/AI",
			"TakesTwo/Variant_Combat/Animation",
			"TakesTwo/Variant_Combat/Gameplay",
			"TakesTwo/Variant_Combat/Interfaces",
			"TakesTwo/Variant_Combat/UI",
			"TakesTwo/Variant_SideScrolling",
			"TakesTwo/Variant_SideScrolling/AI",
			"TakesTwo/Variant_SideScrolling/Gameplay",
			"TakesTwo/Variant_SideScrolling/Interfaces",
			"TakesTwo/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
