// Copyright? 2017 ihomefnt All Rights Reserved.

#include "CraftPaving/PaveMethodFunctionLibrary.h"
#include "CraftPaving/CraftPavingComponent.h"
#include "CraftPaving/IPaveEditor.h"
#include "DesktopPlatformModule.h"
#include "ProceduralMeshComponent.h"
#include "Misc/Paths.h"

bool UPaveMethodFunctionLibrary::PaveArea(UProceduralMeshComponent * PaveComponent, UCraftPavingComponent * PatternComponent, const TArray<FVector>& Area)
{
	if (PaveComponent == nullptr || PatternComponent == nullptr || PatternComponent->GetInternalObject() == nullptr) {
		return false;
	}


	FBuildingSDKModule &sdkModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));

	ISuite* projectSuite(sdkModule.GetSDK()->GetSuite(""));
	if (projectSuite == nullptr) {
		projectSuite = sdkModule.GetSDK()->CreateSuite("");
	}

	IPaveEditor* paveEditor = QueryInterface<IPaveEditor>(sdkModule.GetSDK(), "PaveMethodEditor");
	if (paveEditor == nullptr) {
		return false;
	}

	std::vector<kPoint> convertedArea;
	for (const FVector& var : Area)
	{
		convertedArea.push_back(kPoint(var.X, var.Y));
	}

	ObjectID paveId = paveEditor->PavePolygon(kArray<kPoint>(convertedArea), kArray<kArray<kPoint>>() , PatternComponent->GetInternalObject()->GetRawObject());
	
	IObject* paveObj = projectSuite->GetObject(paveId);
	if (paveObj == nullptr) {
		return false;
	}

	IMeshObject* paveMesh =  paveObj->GetMeshObject();
	if (paveMesh == nullptr) {
		return false;
	}
	PaveComponent->ClearAllMeshSections();
	for (int32 sectionIndex = 0; sectionIndex < paveMesh->GetSectionCount(); sectionIndex++)
	{
		float *paveVertices(nullptr);
		float *paveNormals(nullptr);
		float *paveTangents(nullptr);
		float *paveUVs(nullptr); 
		float *paveLightmapUVs(nullptr);
		int paveNumVerts;
		int *paveIndices(nullptr);
		int paveNumIndices;
		if (!paveMesh->GetSectionMesh(sectionIndex, paveVertices, paveNormals, paveTangents, paveUVs, paveLightmapUVs, paveNumVerts, paveIndices, paveNumIndices)) {
			continue;
		}

		TArray<FVector> vertices, normals;
		TArray<int32> triangles;
		TArray<FVector2D> uvs;
		TArray<FColor> vertexColor;
		TArray<FProcMeshTangent> tangents;

		uvs.SetNum(paveNumVerts);
		vertices.SetNum(paveNumVerts);
		normals.SetNum(paveNumVerts);
		tangents.SetNum(paveNumVerts);
		vertexColor.SetNum(paveNumVerts);
		triangles.SetNum(paveNumIndices);

		FMemory::Memcpy(vertices.GetData(), paveVertices, paveNumVerts * 12);
		FMemory::Memcpy(normals.GetData(), paveNormals, paveNumVerts * 12);

		for (int32 tangentIndex = 0; tangentIndex < tangents.Num(); ++tangentIndex)
		{
			vertexColor[tangentIndex] = FColor::White;
			tangents[tangentIndex].TangentX = *((FVector *)(paveTangents + 3 * tangentIndex));
		}

		FMemory::Memcpy(uvs.GetData(), paveUVs, paveNumVerts * 8);
		FMemory::Memcpy(triangles.GetData(), paveIndices, paveNumIndices * 4);

		PaveComponent->CreateMeshSection(sectionIndex, vertices, triangles, normals, uvs, vertexColor, tangents, true);
	}

	paveEditor->DeletePave(paveId);
	return true;
}

bool UPaveMethodFunctionLibrary::GetResourceLocalVersion(const FString& FilePath, int32& LocalVersion)
{
	if (FilePath.IsEmpty() || !FPaths::FileExists(FilePath)) {
		return false;
	}

	unsigned int localVersion(0);

	FBuildingSDKModule &sdkModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
	bool bSuccess = sdkModule.GetSDK()->GetResourceMgr()->LoadHeader(TCHAR_TO_ANSI(*FilePath), &localVersion, nullptr, nullptr);

	LocalVersion = localVersion;
	
	return bSuccess;
}
