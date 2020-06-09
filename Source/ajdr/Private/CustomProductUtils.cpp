// Copyright? 2017 ihomefnt All Rights Reserved.

#include "CustomProductUtils.h"
#include "CompundResourceMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UCustomProductUtils::HasAnyParent(const FElementInfo& Target)
{
	return !Target.ParentGuid.IsEmpty();
}

int32 UCustomProductUtils::PinExists_Vector(const FElementInfo& Target, const FVector& Pin)
{
	TArray<int32> PinIds;
	
	Target.Pins.GetKeys(PinIds);

	for (int32 Index = 0; Index < PinIds.Num(); Index++)
	{
		FVector DirtyUniformLocation = Target.Pins[PinIds[Index]];

		if (Pin.Equals(DirtyUniformLocation))
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 UCustomProductUtils::PinExists_Id(const FElementInfo & Target, int32 Pin)
{
	TArray<int32> PinIds;

	Target.Pins.GetKeys(PinIds);

	return PinIds.Find(Pin);

}

bool UCustomProductUtils::CheckIsInGameThread()
{
	return IsInGameThread();
}

FBox2D UCustomProductUtils::ProjectActorBoundsToScreen(const APlayerController* Player, const FBox& Bounds)
{
	TArray<FVector2D> BoundsPoints;

	if (Player)
	{
		FVector Origin = Bounds.GetCenter();
		FVector BoxExtent = Bounds.GetExtent();

		FVector TopOrigin = Origin + FVector(0, 0, BoxExtent.Z);
		FVector BottomOrigin = Origin - FVector(0, 0, BoxExtent.Z);
		FVector2D DirtyPoint = FVector2D::ZeroVector;

		// 添加Box的八个点

		UGameplayStatics::ProjectWorldToScreen(Player, TopOrigin + FVector(-BoxExtent.X, -BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, TopOrigin + FVector(BoxExtent.X, -BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, TopOrigin + FVector(BoxExtent.X, BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, TopOrigin + FVector(-BoxExtent.X, BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, BottomOrigin + FVector(-BoxExtent.X, -BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, BottomOrigin + FVector(BoxExtent.X, -BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, BottomOrigin + FVector(BoxExtent.X, BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		DirtyPoint = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(Player, BottomOrigin + FVector(-BoxExtent.X, BoxExtent.Y, 0), DirtyPoint);
		BoundsPoints.Add(DirtyPoint);

		// 分割坐标
		TArray<float> PosX;
		TArray<float> PosY;

		for (int32 Index = 0; Index < BoundsPoints.Num(); Index++)
		{
			PosX.Add(BoundsPoints[Index].X);
			PosY.Add(BoundsPoints[Index].Y);
		}

		PosX.Sort();
		PosY.Sort();

		BoundsPoints.Empty();

		// 左上角点
		BoundsPoints.Add(FVector2D(PosX[0], PosY[0]));

		// 右上角点
		BoundsPoints.Add(FVector2D(PosX.Last(), PosY[0]));

		// 右下角点
		BoundsPoints.Add(FVector2D(PosX.Last(), PosY.Last()));

		// 左下角点
		BoundsPoints.Add(FVector2D(PosX[0], PosY.Last()));
	}

	return BoundsPoints;
}

bool UCustomProductUtils::IsPointInsideBox(const FBox2D & Box, const FVector2D & Point)
{
	return Box.IsInside(Point);
}

int32 UCustomProductUtils::CheckPinIndex(const FVector & Pin)
{
	if (Pin == FVector(0, 0, 1))
	{
		return 1;
	}
	else if (Pin == FVector(1, 0, 1))
	{
		return 2;
	}
	else if (Pin == FVector(1, 1, 1))
	{
		return 3;
	}
	else if (Pin == FVector(0, 1, 1))
	{
		return 4;
	}
	else if (Pin == FVector::ZeroVector)
	{
		return 5;
	}
	else if (Pin == FVector(1, 0, 0))
	{
		return 6;
	}
	else if (Pin == FVector(1, 1, 0))
	{
		return 7;
	}
	else if (Pin == FVector(0, 1, 0))
	{
		return 8;
	}
	else
	{
		return 0;
	}
}

FVector UCustomProductUtils::GetPinNormalizeLocation(int32 Pin)
{
	switch (Pin)
	{
	case 1:
		return FVector(0, 0, 1);

	case 2:
		return FVector(1, 0, 1);
		
	case 3:
		return FVector(1, 1, 1);

	case 4:
		return FVector(0, 1, 1);

	case 5:
		return FVector(0, 0, 0);

	case 6:
		return FVector(1, 0, 0);

	case 7:
		return FVector(1, 1, 0);

	case 8:
		return FVector(0, 1, 0);

	default:
		return FVector(-1, -1, -1);
	}
}

TArray<FVector> UCustomProductUtils::GetComponentAngularPoints(UPrimitiveComponent * Target)
{


	TArray<FVector> Result;

	if (Target == nullptr)
	{
		return Result;
	}

	FVector Original, BoxExtent;

	FBox TargetBox = Target->Bounds.GetBox();

	Original = TargetBox.GetCenter();
	BoxExtent = TargetBox.GetExtent();

	FVector TopOriginal = FVector(Original.X, Original.Y, Original.Z + BoxExtent.Z);
	FVector BottomOriginal = FVector(Original.X, Original.Y, Original.Z - BoxExtent.Z);

	// 添加八个角点，顺时针，上到下
	Result.Add(FVector(TopOriginal.X - BoxExtent.X, TopOriginal.Y - BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X + BoxExtent.X, TopOriginal.Y - BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X + BoxExtent.X, TopOriginal.Y + BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X - BoxExtent.X, TopOriginal.Y + BoxExtent.Y, TopOriginal.Z));

	Result.Add(FVector(BottomOriginal.X - BoxExtent.X, BottomOriginal.Y - BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X + BoxExtent.X, BottomOriginal.Y - BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X + BoxExtent.X, BottomOriginal.Y + BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X - BoxExtent.X, BottomOriginal.Y + BoxExtent.Y, BottomOriginal.Z));

	return Result;
}

FString UCustomProductUtils::GetNewGuid()
{
	return FGuid::NewGuid().ToString();
}

TArray<FVector> UCustomProductUtils::GetElementAngularPoints(const FVector & Original, const FVector & Size)
{
	TArray<FVector> Result;

	FVector BoxExtent(Size / 2);

	FVector TopOriginal = FVector(Original.X, Original.Y, Original.Z + BoxExtent.Z);
	FVector BottomOriginal = FVector(Original.X, Original.Y, Original.Z - BoxExtent.Z);

	// 添加八个角点，顺时针，上到下
	Result.Add(FVector(TopOriginal.X - BoxExtent.X, TopOriginal.Y - BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X + BoxExtent.X, TopOriginal.Y - BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X + BoxExtent.X, TopOriginal.Y + BoxExtent.Y, TopOriginal.Z));
	Result.Add(FVector(TopOriginal.X - BoxExtent.X, TopOriginal.Y + BoxExtent.Y, TopOriginal.Z));

	Result.Add(FVector(BottomOriginal.X - BoxExtent.X, BottomOriginal.Y - BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X + BoxExtent.X, BottomOriginal.Y - BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X + BoxExtent.X, BottomOriginal.Y + BoxExtent.Y, BottomOriginal.Z));
	Result.Add(FVector(BottomOriginal.X - BoxExtent.X, BottomOriginal.Y + BoxExtent.Y, BottomOriginal.Z));

	return Result;
}

FVector UCustomProductUtils::GetPinFixedWorldLocation(const FVector & Original, const FVector & Location)
{
	// 当前点到中心点的距离
	float Distance = (Original - Location).Size();
	// 中心点到当前点的方向
	FVector LocationDir = Location - Original;
	LocationDir.Normalize();
	// 根据距离和方向得到基于坐标原点的新点
	FVector OriginLocation = FVector::ZeroVector + LocationDir * Distance;

	// 坐标原点到中心点的距离
	float OriginDistance = (Original - FVector::ZeroVector).Size();
	// 坐标原点到中心点的方向
	FVector OriginDir = Original - FVector::ZeroVector;
	OriginDir.Normalize();

	// 获得抵消旋转后的角度
	OriginLocation = UKismetMathLibrary::RotateAngleAxis(OriginLocation, 180, FVector(0, 0, 1));

	// 根据坐标原点到中心点的距离和方向计算新点的坐标
	FVector FixedLocation = OriginLocation + OriginDir * OriginDistance;

	return FixedLocation;
}

FString UCustomProductUtils::Conv_VectorToString(FVector InVec)
{
	return FString::Printf(TEXT("X=%f Y=%f Z=%f"), InVec.X, InVec.Y, InVec.Z);;
}

bool UCustomProductUtils::HasSpecifiedMeshName(int32 InCategoryId, TArray<FMeshInfo> InMeshInfo)
{
	TArray<FMeshInfo> MeshInfos = InMeshInfo;
	bool bHasSpecifiedMeshName = false;
	for ( int32 i=0;i<MeshInfos.Num();i++)
	{
		if (MeshInfos[i].CategoryId == InCategoryId)
		{
			bHasSpecifiedMeshName = true;
			break;
		}
	}
	return bHasSpecifiedMeshName;
}

TArray<FString> UCustomProductUtils::GetMeshNamesByCategoryId(int32 InCategoryId, TArray<FMeshInfo> InMeshInfo)
{
	TArray<FMeshInfo> MeshInfos = InMeshInfo;
	TArray<FString> MeshNames;
	for (int32 i = 0; i < MeshInfos.Num(); i++)
	{
		if (MeshInfos[i].CategoryId == InCategoryId)
		{
			MeshNames=MeshInfos[i].MeshName;
		}
	}
	return MeshNames; 
}

void UCustomProductUtils::SetElementSurfaceByCategoryId(UCompundResourceMeshComponent *InCompoundResourceMesh, int32 InElementId, int32 InCategoryId, TArray<FMeshInfo> InMeshInfo, FString ResID)
{
	TArray<FString> MyMeshNames = GetMeshNamesByCategoryId(InCategoryId, InMeshInfo);
	for (int i=0;i<MyMeshNames.Num();i++)
	{
		InCompoundResourceMesh->SetElementSurfaceByMeshName(InElementId,MyMeshNames[i], ResID);
	}
}

void UCustomProductUtils::SetElementSurface(UCompundResourceMeshComponent * InCompoundResourceMesh, int32 InElementId, int32 InCategoryId, TArray<FMeshInfo> InMeshInfo, FString ResID)
{

	if (UCustomProductUtils::HasSpecifiedMeshName(InCategoryId,InMeshInfo))
	{
		UCustomProductUtils::SetElementSurfaceByCategoryId(InCompoundResourceMesh, InElementId, InCategoryId, InMeshInfo,ResID);
	}
	else
	{
		InCompoundResourceMesh->SetElementSurfaceByMeshName(InElementId,"mesh_color1", ResID);
	}
}

