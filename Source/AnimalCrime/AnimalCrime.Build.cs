// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimalCrime : ModuleRules
{
	public AnimalCrime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Æú´õ
        PublicIncludePaths.AddRange(new[] { "AnimalCrime" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
