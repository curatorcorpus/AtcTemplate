# Voxel Rendering Plugin for Unreal Engine 4.26

This is a plugin which can be integrated into almost any Unreal project created with Unreal Engine 4.26, and which *might* port to other versions of the engine. 

An example of how to use it is the `vimr_render_unreal426` project, which includes pre-built VIMR binaries which should only require the Visual Studio 2019 redistributables. 
The example is intended to serve as a minimal starting point for most VR applications of VIMR - if this is not what you want then read on.

## Starting a new Unreal Project

To start from scratch you first need to:

1. Get the VIMR binaries, and make sure that VIMR_ROOT_07 is set correctly
2. Create an new project in Unreal Engine 4.26
3. Close Unreal Editor
4. Create a `Plugins` directory in the top-level directory if it doesn't already exist
5. Place the `Voxels` directory inside `Plugins` 
6. Edit the `.uproject` file and add the `Plugins` and `PostBuildSteps` sections from the example config below
7. Re-open the project in the Unreal Editor
   1. Pray that the fickle whims of the Unreal Build Tool are satisfied
8. Look at the example project for how to create wrapper actors for the voxel components
   1. Note that the UDP source (VoxelServer) will hang if there's no VNet server, and it's useless without the rest of VIMR. Note also that this is the only way to get live voxel reconstructions
   2. VoxelVideo sources work fine on their own

```JSON
{
	"FileVersion": 3,
	"EngineAssociation": "4.26",
	"Category": "",
	"Description": "",
    "Plugins": [
        {
            "Name": "Voxels",
            "Enabled": true
        }
    ],
    "TargetPlatforms": [
        "WindowsNoEditor"
    ],
    "PostBuildSteps": {
        "Win64": [
            "IF NOT EXIST \"$(ProjectDir)\\Binaries\\$(TargetPlatform)\\vimr.dll\" copy /Y \"$(VIMR_ROOT_07)\\bin\\vimr.dll\" \"$(ProjectDir)\\Binaries\\$(TargetPlatform)\\\""
        ]
    }
}

```

## Packaging

The above will get you PIE, including VR preview, but if you want to package projects that use this plugin you also need to:

1. Make sure that a working version of the file  `Source/<project-name>/<project-name>.Build.cs` to exist.
   1. If you started with a C++ project it should be there
   2. If it's not there, then use `File>Add C++ Class` to add an empty dummy class which should create this file
2. Use the example below to add any `using` statements which are missing, and 

```CS
using System;
using System.IO;
using UnrealBuildTool;

public class vrender : ModuleRules
{
	public vrender(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		/* Add VIMR as a dependecy when packaging */
		string VIMR_ROOT = System.Environment.GetEnvironmentVariable("VIMR_ROOT_07");
		if(Target.Platform == UnrealTargetPlatform.Win64)
		{
			string ProjectRoot = Directory.GetParent(ModuleDirectory).Parent.FullName;
			RuntimeDependencies.Add(ProjectRoot + @"\Binaries\Win64\vimr.dll");
		}
	}
}

```
