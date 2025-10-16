// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CharacterTest : ModuleRules
{
	public CharacterTest(ReadOnlyTargetRules Target) : base(Target)
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
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "MotionWarping" });

		PublicIncludePaths.AddRange(new string[] {
			"CharacterTest",
			"CharacterTest/Variant_Platforming",
			"CharacterTest/Variant_Platforming/Animation",
			"CharacterTest/Variant_Combat",
			"CharacterTest/Variant_Combat/AI",
			"CharacterTest/Variant_Combat/Animation",
			"CharacterTest/Variant_Combat/Gameplay",
			"CharacterTest/Variant_Combat/Interfaces",
			"CharacterTest/Variant_Combat/UI",
			"CharacterTest/Variant_SideScrolling",
			"CharacterTest/Variant_SideScrolling/AI",
			"CharacterTest/Variant_SideScrolling/Gameplay",
			"CharacterTest/Variant_SideScrolling/Interfaces",
			"CharacterTest/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
