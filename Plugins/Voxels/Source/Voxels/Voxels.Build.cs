// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Voxels : ModuleRules
{
    public Voxels(ReadOnlyTargetRules Target) : base(Target)
    {
        Definitions.Add("JSON_NOEXCEPTION");
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        string VIMR_ROOT = System.Environment.GetEnvironmentVariable("VIMR_ROOT_07");

        PublicIncludePaths.AddRange(
            new string[] {
                "Voxels/Public"
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "Voxels/Private",
                VIMR_ROOT + "/include/"
            }
            );


        PublicAdditionalLibraries.AddRange(
            new string[]
            {
                VIMR_ROOT + "/lib/vimr.lib"
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );
    }
}
