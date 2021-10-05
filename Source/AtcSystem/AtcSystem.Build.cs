// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class AtcSystem : ModuleRules
{
	public AtcSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", "Networking" });

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		string VIMR_ROOT = System.Environment.GetEnvironmentVariable("VIMR_ROOT_07");
		PrivateDependencyModuleNames.AddRange(new string[] { });
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string ProjectRoot = Directory.GetParent(ModuleDirectory).Parent.FullName;
			RuntimeDependencies.Add(ProjectRoot + @"\Binaries\Win64\vimr.dll");
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
