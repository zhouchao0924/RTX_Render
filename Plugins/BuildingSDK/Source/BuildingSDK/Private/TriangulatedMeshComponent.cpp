
#include "TriangulatedMeshComponent.h"

UTriangulatedMeshComponent::UTriangulatedMeshComponent(FObjectInitializer const &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UTriangulatedMeshComponent::CreatePolygon(const TArray<FVector2D> &Polygon, float Thickness)
{
	IBuildingSDK *SDK = GetBuildingSDK();
	ITriangulatedMesh *MeshEditor = SDK ? SDK->GetTriMeshEditor() : nullptr;
	if (MeshEditor && !Resource.IsValid())
	{
		kArray<kPoint> kPolygon;
		kPolygon.set((kPoint *)Polygon.GetData(), Polygon.Num());
		ObjectID ID = MeshEditor->CreatePlane(kPolygon, Thickness, EAlign_PlaneZLow);
		if (ID != INVALID_OBJID)
		{
			SetResource(ID);
		}
	}
}

void UTriangulatedMeshComponent::AddHole(const TArray<FVector2D> &Hole)
{
	IBuildingSDK *SDK = GetBuildingSDK();
	ITriangulatedMesh *MeshEditor = SDK ? SDK->GetTriMeshEditor() : nullptr;
	if (MeshEditor && Resource.IsValid())
	{
		kArray<kPoint> kHole;
		kHole.set((kPoint *)Hole.GetData(), Hole.Num());
		MeshEditor->AddHole(Resource.GetID(), kHole);
	}
}

void UTriangulatedMeshComponent::SetSurface(const FString &MaterialUri)
{
	IBuildingSDK *SDK = GetBuildingSDK();
	std::string AnsiMaterialUri = TCHAR_TO_ANSI(*MaterialUri);
	ITriangulatedMesh *MeshEditor = SDK ? SDK->GetTriMeshEditor() : nullptr;

	if (MeshEditor && Resource.IsValid())
	{
		int32 FoundIndex = INDEX_NONE;
		if (MaterialUri.FindLastChar(_T('.'), FoundIndex))
		{
			MeshEditor->SetSurface(Resource.GetID(), EUE4Material, AnsiMaterialUri.c_str());
		}
		else
		{
			MeshEditor->SetSurface(Resource.GetID(), ERefMaterial, AnsiMaterialUri.c_str());
		}
	}
}

