// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class RTXRender : ModuleRules
{
	public RTXRender(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableUndefinedIdentifierWarnings = false;
        bEnableExceptions = true;

        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");

        PublicIncludePaths.AddRange( new string[] {
            ModuleDirectory,
            Path.Combine(ModuleDirectory, "Model"),
            Path.Combine(ModuleDirectory, "Data"),
            Path.Combine(ModuleDirectory, "Struct"),
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RHI",
            "ImageWrapper",
            "TextureCompressor",
            "FBX",
            "TargetPlatform",
            "Networking",
            "OpenSSL",
            "TextureFormatDXT",
            "nvTextureTools"
        });

		PublicDependencyModuleNames.AddRange(new string[] {
            "BuildingSDK",
            "CGAL",
            "VaRest",
            "PeExtendedToolKit",
            "CustomLayout"
        });
	}
}
