// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BuildingSDK : ModuleRules
{
    public BuildingSDK(ReadOnlyTargetRules Target) : base(Target)
    {

        PublicIncludePaths.AddRange(
            new string[] {
                // Path.Combine(ModuleDirectory, "Components"),
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "BuildingSDKLibrary",
                "Projects",
                "ProceduralMeshComponent",
                "Json",
                "JsonUtilities",
                "DesktopPlatform",
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",                
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        //PublicDefinitions.bAdaptiveUnityDisablesPCH = false;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/BuildingSDKLibrary.dll");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/HousePluginComponent.bl");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/CraftPavingComponent.bl");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/libgmp-10.dll");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/libmpfr-4.dll");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/libfbxsdk.dll");
            RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/nvtt_64.dll");
            //RuntimeDependencies.Add("$(ProjectDir)/Plugins/BuildingSDK/Binaries/ThirdParty/BuildingSDKLibrary/Win64/libcurl.dll");
        }
    }
}
