// Some copyright should be here...
using System.IO;
using UnrealBuildTool;

public class PeExtendedToolKit : ModuleRules
{
	public PeExtendedToolKit(ReadOnlyTargetRules Target) : base(Target)
    {

        //PublicIncludePaths.AddRange(
        //	new string[] {
        //		"PeExtendedToolKit/Public"

        //		 ... add public include paths required here ...
        //	}
        //	);


        //PrivateIncludePaths.AddRange(
        //	new string[] {
        //		"PeExtendedToolKit/Private",

        //		 ... add other private include paths required here ...
        //	}
        //	);
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
                 
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "ProceduralMeshComponent",
                "APPFRAMEWORK" ,
                "RHI",
                "RenderCore",
                "UMG",
               // "DesktopPlatform",
                "ImageWrapper",
             //   "Kismet"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
