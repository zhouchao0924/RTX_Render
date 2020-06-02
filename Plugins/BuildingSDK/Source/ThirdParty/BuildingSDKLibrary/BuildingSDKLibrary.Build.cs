// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class BuildingSDKLibrary : ModuleRules
{
	public BuildingSDKLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Debug", "BuildingSDKLibrary.lib"));

            PublicIncludePaths.AddRange(
               new string[] {
                    Path.Combine(ModuleDirectory, "Public"),
               });

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "x64", "Debug", "BuildingSDKLibrary.dll"));
		}
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libBuildingSDKLibrary.dylib"));
        }
	}
}
