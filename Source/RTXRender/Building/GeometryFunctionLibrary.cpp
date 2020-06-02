

#include "GeometryFunctionLibrary.h"
#include "BuildingSDK.h"
#include "IBuildingSDK.h"

bool UGeometryFunctionLibrary::GetBoundary(const FString &MXFilename, TArray<FVector2D> &OutBoundary)
{
	FBuildingSDKModule &SDKModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
	IBuildingSDK *BuildingSDK = SDKModule.GetSDK();
	if (BuildingSDK)
	{
		IGeometry *geometryLib = BuildingSDK->GetGeometryLibrary();
		std::string ansiFilename = TCHAR_TO_ANSI(*MXFilename);
		IObject *mxFile = BuildingSDK->LoadFile(ansiFilename.c_str());
		if (mxFile)
		{
			IMeshObject *meshObj = mxFile->GetMeshObject(0);
			if (meshObj)
			{
				int num = 0;
				unsigned char *bCloseFlags = nullptr;
				kArray<kPoint> *boundaries = nullptr;
				if (geometryLib->GetMesh2DBoundary(meshObj, boundaries, bCloseFlags, num))
				{
					bool bClosed = false;
					int maxPolyCount = 0;
					int maxBoundaryIndex = -1;

					for (int i = 0; i < num; ++i)
					{
						kArray<kPoint> &boundary = boundaries[i];
						if (boundary.size() > maxPolyCount)
						{
							maxPolyCount = boundary.size();
							maxBoundaryIndex = i;
							bClosed = bCloseFlags[i];
						}
					}

					if (maxBoundaryIndex >= 0)
					{
						kArray<kPoint> &boundary = boundaries[maxBoundaryIndex];
						OutBoundary.SetNum(boundary.size());
						FMemory::Memcpy(OutBoundary.GetData(), boundary.data(), boundary.bytes());
						if (bClosed)
						{
							FVector2D Loc = OutBoundary[0];
							OutBoundary.Add(Loc);
						}
						return true;
					}
				}
			}
			mxFile->Delete();
		}
	}

	return false;
}

