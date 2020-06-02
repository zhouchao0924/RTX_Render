//MIT License

//Copyright(c) 2018 annsar

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

using UnrealBuildTool;
using System.IO;

public class CGAL : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

	public CGAL(ReadOnlyTargetRules Target) : base(Target)
	{
        PublicDefinitions.Add("WIN32");
        PublicDefinitions.Add("_WINDOWS");
        PublicDefinitions.Add("_CRT_SECURE_NO_DEPRECATE");
        PublicDefinitions.Add("_SCL_SECURE_NO_DEPRECATE");
        PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
        PublicDefinitions.Add("_SCL_SECURE_NO_WARNINGS");
        PublicDefinitions.Add("CGAL_USE_MPFR");
        PublicDefinitions.Add("CGAL_USE_GMP");

        // Startard Module Dependencies
        PublicDependencyModuleNames.AddRange(new string[] { "Core", });
        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "Slate", "SlateCore", "Projects" });

        // Start CGAL linking here!
        bool isLibrarySupported = false;

        // Create CGAL Path
        string CGALPath = Path.Combine(ThirdPartyPath, "CGAL");

        // Get Library Path
        string LibPath = "";
        bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            LibPath = Path.Combine(CGALPath, "libraries", "Win64");
            isLibrarySupported = true;
        }
        else
        {
            string Err = string.Format("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform.ToString(), this.ToString()); System.Console.WriteLine(Err);
        }

        if (isLibrarySupported)
        {
            //Add Include path
            PublicIncludePaths.AddRange(new string[] { Path.Combine(CGALPath, "includes") });
            PublicIncludePaths.AddRange(new string[] { Path.Combine(CGALPath, "includes","GMP") });//Dependencie

            // Add Dependencies
            if (!isdebug)
            {
                //Add Static Libraries
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libgmp-10.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libmpfr-4.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libCGAL-vc140-mt-4.9.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libCGAL_Core-vc140-mt-4.9.lib"));
            }
            else
            {
                //Add Static Libraries (Debug Version)
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libgmp-10.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libmpfr-4.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libCGAL-vc140-mt-gd-4.9.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libCGAL_Core-vc140-mt-gd-4.9.lib"));
            }

            PublicDelayLoadDLLs.Add(Path.Combine(LibPath, "libgmp-10.dll"));
            PublicDelayLoadDLLs.Add(Path.Combine(LibPath, "libmpfr-4.dll"));
            //RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibPath, "libgmp-10.dll")));
            //RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibPath, "libmpfr-4.dll")));
            RuntimeDependencies.Add(Path.Combine(LibPath, "libgmp-10.dll"));
            RuntimeDependencies.Add(Path.Combine(LibPath, "libmpfr-4.dll"));
        }

        PublicDefinitions.Add(string.Format("WITH_CGAL_BINDING={0}", isLibrarySupported ? 1 : 0));
        bEnableExceptions = true;

        bEnableUndefinedIdentifierWarnings = false;
    }
}
