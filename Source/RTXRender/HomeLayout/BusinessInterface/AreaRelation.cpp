// Fill out your copyright notice in the Description page of Project Settings.
// material node
#pragma once


#include "AreaRelation.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"


void USkirtingSegObject::SetStart(float InSegStart)
{
	SegStart = InSegStart;
}

void USkirtingSegObject::SetEnd(float InSegEnd)
{
	SegEnd = InSegEnd;
}

FSkirtingCollectionHandle USkirtingSegObject::GetSkirtingCollection()
{
	FSkirtingCollectionHandle SkirtingCollection;
	SkirtingCollection.SkirtingLength = CalculateSkirtingLength();
	SkirtingCollection.SkirtingModelID = GetSkirtingModelID();
	SkirtingCollection.SkirtingMaterialID = GetSkirtingMaterialID();
	SkirtingCollection.SkirtingType = RelateSkirtingSeg->GetRelatedNode().SkirtingType;
	SkirtingCollection.SkirtingSegNode = GetRelatedSkirtingSegNode();

	return SkirtingCollection;
}

FSkirtingSegNode USkirtingSegObject::GetRelatedSkirtingSegNode()
{
	return RelateSkirtingSeg->GetRelatedNode();
}

double USkirtingSegObject::CalculateSkirtingLength() const
{
	return SegEnd - SegStart;
}

FString USkirtingSegObject::GetSkirtingModelID() const
{
	return RelateSkirtingSeg->GetRelatedNode().SkirtingMeshID;
}

FString USkirtingSegObject::GetSkirtingMaterialID() const
{
	return RelateSkirtingSeg->GetRelatedNode().MaterialID;
}


UWallSegObject::UWallSegObject()
	:bSameDirWithRegion(false), bWallInRegion(false)
{

}

FWallCollectionHandle UWallSegObject::GetWallCollection()
{
	FWallCollectionHandle WallCollection;

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WallLayOnSeg);
	if (projectDataManager == nullptr) {
		return WallCollection;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallLayOnSeg->ObjectId));
	if (solidWallAdapter == nullptr) {
		return WallCollection;
	}

	TArray<FDRMaterial> WallMats = solidWallAdapter->GetWallMaterials();
	if (WallMats.Num() > GetFaceType(FaceType))
	{
		FDRMaterial currentWallMat = WallMats[GetFaceType(FaceType)];
		WallCollection.WallMaterial.ModelID = currentWallMat.ModelID;
		WallCollection.WallMaterial.RoomClassID = currentWallMat.RoomClassID;
		WallCollection.WallMaterial.CraftID = currentWallMat.CraftID;
	}
	WallCollection.WallArea = WallLayOnSeg->GetFaceArea(GetFaceType(FaceType));
	WallCollection.FaceType = FaceType;
	return WallCollection;
}

int32 UWallSegObject::GetFaceType(EWallSurfaceType & Type)
{
	switch (Type)
	{
	case EWallSurfaceType::Front:
		return 4;
	case EWallSurfaceType::Back:
		return 5;
	case EWallSurfaceType::StartSide:
		return 2;
	case EWallSurfaceType::EndSide:
		return 3;
	case EWallSurfaceType::Top:
		return 1;
	case EWallSurfaceType::Bottom:
		return 0;
	}
	return -1;
}

double UWallSegObject::BuildWallArea()
{
	using FPolygon2DWithHoles = FExactDataConv::FP2CPolygonWithHoles2D;
	using FPolygon2D = FExactDataConv::FP2CPolygon2D;
	using FSegment2D = FExactDataConv::FP2CSegment2D;
	using FPoint2D = FExactDataConv::FP2CPoint2D;
	using FPolygonSet2D = FExactDataConv::FP2CPolygonSet2D;
	OpeningsOnThisWallSurface.Empty();

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WallLayOnSeg);
	if (projectDataManager == nullptr) {
		return 0.0;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallLayOnSeg->ObjectId));
	if (solidWallAdapter == nullptr) {
		return 0.0;
	}

	auto WallHeight = solidWallAdapter->GetHeight();

	double WallSegLength = std::sqrt(CGAL::squared_distance(FExactDataConv::Vector2D2CgPointConv(WallSegStart),
		FExactDataConv::Vector2D2CgPointConv(WallSegEnd)));
	if (WallSegLength < SmallPositiveNumber)
	{
		return 0.0f;
	}

	SegStart = 0.0f;
	SegEnd = WallSegLength;
	double WallZPos = 0.0f;

	FPolygon2D WallSurface;
	WallSurface.push_back(FPoint2D(SegStart, WallZPos));
	WallSurface.push_back(FPoint2D(SegEnd, WallZPos));
	WallSurface.push_back(FPoint2D(SegEnd, WallHeight));
	WallSurface.push_back(FPoint2D(SegStart, WallHeight));
	FPolygon2DWithHoles WallSurfaceWithOpenings(WallSurface);
	FPoint2D WallStartPnt(FExactDataConv::Vector2D2CgPointConv(WallSegStart));
	FPoint2D WallEndPnt(FExactDataConv::Vector2D2CgPointConv(WallSegEnd));
	FSegment2D WallSegment(WallStartPnt, WallEndPnt);
	auto WallSegLine = WallSegment.supporting_line();

	FPolygonSet2D WallSurfacePolySet;
	WallSurfacePolySet.insert(WallSurface);
	if (FaceType == EWallSurfaceType::Front || FaceType == EWallSurfaceType::Back)
	{
		auto OpeningsOnWall = WallLayOnSeg->GetOpeningsOnWall();
		for (auto OpeningOnWall : OpeningsOnWall)
		{
			FOpeningSimpleAttr OpeningSimpleAttr = OpeningOnWall->GetOpeningSimpleAttr();
			double OpeningZPos = (double)OpeningSimpleAttr.ZPos;
			if (OpeningZPos > WallHeight)
			{
				continue;
			}

			// project opening to region segment
			FPoint2D OpeningLocPnt(FExactDataConv::Vector2D2CgPointConv(OpeningSimpleAttr.Location));
			FPoint2D OpeningLocPrjPnt;
			bool bOpeningOnWallSeg = ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(OpeningLocPrjPnt,
				OpeningLocPnt,
				WallSegment, &WallSegLine);
			if (bOpeningOnWallSeg)
			{
				// collect openings on this surface
				switch (OpeningSimpleAttr.Type)
				{
				case EOpeningType::DoorWay:
				{
					OpeningsOnThisWallSurface.Add(OpeningOnWall);
				}
				break;
				case EOpeningType::SlidingDoor:
				{
					OpeningsOnThisWallSurface.Add(OpeningOnWall);
				}
				break;
				case EOpeningType::SingleDoor:
				case EOpeningType::DoubleDoor:
				{
					if (bWallInRegion || (bSameDirWithRegion ^ OpeningOnWall->VerticalFlip))
					{
						OpeningsOnThisWallSurface.Add(OpeningOnWall);
					}
				}
				break;
				case EOpeningType::Window:
				case EOpeningType::BayWindow:
				case EOpeningType::FrenchWindow:
				{
					OpeningsOnThisWallSurface.Add(OpeningOnWall);
				}
				break;
				default:
					break;
				}
			}

			double OpeningPos2RegionStart = std::sqrt(CGAL::squared_distance(OpeningLocPrjPnt, WallStartPnt));
			double OpeningSegStart = OpeningPos2RegionStart - (double)OpeningSimpleAttr.LeftWidth;
			double OpeningSegEnd = OpeningPos2RegionStart + (double)OpeningSimpleAttr.RightWidth;
			if (OpeningSegStart < SegStart)
			{
				OpeningSegStart = SegStart;
			}
			if (OpeningSegEnd > SegEnd)
			{
				OpeningSegEnd = SegEnd;
			}

			double OpeningZHeightPos = OpeningZPos + (double)OpeningSimpleAttr.Height;
			if (OpeningZHeightPos > WallHeight)
			{
				OpeningZHeightPos = WallHeight;
			}

			double OpeningProjectWidth = FMath::Abs(OpeningSegEnd - OpeningSegStart);
			if (OpeningProjectWidth < SmallPositiveNumber)
			{
				continue;
			}
			double OpeningProjectHeight = FMath::Abs(OpeningZHeightPos - OpeningZPos);
			if (OpeningProjectHeight < SmallPositiveNumber)
			{
				continue;
			}
			if (OpeningZPos < WallZPos)
			{
				OpeningZPos = WallZPos;
			}

			FPolygon2D OpeningPolygon;
			OpeningPolygon.push_back(FPoint2D(OpeningSegStart, OpeningZPos));
			OpeningPolygon.push_back(FPoint2D(OpeningSegEnd, OpeningZPos));
			OpeningPolygon.push_back(FPoint2D(OpeningSegEnd, OpeningZHeightPos));
			OpeningPolygon.push_back(FPoint2D(OpeningSegStart, OpeningZHeightPos));
			WallSurfacePolySet.difference(OpeningPolygon);
		}
	}

	FExactDataConv::FP2CPwhList2D WallSurfaceWithOpening;
	WallSurfacePolySet.polygons_with_holes(std::back_inserter(WallSurfaceWithOpening));
	double AreaOfWallSurfaceWithOpening = 0.0f;
	for (FExactDataConv::FP2CPwhList2D::const_iterator iterPoly = WallSurfaceWithOpening.begin(); iterPoly != WallSurfaceWithOpening.end(); ++iterPoly)
	{
		AreaOfWallSurfaceWithOpening += std::abs(iterPoly->outer_boundary().area());
		double HolesArea = 0.0f;
		for (auto IterHole = iterPoly->holes_begin(); IterHole != iterPoly->holes_end(); ++IterHole)
		{
			HolesArea += std::abs(IterHole->area());
		}
		AreaOfWallSurfaceWithOpening -= HolesArea;
	}

	return AreaOfWallSurfaceWithOpening;
}

void UWallSegObject::SetStart(double InSegStart)
{
	SegStart = InSegStart;
}

void UWallSegObject::SetEnd(double InSegEnd)
{
	SegEnd = InSegEnd;
}

void UWallSegObject::SetWallSegStart(const FVector2D &InWallSegStart)
{
	WallSegStart = InWallSegStart;
}

void UWallSegObject::SetWallSegEnd(const FVector2D &InWallSegEnd)
{
	WallSegEnd = InWallSegEnd;
}

void UWallSegObject::SetRegionStart(const FVector2D &InRegionStart)
{
	OwnerRegionSegStart = InRegionStart;
}

void UWallSegObject::SetRegionEnd(const FVector2D &InRegionEnd)
{
	OwnerRegionSegEnd = InRegionEnd;
}

void UWallSegObject::BeginDestroy()
{
	OpeningsOnThisWallSurface.Empty();

	Super::BeginDestroy();
}


const FRegionCollection& URegionObject::CalculateRegionCollection()
{
	RegionCollection.WallCollections.Empty();
	TArray<UWallSurfaceObject*> WallsRelated2Region;
	WallsRelated2Region.Append(WallsCcb);
	WallsRelated2Region.Append(WallsInThisRegion);
	for (auto WallRelated : WallsRelated2Region)
	{
		for (auto WallSeg : WallRelated->WallsLayOnThisSeg)
		{
			RegionCollection.WallCollections.Add(WallSeg->GetWallCollection());
		}
	}

	RegionCollection.CalculateWallCollections();

	RegionCollection.SkirtingCollections.Empty();
	for (auto SkirtingSeg : SkirtingLines)
	{
		RegionCollection.SkirtingCollections.Add(SkirtingSeg->GetSkirtingCollection());
	}
	RegionCollection.CalculateSkirtingCollections();

	return RegionCollection;
}

TArray<FSkirtingCollectionHandle> URegionObject::MergeSameSkirtingLines()
{
	TArray<FSkirtingCollectionHandle> SkirtingArry;
	for (auto TempSkirtingLine : SkirtingLines)
	{
		if(TempSkirtingLine->GetSkirtingCollection().SkirtingLength>2)
			SkirtingArry.Add(TempSkirtingLine->GetSkirtingCollection());
	}
	for (int i = 0;i < SkirtingArry.Num()-1;++i)
	{
		for (int j = i+1;j < SkirtingArry.Num();++j)
		{
			if (SkirtingArry[i].SkirtingType== SkirtingArry[j].SkirtingType
				&&SkirtingArry[i].SkirtingModelID == SkirtingArry[j].SkirtingModelID
				&&SkirtingArry[i].SkirtingMaterialID == SkirtingArry[j].SkirtingMaterialID
				&&SkirtingArry[i].SkirtingSegNode.CraftId== SkirtingArry[j].SkirtingSegNode.CraftId
				&&SkirtingArry[i].SkirtingSegNode.RoomClassID == SkirtingArry[j].SkirtingSegNode.RoomClassID)
			{
				SkirtingArry[i].SkirtingLength += SkirtingArry[j].SkirtingLength;
				FSkirtingCollectionHandle temp;
				temp = SkirtingArry[j];
				SkirtingArry[j] = SkirtingArry[SkirtingArry.Num() - 1];
				SkirtingArry[SkirtingArry.Num() - 1] = temp;
				SkirtingArry.RemoveAt(SkirtingArry.Num() - 1);
				--j;
			}
		}
	}
	return SkirtingArry;
}