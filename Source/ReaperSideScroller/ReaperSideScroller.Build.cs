// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ReaperSideScroller : ModuleRules
{
	public ReaperSideScroller(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Paper2D" });
	}
}
