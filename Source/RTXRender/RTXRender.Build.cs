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

		PrivateDependencyModuleNames.AddRange(new string[] {
            "BuildingSDK",
            "CGAL",
            "VaRest",
            "PeExtendedToolKit",
            "CustomLayout"
        });
	}
}
