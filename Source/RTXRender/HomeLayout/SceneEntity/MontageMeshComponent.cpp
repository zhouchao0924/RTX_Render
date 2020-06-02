// Fill out your copyright notice in the Description page of Project Settings.

#include "MontageMeshComponent.h"
#include "SolidGeometry/SolidGeometryFunctionLibrary.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "EditorGameInstance.h"
#include "HomeLayout/SceneEntity/Wall_Boolean_Base.h"
#include "HomeLayout/SceneEntity/LineWallActor.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "Model/ModelFileComponent.h"
#include "Model/ResourceMgr.h"
#include "Building/BuildingSystem.h"


#pragma warning(disable : 4668)

UWallLine::UWallLine(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UWallLine::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	OnLineTypeChanged();
}
#endif

void UWallLine::ChangeLineType(UStaticMesh *InNewMeshType)
{
	Mesh = InNewMeshType;
	OnLineTypeChanged();
}

void UWallLine::OnLineTypeChanged()
{
	if (!Mesh)
		return;

	FStaticMeshLODResources &LODResource = Mesh->RenderData->LODResources[0];
	if (LODResource.GetNumVertices()!= 0)
	{
		FIndexArrayView IndexView = LODResource.IndexBuffer.GetArrayView();
		Indices.SetNum(IndexView.Num());
		for (int i = 0; i < IndexView.Num(); ++i)
		{
			Indices[i] = IndexView[i];
		}

		int32 NumVerts = LODResource.VertexBuffers.PositionVertexBuffer.GetNumVertices();
		if (NumVerts != 0)
		{
			Positions.SetNum(NumVerts);
			Normals.SetNum(NumVerts);
			Tangents.SetNum(NumVerts);
			UVs.SetNum(NumVerts);
			Colors.SetNum(NumVerts);

			uint32 NumUVCoords = LODResource.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();

			for (int32 i = 0; i < NumVerts; ++i)
			{
				Positions[i] = LODResource.VertexBuffers.PositionVertexBuffer.VertexPosition(i);
				Normals[i] = LODResource.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
				UVs[i] = NumUVCoords > 0 ? LODResource.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0) : FVector2D::ZeroVector;
				FVector TangentX = FVector::CrossProduct(LODResource.VertexBuffers.StaticMeshVertexBuffer.VertexTangentY(i), Normals[i]);
				Tangents[i].TangentX = TangentX;
				if (LODResource.VertexBuffers.ColorVertexBuffer.GetNumVertices() > 0)
				{
					Colors[i] = LODResource.VertexBuffers.ColorVertexBuffer.VertexColor(i);
				}
			}
		}
		
	}
	
}


UStaticMeshInfoExtract::UStaticMeshInfoExtract(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UStaticMeshInfoExtract::ResetStaticMesh(UStaticMesh *InNewMeshType)
{
	Mesh = InNewMeshType;
	if (!Mesh)
		return;

	ClearDataInfo();

	FStaticMeshLODResources &LODResource = Mesh->RenderData->LODResources[0];
	if (LODResource.GetNumVertices()!=0)
	{
		FIndexArrayView IndexView = LODResource.IndexBuffer.GetArrayView();
		Indices.SetNum(IndexView.Num());
		for (int i = 0; i < IndexView.Num(); ++i)
		{
			Indices[i] = IndexView[i];
		}

		int32 NumVerts = LODResource.VertexBuffers.PositionVertexBuffer.GetNumVertices();
		if (NumVerts > 0)
		{
			Positions.SetNum(NumVerts);
			Normals.SetNum(NumVerts);
			Tangents.SetNum(NumVerts);
			UVs.SetNum(NumVerts);
			Colors.SetNum(NumVerts);

			uint32 NumUVCoords = LODResource.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();


			for (int32 i = 0; i < NumVerts; ++i)
			{
				Positions[i] = LODResource.VertexBuffers.PositionVertexBuffer.VertexPosition(i);
				Normals[i] = LODResource.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
				UVs[i] = NumUVCoords > 0 ? LODResource.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0) : FVector2D::ZeroVector;
				FVector TangentX = FVector::CrossProduct(LODResource.VertexBuffers.StaticMeshVertexBuffer.VertexTangentY(i), Normals[i]);
				Tangents[i].TangentX = TangentX;
				if (LODResource.VertexBuffers.ColorVertexBuffer.GetNumVertices() > 0)
				{
					Colors[i] = LODResource.VertexBuffers.ColorVertexBuffer.VertexColor(i);
				}
			}
		}
		
	}
	
}

void UStaticMeshInfoExtract::ResetMXMesh(UModelFile *InNewMesh)
{
	MxMesh = InNewMesh;
	if (!MxMesh)
		return;
	
	ClearDataInfo();

	auto &SubModels = InNewMesh->GetSubModels();
	for (auto &SubModel : SubModels)
	{
		for (auto &ModelSection : SubModel->Sections)
		{
			for (auto &VertexBuffer : ModelSection.ProcVertexBuffer)
			{
				Positions.Add(VertexBuffer.Position);
				Normals.Add(VertexBuffer.Normal);
				Tangents.Add(VertexBuffer.Tangent);
				UVs.Add(VertexBuffer.UV0);
				Colors.Add(VertexBuffer.Color);
			}
			Indices.Append(ModelSection.ProcIndexBuffer);
		}
	}
}

void UStaticMeshInfoExtract::ClearDataInfo()
{
	Indices.Empty();
	Positions.Empty();
	Normals.Empty();
	Tangents.Empty();
	UVs.Empty();
	Colors.Empty();
}


USkirtingSegComponent::USkirtingSegComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

FSkirtingSegNode& USkirtingSegComponent::GetRelatedNode()
{
	return SkirtingSegNode;
}

void USkirtingSegComponent::UpdateMeshByNode()
{
	DoClearWork();
	const float UnExpectLength = 200000;
	float SkirtingSegLength =
	FVector2D::Distance(SkirtingSegNode.SkirtingStartCorner.Position,
		SkirtingSegNode.SkirtingEndCorner.Position);
	if (SkirtingSegLength >= UnExpectLength)
	{
		DoClearWork();
		return;
	}
	FVector2D StartPos = SkirtingSegNode.SkirtingStartCorner.Position;
	FVector2D EndPos = SkirtingSegNode.SkirtingEndCorner.Position;
	
	FBox BoundingMesh;
	if (InnerStaticMeshType)
	{
		BoundingMesh = InnerStaticMeshType->LocalBounds.TransformBy(FTransform(FRotator(0, 180.0f, 0),
			InnerStaticMeshType->Offset, InnerStaticMeshType->Scale3D));
	}
	else if (InnerMeshType)
	{
		BoundingMesh = InnerMeshType->GetBoundingBox();
	}
	
	FVector MeshSize = BoundingMesh.GetSize();
	float SkirtingMeshLength = MeshSize.X;
	float SkirtingMeshWidth = MeshSize.Y;
	float SkirtingMeshHeight = MeshSize.Z;

	// calculate inner skirting point position
	FVector2D SkirtingStart2EndDir = EndPos - StartPos;
	SkirtingStart2EndDir.Normalize();
	FVector2D Start2EndYAxis = SkirtingStart2EndDir.GetRotated(90);
	float RelativeAngle = ULinearEntityLibrary::GetRotatedOfVector2D(SkirtingStart2EndDir);
	float StartSectionRelAngle = SkirtingSegNode.StartSectionAngle;
	float EndSectionRelAngle = SkirtingSegNode.EndSectionAngle;
	float StartSecSinValue = FMath::Abs(FMath::Sin(StartSectionRelAngle));
	float EndSecSinValue = FMath::Abs(FMath::Sin(EndSectionRelAngle));
	if (StartSecSinValue < SmallPositiveNumber || EndSecSinValue < SmallPositiveNumber)
	{
		return;
	}
	bool bMirror = (SkirtingSegNode.HeadingDir == ECgHeadingDirection::ELeftTurn);
	// x direction should multiply by mirror sign
	int32 MirrorSign = bMirror ? -1 : 1;
	//(FMath::Sin(EndSectionRelAngle) > 0 ? 1 : -1);
	FVector2D InnerStartPos =
		SkirtingSegNode.SkirtingStartCorner.Position + (SkirtingStart2EndDir * FMath::Cos(StartSectionRelAngle) / FMath::Sin(StartSectionRelAngle) * MirrorSign + Start2EndYAxis * MirrorSign) * SkirtingMeshWidth;
	FVector2D InnerEndPos = 
		SkirtingSegNode.SkirtingEndCorner.Position + (SkirtingStart2EndDir * FMath::Cos(EndSectionRelAngle) / FMath::Sin(EndSectionRelAngle) * MirrorSign + Start2EndYAxis * MirrorSign) * SkirtingMeshWidth;
	// do projection
	FVector2D InnerStartProj = FVector2D(FMath::ClosestPointOnInfiniteLine(FVector(StartPos, 0), FVector(EndPos, 0), FVector(InnerStartPos, 0)));
	FVector2D InnerEndProj = FVector2D(FMath::ClosestPointOnInfiniteLine(FVector(StartPos, 0), FVector(EndPos, 0), FVector(InnerEndPos, 0)));
	TArray<FVector2D> PointsOnSkirtingLine;
	PointsOnSkirtingLine.Add(InnerStartProj);
	PointsOnSkirtingLine.Add(InnerEndProj);
	PointsOnSkirtingLine.Add(StartPos);
	PointsOnSkirtingLine.Add(EndPos);
	PointsOnSkirtingLine.Sort([SkirtingStart2EndDir, StartPos](const FVector2D &Pnt1, const FVector2D &Pnt2)
	{
		FVector2D Start2Pnt1 = Pnt1 - StartPos;
		FVector2D Start2Pnt2 = Pnt2 - StartPos;

		float Start2Pnt1Dist = Start2Pnt1 | SkirtingStart2EndDir;
		float Start2Pnt2Dist = Start2Pnt2 | SkirtingStart2EndDir;

		return Start2Pnt1Dist < Start2Pnt2Dist;
	});
	// get full skirting start and end position
	FVector2D FullSkirtingStartPos = PointsOnSkirtingLine[0];
	FVector2D FullSkirtingEndPos = PointsOnSkirtingLine.Top();
	FVector LocalX = FVector(FullSkirtingEndPos - FullSkirtingStartPos, 0);
	LocalX.Normalize();
	FVector LocalZ = FVector::UpVector;
	FVector LocalY = FVector::CrossProduct(LocalZ, LocalX);
	LocalY.Normalize();
	LocalY *= MirrorSign;
	// find owner area height
	
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	auto WallSystem = GameInstance->WallBuildSystem;
	FVector SkirtingZ(0, 0, 0);
	if (SkirtingSegNode.SkirtingType == ESkirtingType::SkirtingTopLine)
	{
		auto OwnerRoom = WallSystem->GetSegmentOwnerRoom(SkirtingSegNode.SkirtingStartCorner.Position,
			SkirtingSegNode.SkirtingEndCorner.Position);
		float RoomCeilingHeight = DEF_CEILING_HEIGHT;
		if (OwnerRoom)
		{
			RoomCeilingHeight = OwnerRoom->GetRoomCeilingHeight();
		}
		SkirtingZ.Z = RoomCeilingHeight - SkirtingMeshHeight - GameInstance->GetSkirtingSystem()->GetSkirtingTopLineDefHeight();
	}
	
	FTransform  SkirtingTransform;
	if (InnerStaticMeshType)
	{
		SkirtingTransform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, SkirtingZ - BoundingMesh.Min)*
			FMatrix(LocalX, LocalY, LocalZ, FVector(FullSkirtingStartPos, 0)));
	}
	else if (InnerMeshType)
	{
		SkirtingTransform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, SkirtingZ - BoundingMesh.Min)*
			FMatrix(LocalX, LocalY, LocalZ, FVector(FullSkirtingStartPos, 0)));
	}

	FVector StartPnt(StartPos, 0);
	FVector EndPnt(EndPos, 0);
	FVector Start2EndNormal = FVector(SkirtingStart2EndDir, 0);
	FVector StartClippedPlaneDir = SkirtingSegNode.StartClippedPlaneDir;
	FVector EndClippedPlaneDir = SkirtingSegNode.EndClippedPlaneDir;
	
	// collect openings on skirting segment
	TArray<int32> TempHoleIDs;
	TArray<FFClosedInterval> OpeningAttrs;
	if (SkirtingSegNode.SkirtingType == ESkirtingType::SkirtingLine)
	{
		TArray<int32> OutWalls;
		UWorld *MyWorld = GetWorld();
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
		UBuildingSystem *BuildingSystem = WallSystem->BuildingSystem;
		if (WallSystem&&BuildingSystem)
		{
			WallSystem->SearchWallsBySegPoints(OutWalls, StartPos, EndPos);
			for (int32 WallIndex = 0; WallIndex < OutWalls.Num(); WallIndex++)
			{
				UBuildingData *WallData = BuildingSystem->GetData(OutWalls[WallIndex]);
				if (WallData)
				{
					ISuite *ISu = WallData->GetSuite();
					if (ISu)
					{
						IValue &HoleInfos = ISu->GetProperty(OutWalls[WallIndex], "Holes");
						int32 NumHole = HoleInfos.GetArrayCount();
						for (int32 HoleIndex = 0; HoleIndex < NumHole; HoleIndex++)
						{
							IValue & Hole = HoleInfos.GetField(HoleIndex);
							IValue &HoleID = Hole.GetField("HoleID");
							UBuildingData *HoleData = BuildingSystem->GetData(HoleID.IntValue());
							if (HoleData)
							{
								float HoleHeight = HoleData->GetFloat("ZPos");
								float HoleWidth = HoleData->GetFloat("Width");
								if (HoleHeight <= 1.0e-3)
								{
									TempHoleIDs.Add(HoleID.IntValue());
									FVector2D OpeningLocation = FVector2D(HoleData->GetVector("Location"));
									float OpeningRelLoc = (OpeningLocation - StartPos) | SkirtingStart2EndDir;
									OpeningAttrs.Add(FFClosedInterval(OpeningRelLoc - 0.5*HoleWidth, OpeningRelLoc + 0.5*HoleWidth));
								}
							}
						}
					}
					
				}
			}
		}
	}

	TArray<FFClosedInterval> CombinedOpenings;
	ULinearEntityLibrary::ClosedIntervalSetCombine(CombinedOpenings, OpeningAttrs);
	TArray<FFClosedInterval> SkirtingIntervals;
	float SkirtingTotalLen = FVector2D::Distance(StartPos, EndPos);
	FFClosedInterval TotalInterval(0, SkirtingTotalLen);
	ULinearEntityLibrary::IntervalSubtract(SkirtingIntervals, TotalInterval, CombinedOpenings);
	float FullSkirtingLength = FVector2D::Distance(FullSkirtingStartPos, FullSkirtingEndPos);
	SkirtingSegNode.ClearIntervalCache();
	int32 SkirtingIntervalCnt = SkirtingIntervals.Num();

	if (SkirtingIntervalCnt > 0)
	{
		for (int32 i = 0; i < SkirtingIntervalCnt; ++i)
		{
			auto &CurSkirtingInterval = SkirtingIntervals[i];
			FVector IntervalStart = StartPnt + Start2EndNormal * CurSkirtingInterval.StartPnt;
			FVector IntervalEnd = StartPnt + Start2EndNormal * CurSkirtingInterval.EndPnt;

			float IntervalSkirtingLength = FVector::Distance(IntervalStart, IntervalEnd);
			FVector IntervalStartPlaneDir, IntervalEndPlaneDir;
			FTransform LocalTrans = SkirtingTransform;
			if (FMath::IsNearlyEqual(CurSkirtingInterval.StartPnt, 0.0f))
			{
				IntervalStartPlaneDir = StartClippedPlaneDir;
				IntervalSkirtingLength += FVector2D::Distance(FullSkirtingStartPos, StartPos);
			}
			else
			{
				LocalTrans.AddToTranslation(Start2EndNormal*FVector2D::Distance(FVector2D(IntervalStart), FullSkirtingStartPos));
				IntervalStartPlaneDir = -Start2EndNormal;
			}

			if (FMath::IsNearlyEqual(CurSkirtingInterval.EndPnt, SkirtingTotalLen))
			{
				IntervalEndPlaneDir = EndClippedPlaneDir;
				IntervalSkirtingLength += FVector2D::Distance(FullSkirtingEndPos, EndPos);
			}
			else
			{
				IntervalEndPlaneDir = Start2EndNormal;
			}
			SkirtingSegNode.AddInterval(FVector2D(IntervalStart), FVector2D(IntervalEnd));
			if (InnerStaticMeshType)
			{
				BuildOneSkirtingSegByMX(IntervalStart, IntervalEnd, IntervalStartPlaneDir, IntervalEndPlaneDir,
					LocalTrans, InnerStaticMeshType, IntervalSkirtingLength,
					SkirtingMeshLength, Start2EndNormal);
			}
			else if (InnerMeshType)
			{
				BuildOneSkirtingSeg(IntervalStart, IntervalEnd, IntervalStartPlaneDir, IntervalEndPlaneDir,
					LocalTrans, InnerMeshType, IntervalSkirtingLength,
					SkirtingMeshLength, Start2EndNormal);
			}
		}
		ChangeMatByID(SkirtingSegNode.MaterialID);
	}
}

void USkirtingSegComponent::UpdateMeshByComponentNode(bool HasComponent)
{
	DoClearWork();
	const float UnExpectLength = 200000;
	float SkirtingSegLength =
		FVector2D::Distance(SkirtingSegNode.SkirtingStartCorner.Position,
			SkirtingSegNode.SkirtingEndCorner.Position);
	if (SkirtingSegLength >= UnExpectLength)
	{
		DoClearWork();
		return;
	}
	FVector2D StartPos = SkirtingSegNode.SkirtingStartCorner.Position;
	FVector2D EndPos = SkirtingSegNode.SkirtingEndCorner.Position;

	FBox BoundingMesh;
	if (InnerStaticMeshType)
	{
		BoundingMesh = InnerStaticMeshType->LocalBounds.TransformBy(FTransform(FRotator(0, 180.0f, 0),
			InnerStaticMeshType->Offset, InnerStaticMeshType->Scale3D));
	}
	else if (InnerMeshType)
	{
		BoundingMesh = InnerMeshType->GetBoundingBox();
	}

	FVector MeshSize = BoundingMesh.GetSize();
	float SkirtingMeshLength = MeshSize.X;
	float SkirtingMeshWidth = MeshSize.Y;
	float SkirtingMeshHeight = MeshSize.Z;

	// calculate inner skirting point position
	FVector2D SkirtingStart2EndDir = EndPos - StartPos;
	SkirtingStart2EndDir.Normalize();
	FVector2D Start2EndYAxis = SkirtingStart2EndDir.GetRotated(90);
	float RelativeAngle = ULinearEntityLibrary::GetRotatedOfVector2D(SkirtingStart2EndDir);
	float StartSectionRelAngle = SkirtingSegNode.StartSectionAngle;
	float EndSectionRelAngle = SkirtingSegNode.EndSectionAngle;
	float StartSecSinValue = FMath::Abs(FMath::Sin(StartSectionRelAngle));
	float EndSecSinValue = FMath::Abs(FMath::Sin(EndSectionRelAngle));
	if (StartSecSinValue < SmallPositiveNumber || EndSecSinValue < SmallPositiveNumber)
	{
		return;
	}
	bool bMirror = (SkirtingSegNode.HeadingDir == ECgHeadingDirection::ELeftTurn);
	// x direction should multiply by mirror sign
	int32 MirrorSign = -1;
	if (HasComponent)
	{
		MirrorSign = -1;
	}
	else
	{
		MirrorSign = bMirror ? -1: 1;
	}

	//(FMath::Sin(EndSectionRelAngle) > 0 ? 1 : -1);
	FVector2D InnerStartPos =
		SkirtingSegNode.SkirtingStartCorner.Position + (SkirtingStart2EndDir * FMath::Cos(StartSectionRelAngle) / FMath::Sin(StartSectionRelAngle) * MirrorSign + Start2EndYAxis * MirrorSign) * SkirtingMeshWidth;
	FVector2D InnerEndPos =
		SkirtingSegNode.SkirtingEndCorner.Position + (SkirtingStart2EndDir * FMath::Cos(EndSectionRelAngle) / FMath::Sin(EndSectionRelAngle) * MirrorSign + Start2EndYAxis * MirrorSign) * SkirtingMeshWidth;
	// do projection
	FVector2D InnerStartProj = FVector2D(FMath::ClosestPointOnInfiniteLine(FVector(StartPos, 0), FVector(EndPos, 0), FVector(InnerStartPos, 0)));
	FVector2D InnerEndProj = FVector2D(FMath::ClosestPointOnInfiniteLine(FVector(StartPos, 0), FVector(EndPos, 0), FVector(InnerEndPos, 0)));
	TArray<FVector2D> PointsOnSkirtingLine;
	PointsOnSkirtingLine.Add(InnerStartProj);
	PointsOnSkirtingLine.Add(InnerEndProj);
	PointsOnSkirtingLine.Add(StartPos);
	PointsOnSkirtingLine.Add(EndPos);
	PointsOnSkirtingLine.Sort([SkirtingStart2EndDir, StartPos](const FVector2D &Pnt1, const FVector2D &Pnt2)
	{
		FVector2D Start2Pnt1 = Pnt1 - StartPos;
		FVector2D Start2Pnt2 = Pnt2 - StartPos;

		float Start2Pnt1Dist = Start2Pnt1 | SkirtingStart2EndDir;
		float Start2Pnt2Dist = Start2Pnt2 | SkirtingStart2EndDir;

		return Start2Pnt1Dist < Start2Pnt2Dist;
	});
	// get full skirting start and end position
	FVector2D FullSkirtingStartPos = PointsOnSkirtingLine[0];
	FVector2D FullSkirtingEndPos = PointsOnSkirtingLine.Top();
	FVector LocalX = FVector(FullSkirtingEndPos - FullSkirtingStartPos, 0);
	LocalX.Normalize();
	FVector LocalZ = FVector::UpVector;
	FVector LocalY = FVector::CrossProduct(LocalZ, LocalX);
	LocalY.Normalize();
	LocalY *= MirrorSign;
	// find owner area height

	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	auto WallSystem = GameInstance->WallBuildSystem;
	FVector SkirtingZ(0, 0, 0);
	if (SkirtingSegNode.SkirtingType == ESkirtingType::SkirtingTopLine)
	{
		auto OwnerRoom = WallSystem->GetSegmentOwnerRoom(SkirtingSegNode.SkirtingStartCorner.Position,
			SkirtingSegNode.SkirtingEndCorner.Position);
		float RoomCeilingHeight = DEF_CEILING_HEIGHT;
		if (OwnerRoom)
		{
			RoomCeilingHeight = OwnerRoom->GetRoomCeilingHeight();
		}
		SkirtingZ.Z = RoomCeilingHeight - SkirtingMeshHeight - GameInstance->GetSkirtingSystem()->GetSkirtingTopLineDefHeight();
	}

	FTransform  SkirtingTransform;
	if (InnerStaticMeshType)
	{
		SkirtingTransform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, SkirtingZ - BoundingMesh.Min)*
			FMatrix(LocalX, LocalY, LocalZ, FVector(FullSkirtingStartPos, 0)));
	}
	else if (InnerMeshType)
	{
		SkirtingTransform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, SkirtingZ - BoundingMesh.Min)*
			FMatrix(LocalX, LocalY, LocalZ, FVector(FullSkirtingStartPos, 0)));
	}

	FVector StartPnt(StartPos, 0);
	FVector EndPnt(EndPos, 0);
	FVector Start2EndNormal = FVector(SkirtingStart2EndDir, 0);
	FVector StartClippedPlaneDir = SkirtingSegNode.StartClippedPlaneDir;
	FVector EndClippedPlaneDir = SkirtingSegNode.EndClippedPlaneDir;

	// collect openings on skirting segment
	TArray<int32> TempHoleIDs;
	TArray<FFClosedInterval> OpeningAttrs;
	if (SkirtingSegNode.SkirtingType == ESkirtingType::SkirtingLine)
	{
		TArray<int32> OutWalls;
		UWorld *MyWorld = GetWorld();
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
		UBuildingSystem *BuildingSystem = WallSystem? WallSystem->BuildingSystem : nullptr;
		if (WallSystem&&BuildingSystem)
		{
			WallSystem->SearchWallsBySegPoints(OutWalls, StartPos, EndPos);
			for (int32 WallIndex = 0; WallIndex < OutWalls.Num(); WallIndex++)
			{
				UBuildingData *WallData = BuildingSystem->GetData(OutWalls[WallIndex]);
				if (WallData)
				{
					ISuite *ISu = WallData->GetSuite();
					if (ISu)
					{
						IValue &HoleInfos = ISu->GetProperty(OutWalls[WallIndex], "Holes");
						int32 NumHole = HoleInfos.GetArrayCount();
						for (int32 HoleIndex = 0; HoleIndex < NumHole; HoleIndex++)
						{
							IValue & Hole = HoleInfos.GetField(HoleIndex);
							IValue &HoleID = Hole.GetField("HoleID");
							UBuildingData *HoleData = BuildingSystem->GetData(HoleID.IntValue());
							if (HoleData)
							{
								float HoleHeight = HoleData->GetFloat("ZPos");
								float HoleWidth = HoleData->GetFloat("Width");
								if (HoleHeight <= 1.0e-3)
								{
									TempHoleIDs.Add(HoleID.IntValue());
									FVector2D OpeningLocation = FVector2D(HoleData->GetVector("Location"));
									float OpeningRelLoc = (OpeningLocation - StartPos) | SkirtingStart2EndDir;
									OpeningAttrs.Add(FFClosedInterval(OpeningRelLoc - 0.5*HoleWidth, OpeningRelLoc + 0.5*HoleWidth));
								}
							}
						}
					}

				}
			}
		}
	}

	TArray<FFClosedInterval> CombinedOpenings;
	ULinearEntityLibrary::ClosedIntervalSetCombine(CombinedOpenings, OpeningAttrs);
	TArray<FFClosedInterval> SkirtingIntervals;
	float SkirtingTotalLen = FVector2D::Distance(StartPos, EndPos);
	FFClosedInterval TotalInterval(0, SkirtingTotalLen);
	ULinearEntityLibrary::IntervalSubtract(SkirtingIntervals, TotalInterval, CombinedOpenings);
	float FullSkirtingLength = FVector2D::Distance(FullSkirtingStartPos, FullSkirtingEndPos);
	SkirtingSegNode.ClearIntervalCache();
	int32 SkirtingIntervalCnt = SkirtingIntervals.Num();

	if (SkirtingIntervalCnt > 0)
	{
		for (int32 i = 0; i < SkirtingIntervalCnt; ++i)
		{
			auto &CurSkirtingInterval = SkirtingIntervals[i];
			FVector IntervalStart = StartPnt + Start2EndNormal * CurSkirtingInterval.StartPnt;
			FVector IntervalEnd = StartPnt + Start2EndNormal * CurSkirtingInterval.EndPnt;

			float IntervalSkirtingLength = FVector::Distance(IntervalStart, IntervalEnd);
			FVector IntervalStartPlaneDir, IntervalEndPlaneDir;
			FTransform LocalTrans = SkirtingTransform;
			if (FMath::IsNearlyEqual(CurSkirtingInterval.StartPnt, 0.0f))
			{
				IntervalStartPlaneDir = StartClippedPlaneDir;
				IntervalSkirtingLength += FVector2D::Distance(FullSkirtingStartPos, StartPos);
			}
			else
			{
				LocalTrans.AddToTranslation(Start2EndNormal*FVector2D::Distance(FVector2D(IntervalStart), FullSkirtingStartPos));
				IntervalStartPlaneDir = -Start2EndNormal;
			}

			if (FMath::IsNearlyEqual(CurSkirtingInterval.EndPnt, SkirtingTotalLen))
			{
				IntervalEndPlaneDir = EndClippedPlaneDir;
				IntervalSkirtingLength += FVector2D::Distance(FullSkirtingEndPos, EndPos);
			}
			else
			{
				IntervalEndPlaneDir = Start2EndNormal;
			}
			SkirtingSegNode.AddInterval(FVector2D(IntervalStart), FVector2D(IntervalEnd));
			if (InnerStaticMeshType)
			{
				BuildOneSkirtingSegByMX(IntervalStart, IntervalEnd, IntervalStartPlaneDir, IntervalEndPlaneDir,
					LocalTrans, InnerStaticMeshType, IntervalSkirtingLength,
					SkirtingMeshLength, Start2EndNormal);
			}
			else if (InnerMeshType)
			{
				BuildOneSkirtingSeg(IntervalStart, IntervalEnd, IntervalStartPlaneDir, IntervalEndPlaneDir,
					LocalTrans, InnerMeshType, IntervalSkirtingLength,
					SkirtingMeshLength, Start2EndNormal);
			}
		}
		ChangeMatByID(SkirtingSegNode.MaterialID);
	}
}

void USkirtingSegComponent::BuildOneSkirtingSeg(const FVector &StartPnt, const FVector &EndPnt,
	const FVector &StartClippedPlaneDir, const FVector &EndClippedPlaneDir,
	FTransform &SkirtingTransform, UStaticMesh *InMeshType,
	float FullSkirtingLength, float SkirtingMeshLength, const FVector &Start2EndNormal)
{
	if (FullSkirtingLength < SkirtingMeshLength)
	{
		AddProceduralMesh(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
	}
	else
	{
		int32 SkirtingSegCnt = (int32)(FullSkirtingLength / SkirtingMeshLength) + 1;
		//if (SkirtingSegCnt <= 50)
		//{
			// start skirting segment
			AddProceduralMesh(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
			SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);

			// middle skirting segment
			int32 MiddleSkirtingSegCnt = SkirtingSegCnt - 2;

			//judge last second segment to be clipped according to distance bound
			const float DisBound = 10;
			for (int32 i = 0; i < MiddleSkirtingSegCnt; ++i)
			{
				FVector TempStartP = StartPnt + Start2EndNormal * SkirtingMeshLength*(i + 1);
				FVector TempEndP = StartPnt + Start2EndNormal * SkirtingMeshLength*(i + 2);
				float Dis = sqrt(FVector::Distance(TempEndP, EndPnt));
				if (Dis <= DisBound)
				{
					AddProceduralMesh(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
				}
				else
				{
					AddStaticMesh(InMeshType, SkirtingTransform);
				}
				SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);
			}

			// end skirting segment
			AddProceduralMesh(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
			SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);
		//}
	}
}

void USkirtingSegComponent::ResetMesh(UModelFile *SkirtingType)
{
	InnerStaticMeshType = SkirtingType;
}

void USkirtingSegComponent::ResetStaticMesh(UStaticMesh *StaticMeshType)
{
	InnerMeshType = StaticMeshType;
}

void USkirtingSegComponent::ChangeMaterial(UMaterialInterface *NewMaterial)
{
	if (!NewMaterial)
	{
		return;
	}
	for (auto &ProceduralMesh : ProceduralMeshs)
	{
		ProceduralMesh->SetMaterial(0, NewMaterial);
	}
	for (auto &MiddleC : StaticMeshComps)
	{
		MiddleC->SetMaterial(0, NewMaterial);
	}
	for (auto &Model : StaticModelFileComps)
	{
		Model->SetMaterialByChannel(0, NewMaterial);
	}
}

void USkirtingSegComponent::ChangeMatByID(const FString &ResMatID)
{
	if (ResMatID.IsEmpty())
	{
		return;
	}
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (ResMgr)
	{
		UModelFile *ResModel = Cast<UModelFile>(ResMgr->FindRes(ResMatID));
		if (ResModel)
		{
			ResModel->ForceLoad();
			auto Material = ResModel->GetUE4Material(0);
			ChangeMaterial(Material);
		}
	}
}

FString USkirtingSegComponent::GetMatID()
{
	return SkirtingSegNode.MaterialID;
}

void USkirtingSegComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoClearWork();

	Super::EndPlay(EndPlayReason);
}

void USkirtingSegComponent::AddProceduralMesh(UStaticMesh *InMesh, const FTransform &InTransform, 
	const FVector &StartPos, const FVector &StartClippedPlaneDir,
	const FVector &EndPos, const FVector &EndClippedPlaneDir)
{
	FVector SkirtingStart2EndDir = EndPos - StartPos;
	SkirtingStart2EndDir.Normalize();
	UStaticMeshInfoExtract *StaticMeshInfoExtract = NewObject<UStaticMeshInfoExtract>(this, TEXT("MeshInfoExtract"));
	if (StaticMeshInfoExtract != nullptr)
	{
		StaticMeshInfoExtract->ResetStaticMesh(InMesh);

		TArray<FVector> Positions;
		USolidGeometryFunctionLibrary::TransformPnts(Positions, StaticMeshInfoExtract->Positions, InTransform);

		USolidGeometryFunctionLibrary::ClipPntsByPlaneAndDirection(Positions, Positions,
			StartPos, StartClippedPlaneDir, SkirtingStart2EndDir);
		USolidGeometryFunctionLibrary::ClipPntsByPlaneAndDirection(Positions, Positions,
			EndPos, EndClippedPlaneDir, SkirtingStart2EndDir);

		AddProceduralMeshByStaticMeshInfo(StaticMeshInfoExtract, Positions, InTransform);
	}
	
}

void USkirtingSegComponent::AddProceduralMeshByStaticMeshInfo(UStaticMeshInfoExtract *InStaticMeshInfo, const TArray<FVector> &PntsOfStaticMesh, const FTransform &Transform)
{
	if (!InStaticMeshInfo)
	{
		return;
	}

	UProceduralMeshComponent *ProceduralMeshComp = NewObject<UProceduralMeshComponent>(this);
	if (ProceduralMeshComp != nullptr)
	{
		ProceduralMeshComp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		ProceduralMeshs.Add(ProceduralMeshComp);

		int32 NumVert = InStaticMeshInfo->Positions.Num();
		TArray<FVector>	Positions;
		Positions.SetNum(NumVert);
		for (int32 i = 0; i < NumVert; ++i)
		{
			Positions[i] = Transform.InverseTransformPosition(PntsOfStaticMesh[i]);
		}

		ProceduralMeshComp->CreateMeshSection_LinearColor(0, Positions,
			InStaticMeshInfo->Indices, InStaticMeshInfo->Normals, InStaticMeshInfo->UVs, InStaticMeshInfo->Colors, InStaticMeshInfo->Tangents, true);
		ProceduralMeshComp->SetWorldTransform(Transform);
		ProceduralMeshComp->RegisterComponentWithWorld(GetWorld());
		ProceduralMeshComp->SetCastShadow(false);

		if (InStaticMeshInfo->MxMesh)
		{
			if (InStaticMeshInfo->MxMesh->GetNumberOfMaterial() > 0)
			{
				UMaterialInterface *Material = InStaticMeshInfo->MxMesh->GetUE4Material(0);
				if (Material)
				{
					ProceduralMeshComp->SetMaterial(0, Material);
				}
			}
		}
		else if (InStaticMeshInfo->Mesh)
		{
			UMaterialInterface *Material = InStaticMeshInfo->Mesh->GetMaterial(0);
			if (Material)
			{
				ProceduralMeshComp->SetMaterial(0, Material);
			}
		}
	}
	
}

void USkirtingSegComponent::AddStaticMesh(UStaticMesh *InMesh, const FTransform &InTransform)
{
	if (!InMesh)
	{
		return;
	}

	UStaticMeshComponent *MeshComponent = NewObject<UStaticMeshComponent>(this);
	MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	if (!MeshComponent)
		return;

	MeshComponent->SetStaticMesh(InMesh);
	MeshComponent->SetWorldTransform(InTransform);
	MeshComponent->RegisterComponentWithWorld(GetWorld());
	MeshComponent->SetCastShadow(false);
	StaticMeshComps.Add(MeshComponent);
}


void USkirtingSegComponent::BuildOneSkirtingSegByMX(const FVector &StartPnt, const FVector &EndPnt, 
	const FVector &StartClippedPlaneDir, const FVector &EndClippedPlaneDir, 
	FTransform &SkirtingTransform, UModelFile *InMeshType, 
	float FullSkirtingLength, float SkirtingMeshLength, const FVector &Start2EndNormal)
{
	if (FullSkirtingLength < SkirtingMeshLength)
	{
		AddProceduralMeshByMX(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
	}
	else
	{
		int32 SkirtingSegCnt = (int32)(FullSkirtingLength / SkirtingMeshLength) + 1;
		// start skirting segment
		//if (SkirtingSegCnt <= 50)
		//{
			AddProceduralMeshByMX(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
			SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);

			// middle skirting segment
			int32 MiddleSkirtingSegCnt = SkirtingSegCnt - 2;

			//judge last second segment to be clipped according to distance bound
			const float DisBound = 10;
			for (int32 i = 0; i < MiddleSkirtingSegCnt; ++i)
			{
				FVector TempStartP = StartPnt + Start2EndNormal * SkirtingMeshLength*(i + 1);
				FVector TempEndP = StartPnt + Start2EndNormal * SkirtingMeshLength*(i + 2);
				float Dis = sqrt(FVector::Distance(TempEndP, EndPnt));
				if (Dis <= DisBound)
				{
					AddProceduralMeshByMX(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
				}
				else
				{
					AddStaticMeshByMX(InMeshType, SkirtingTransform);
				}
				SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);
			}

			// end skirting segment
			AddProceduralMeshByMX(InMeshType, SkirtingTransform, StartPnt, StartClippedPlaneDir, EndPnt, EndClippedPlaneDir);
			SkirtingTransform.AddToTranslation(Start2EndNormal*SkirtingMeshLength);
		//}
		
	}
}

void USkirtingSegComponent::AddProceduralMeshByMX(UModelFile *InMesh, const FTransform &InTransform, const FVector &StartPos, const FVector &StartClippedPlaneDir, const FVector &EndPos, const FVector &EndClippedPlaneDir)
{
	FVector SkirtingStart2EndDir = EndPos - StartPos;
	SkirtingStart2EndDir.Normalize();
	UStaticMeshInfoExtract *StaticMeshInfoExtract = NewObject<UStaticMeshInfoExtract>(this, TEXT("MeshInfoExtract"));
	StaticMeshInfoExtract->ResetMXMesh(InMesh);

	FTransform Transform(FTransform(FRotator(0, 180.0f, 0),
		InMesh->Offset, InMesh->Scale3D));
	FTransform NewTransform = Transform * InTransform;
	TArray<FVector> Positions;
	USolidGeometryFunctionLibrary::TransformPnts(Positions, StaticMeshInfoExtract->Positions, NewTransform);

	USolidGeometryFunctionLibrary::ClipPntsByPlaneAndDirection(Positions, Positions,
		StartPos, StartClippedPlaneDir, SkirtingStart2EndDir);
	USolidGeometryFunctionLibrary::ClipPntsByPlaneAndDirection(Positions, Positions,
		EndPos, EndClippedPlaneDir, SkirtingStart2EndDir);

	AddProceduralMeshByStaticMeshInfo(StaticMeshInfoExtract, Positions, NewTransform);
}

void USkirtingSegComponent::AddStaticMeshByMX(UModelFile *InMesh, const FTransform &InTransform)
{
	if (!InMesh)
	{
		return;
	}

	UModelFileComponent *MeshComponent = NewObject<UModelFileComponent>(this);
	MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	if (!MeshComponent)
		return;

	MeshComponent->UpdateModel(InMesh);
	
	MeshComponent->SetWorldTransform(InTransform);
	MeshComponent->RegisterComponentWithWorld(GetWorld());
	MeshComponent->SetCastShadow(false);
	MeshComponent->SetAffectByShadow(false);
	StaticModelFileComps.Add(MeshComponent);
}

void USkirtingSegComponent::DoClearWork()
{
	for (int32 i = 0; i < StaticMeshComps.Num(); ++i)
	{
		UStaticMeshComponent *Mesh = StaticMeshComps[i];
		if (Mesh)
		{
			Mesh->DestroyComponent(true);
		}
	}

	for (int32 i = 0; i < StaticModelFileComps.Num(); ++i)
	{
		UModelFileComponent *Mesh = StaticModelFileComps[i];
		if (Mesh)
		{
			Mesh->DestroyComponent(true);
		}
	}

	for (int32 i = 0; i < ProceduralMeshs.Num(); ++i)
	{
		UProceduralMeshComponent *Mesh = ProceduralMeshs[i];
		if (Mesh)
		{
			Mesh->DestroyComponent(true);
		}
	}

	StaticModelFileComps.Empty();
	StaticMeshComps.Empty();
	ProceduralMeshs.Empty();
}

UMontageMeshComponent::UMontageMeshComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	,bDrawDebug(1)
	,bDrawMesh(1)
{
}

void UMontageMeshComponent::AddPolgonMesh(const TArray<FVector> &PolygonLine, TSubclassOf<UWallLine> WallLineType, float InnerOffset, bool bFloor /*= true*/, bool bCW /*= true*/, bool bClosed/*= false*/)
{
	FVector SegmentNormal = bFloor ? FVector::UpVector : -FVector::UpVector;

	if (WallLineType != NULL)
	{
		UWallLine *WallLine = WallLineType->GetDefaultObject<UWallLine>();
		if (bDrawDebug && PolygonLine.Num() > 0)
		{
			for (int i = 0; i < PolygonLine.Num(); ++i)
			{
				DrawDebugString(GetWorld(), PolygonLine[i], FString::Printf(TEXT("%d"), i));
				DrawDebugLine(GetWorld(), PolygonLine[i], PolygonLine[(i + 1) % PolygonLine.Num()], FColor::White, true);
			}
		}

		TArray<FVector> ShrinkPolygon;
		RemoveAuxPoint(ShrinkPolygon, PolygonLine);

		if (!bCW)
		{
			int iStart = 0;
			int iEnd = ShrinkPolygon.Num() - 1;
			while (iStart<iEnd)
			{
				Swap<FVector>(ShrinkPolygon[iStart], ShrinkPolygon[iEnd]);
				iStart++;
				iEnd--;
			}
		}

		TArray<FVector> SortedPolygons;
		SortPolygonLine(SortedPolygons, ShrinkPolygon, InnerOffset, true, bClosed);

		if (bDrawDebug)
		{
			for (int i = 0; i < SortedPolygons.Num(); ++i)
			{
				DrawDebugLine(GetWorld(), SortedPolygons[i], SortedPolygons[(i + 1) % SortedPolygons.Num()], FColor::White, true);
			}
		}

		if (ShrinkPolygon.Num() > 2)
		{
			int32 iStart = 0;

			FVector Dir0 = (SortedPolygons[1] - SortedPolygons[0]).GetSafeNormal();
			FVector Dir1 = (SortedPolygons[2] - SortedPolygons[1]).GetSafeNormal();

			FVector Normal = FVector::CrossProduct(Dir0, Dir1);
			if (Normal.IsZero())
			{
				Normal = FVector(0, 0, 1.0f);
			}

			FVector LastDir = Dir1;

			int32 i = 3;
			for (; i < SortedPolygons.Num(); ++i)
			{
				Dir1 = (SortedPolygons[i] - SortedPolygons[i - 1]).GetSafeNormal();
				FVector CurNormal = FVector::CrossProduct(LastDir, Dir1);
				LastDir = Dir1;

				if (CurNormal.IsZero())
				{
					continue;
				}

				float fDot = FVector::DotProduct(CurNormal, Normal);
				if (fDot < 0)
				{
					if (bDrawDebug)
					{
						DrawDebugSphere(GetWorld(), SortedPolygons[i - 1], 10.0f, 20, FColor::Green, true);
					}
					int32 num = i - iStart;
					DoAddPolgonMesh(SegmentNormal, SortedPolygons, iStart, num, WallLine, bCW, bClosed);
					iStart = i - 1;
					Normal = CurNormal;
					Dir0 = SortedPolygons[iStart] - SortedPolygons[iStart - 1];
				}
			}

			if (i > iStart)
			{
				int32 num = i - iStart;
				DoAddPolgonMesh(SegmentNormal, SortedPolygons, iStart, num, WallLine, bCW, bClosed);
			}
		}
		else
		{
			DoAddPolgonMesh(SegmentNormal, SortedPolygons, 0, SortedPolygons.Num(), WallLine, bCW, bClosed);
		}
	}
}

void UMontageMeshComponent::DoAddPolgonMesh(const FVector &SegmentNormal, const TArray<FVector> &PolygonLine, int32 iStart, int32 Num, UWallLine *WallLine, bool bCW, bool bClosed/*= false*/)
{
	if (bDrawDebug)
	{
		for (int i = iStart+1; i < iStart + Num; ++i)
		{
			DrawDebugLine(GetWorld(), PolygonLine[i - 1] + FVector(0,0,-1.0f), PolygonLine[i] + FVector(0, 0, -1.0f), FColor::Red, true);
		}
	}

	bClosed = bClosed && PolygonLine.Num() > 2;

	FVector StartDir, StartPlaneDir;
	StartDir = (PolygonLine[iStart + 1] - PolygonLine[iStart]).GetSafeNormal();
	
	if (iStart>1)
	{
		FVector LastDir = (PolygonLine[iStart] - PolygonLine[iStart - 1]).GetSafeNormal();
		StartPlaneDir = -GetSplitPlane(LastDir, StartDir, SegmentNormal);
	}
	else
	{
		if (bClosed)
		{
			int nPolyVtx = PolygonLine.Num();
			FVector PreLastDir = (PolygonLine[nPolyVtx-1] - PolygonLine[nPolyVtx-2]).GetSafeNormal();
			FVector LastDir = (PolygonLine[0] - PolygonLine[nPolyVtx - 1]).GetSafeNormal();
			FVector CurDir = (PolygonLine[1] - PolygonLine[0]).GetSafeNormal();
			StartPlaneDir = GetSplitPlane(LastDir, CurDir, SegmentNormal);
		}
		else
		{
			StartPlaneDir = StartDir;
		}
	}

	FVector EndPlaneDir, NextStartPlaneDir;
	FVector EndDir;

	float MeshLength, Radius;
	GetMeshLength(WallLine, MeshLength, Radius);

	for (int i = 0; i < Num - 1; ++i)
	{
		int32 index = i + iStart;

		if (index < PolygonLine.Num() - 2)
		{
			FVector Start = PolygonLine[index + 1];
			FVector End = PolygonLine[index + 2];
			EndDir = (End - Start).GetSafeNormal();
			EndPlaneDir = GetSplitPlane(StartDir, EndDir, SegmentNormal);
		}
		else
		{
			if (bClosed)
			{
				FVector Start = PolygonLine[index + 1];
				FVector End = PolygonLine[0];
				EndDir = (End - Start).GetSafeNormal();
				EndPlaneDir = GetSplitPlane(StartDir, EndDir, SegmentNormal);
			}
			else
			{
				EndPlaneDir = StartDir;
			}
		}
		float BackOffset = ((i+iStart > 0 )||bClosed)? Radius : 0;
		CreateSegment(StartPlaneDir, EndPlaneDir, SegmentNormal, PolygonLine[index], PolygonLine[index + 1], WallLine, MeshLength, BackOffset, Radius, bCW, false);
		StartPlaneDir = -EndPlaneDir;
		StartDir = EndDir;
	}

	if (bClosed && ((iStart+Num)==PolygonLine.Num()) && PolygonLine.Num() > 1)
	{
		FVector Start = PolygonLine.Last();
		FVector End = PolygonLine[0];
		EndDir = (PolygonLine[1] - PolygonLine[0]).GetSafeNormal();
		EndPlaneDir = GetSplitPlane(StartDir, EndDir, SegmentNormal);
		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), Start, 10.0f, 20, FColor::Red, true);
		}
		CreateSegment(StartPlaneDir, EndPlaneDir, SegmentNormal, Start, End, WallLine, MeshLength, Radius, Radius, bCW, false);
	}
}

FVector UMontageMeshComponent::GetSplitPlane(const FVector &StartDir, const FVector &EndDir, const FVector &PlaneDir)
{
	FVector HalfDir = (EndDir - StartDir) * 0.5f;
	FVector EndPlaneDir = FVector::CrossProduct(HalfDir, PlaneDir).GetSafeNormal();
	return  EndPlaneDir;
}

void UMontageMeshComponent::RemoveAuxPoint(TArray<FVector> &NewPoints, const TArray<FVector> &PolygonLine)
{
	int32 Num = PolygonLine.Num();
	TArray<int32> RmPoints;

	if (Num > 2)
	{
		FVector LastForward = (PolygonLine[1] - PolygonLine[0]).GetSafeNormal();
		for (int32 i = 1; i < PolygonLine.Num()-1; ++i)
		{
			FVector Forward = (PolygonLine[i + 1] - PolygonLine[i]).GetSafeNormal();
			FVector Up = FVector::CrossProduct(LastForward, Forward);
			if ( FMath::Abs(Up.Z)  < 0.01f)
			{
				RmPoints.Add(i);
			}
			LastForward = Forward;
		}
	}

	NewPoints = PolygonLine;
	for (int32 i = RmPoints.Num() - 1; i >= 0; --i)
	{
		int32 index = RmPoints[i];
		NewPoints.RemoveAt(index);
	}
}

void UMontageMeshComponent::GetCCW(TArray<uint8> &bCCW, const TArray<FVector> &PolygonLine)
{
	int32 Num = PolygonLine.Num();
	bCCW.AddZeroed(Num);

	FVector Dir0 = (PolygonLine[1] - PolygonLine[0]).GetSafeNormal();
	FVector Dir1 = (PolygonLine[2] - PolygonLine[1]).GetSafeNormal();

	FVector Normal = FVector::CrossProduct(Dir0, Dir1);
	if (Normal.IsZero())
	{
		Normal = FVector(0, 0, 1.0f);
	}

	int32 iStart = 0;
	FVector LastDir = Dir1;

	int32 i = 3;
	for (; i < PolygonLine.Num(); ++i)
	{
		Dir1 = (PolygonLine[i] - PolygonLine[i - 1]).GetSafeNormal();
		FVector CurNormal = FVector::CrossProduct(LastDir, Dir1);
		LastDir = Dir1;

		if (CurNormal.IsZero())
		{
			continue;
		}

		float fDot = FVector::DotProduct(CurNormal, Normal);
		if (fDot < 0)
		{
			bCCW[i] = 1;
			int32 num = i - iStart;
			iStart = i - 1;
			Normal = CurNormal;
			Dir0 = PolygonLine[iStart] - PolygonLine[iStart - 1];
		}
	}
}

void UMontageMeshComponent::SortPolygonLine(TArray<FVector> &SortedPolyonLine, const TArray<FVector> &PolygonLine, float InnerOffset, bool bCW, bool bClose)
{
	int32 Num = PolygonLine.Num();

	if (InnerOffset > 0 && Num>1)
	{
		SortedPolyonLine.SetNum(Num);
		FVector LastForward = (PolygonLine[0] - PolygonLine.Last()).GetSafeNormal();

		FVector StartDir, EndDir;
		float	StartLen = 0, EndLen = 0;
		
		if (!bClose && Num>1)
		{
			(PolygonLine[1] - PolygonLine[0]).ToDirectionAndLength(StartDir, StartLen);
			(PolygonLine[Num - 1] - PolygonLine[Num - 2]).ToDirectionAndLength(EndDir, EndLen);
		}

		for (int32 i = 0; i < Num; ++i)
		{
			FVector OffsetDir;
			FVector Forward = (PolygonLine[(i + 1) % Num] - PolygonLine[i]).GetSafeNormal();

			if(bClose || (!bClose && i!=0 && i!=Num-1))
			{
				float UpZ = FVector::CrossProduct(Forward, LastForward).Z;
				if (UpZ == 0)
				{
					OffsetDir = FVector::CrossProduct(FVector::UpVector, Forward);
				}
				else
				{
					FVector HalfForward = ((Forward - LastForward) * 0.5f).GetSafeNormal();
					FVector Up = FVector::CrossProduct(LastForward, Forward);
					if ((bCW && Up.Z < 0) || (!bCW && Up.Z>0))
					{
						HalfForward = -HalfForward;
					}
					OffsetDir = HalfForward;
				}

				FVector Offset = OffsetDir * InnerOffset;
				SortedPolyonLine[i] = PolygonLine[i] + Offset;
				if (bDrawDebug)
				{
					DrawDebugLine(GetWorld(), PolygonLine[i], SortedPolyonLine[i], FColor::White, true);
				}
			}

			LastForward = Forward;
		}

		if (!bClose && Num>1)
		{
			FPlane StartPlane(PolygonLine[0], StartDir);
			SortedPolyonLine[0] = FPlane::PointPlaneProject(SortedPolyonLine[1], StartPlane);

			FPlane EndPlane(PolygonLine[Num-1], EndDir);
			SortedPolyonLine[Num - 1] = FPlane::PointPlaneProject(SortedPolyonLine[Num - 2], EndPlane);
		}
	}
	else
	{
		SortedPolyonLine = PolygonLine;
	}
}


void UMontageMeshComponent::GetMeshLength(UWallLine *WallLine, float &Length, float &Radius)
{
	Length = 0;
	Radius = 0;

	if (WallLine && WallLine->Mesh != nullptr && WallLine->Mesh->RenderData.IsValid())
	{
		for (int32 i = 0; i < WallLine->Positions.Num(); ++i)
		{
			FVector Position = WallLine->Positions[i];
			if (Position.X > Length)
			{
				Length = Position.X;
			}
		}
		FVector Ext = WallLine->Mesh->ExtendedBounds.BoxExtent;
		Radius = FMath::Max<float>(Ext.Y, Ext.Z);
	}
}

void UMontageMeshComponent::CreateSegment(const FVector &StartPlaneDir, const FVector &EndPlaneDir, const FVector &PolygonPlaneNormal, const FVector &StartPos, const FVector &EndPos, UWallLine *WallLine, float MeshLength, float BackOffset, float ForwardOffset, bool bCW, bool bFlipEnd)
{
	FVector		Forward = (EndPos - StartPos).GetSafeNormal();

	FVector AdjStartPlaneDir = StartPlaneDir;
	FVector AdjEndPlaneDir = EndPlaneDir;
	
	if ((StartPlaneDir | Forward) > 0)
	{
		AdjStartPlaneDir = -StartPlaneDir;
	}

	if ((EndPlaneDir | Forward) < 0)
	{
		AdjEndPlaneDir = -EndPlaneDir;
	}

	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(),  StartPos, EndPos, FColor::White, true);
		DrawDebugPoint(GetWorld(), StartPos, 10.0f, FColor::Blue, true);
		DrawDebugPoint(GetWorld(), EndPos, 10.0f, FColor::Blue, true);
		DrawDebugSolidPlane(GetWorld(), FPlane(StartPos, AdjStartPlaneDir), StartPos, FVector2D(10.0f,10.0f), FColor::Green, true);
		DrawDebugSolidPlane(GetWorld(), FPlane(EndPos, AdjEndPlaneDir), EndPos, FVector2D(10.0f, 10.0f), FColor::White, true);
	}

	FVector		NewStartPos = StartPos - Forward*BackOffset;
	FVector		NewEndPos = EndPos + Forward*ForwardOffset;  
	FVector		UpNormal = PolygonPlaneNormal;
	FVector		Right = FVector::CrossProduct(UpNormal, Forward);
	FTransform  BaseTransform = FTransform(FMatrix(Forward, Right, UpNormal, NewStartPos));

	float Length = (NewEndPos - NewStartPos).Size();
	if (Length > MeshLength)
	{
		TArray<FVector> Positions;

		//Begin Cap
		if (FMath::Abs(Forward | AdjStartPlaneDir) > 0.98f)
		{
			AddStaticMesh(BaseTransform, WallLine->Mesh);
		}
		else
		{
			GetMeshPositions(BaseTransform, Positions, WallLine);
			PlaneClipVertex(Positions, StartPos, AdjStartPlaneDir, Forward); 
			PlaneClipVertex(Positions, EndPos, AdjEndPlaneDir, Forward);
			AddProceduralMesh(BaseTransform, Positions, WallLine);
		}
		
		//Center
		Length -= MeshLength;
		BaseTransform.AddToTranslation(Forward*MeshLength);

		float ExLength = MeshLength + BackOffset;
		while (Length>ExLength)
		{
			AddStaticMesh(BaseTransform, WallLine->Mesh);
			Length -= MeshLength;
			BaseTransform.AddToTranslation(Forward*MeshLength);
		}

		//EndCap
		GetMeshPositions(BaseTransform, Positions, WallLine);
		PlaneClipVertex(Positions, StartPos, AdjStartPlaneDir, Forward);
		PlaneClipVertex(Positions, EndPos, AdjEndPlaneDir, Forward);
		AddProceduralMesh(BaseTransform, Positions, WallLine);
	}
	else
	{
		TArray<FVector> Positions;
		GetMeshPositions(BaseTransform, Positions, WallLine);

		PlaneClipVertex(Positions, StartPos, AdjStartPlaneDir, Forward);
		PlaneClipVertex(Positions, EndPos, AdjEndPlaneDir, Forward);

		AddProceduralMesh(BaseTransform, Positions, WallLine);
	}
}

void UMontageMeshComponent::GetMeshPositions(const FTransform & Transform, TArray<FVector> &WordPositions, UWallLine *WallLine)
{
	if (WallLine!=NULL)
	{
		int32 NumVerts = WallLine->Positions.Num();
		WordPositions.SetNum(NumVerts);

		for (int32 i = 0; i < NumVerts; ++i)
		{
			FVector LocVert = WallLine->Positions[i];
			WordPositions[i] = Transform.TransformPosition(LocVert);
		}
	}
}

void UMontageMeshComponent::PlaneClipVertex(TArray<FVector> &WordPositions, const FVector &BasePos, const FVector &Normal, const FVector &Forward)
{
	FPlane Plane(BasePos, Normal);

	for (int i = 0; i < WordPositions.Num(); ++i)
	{
		FVector &Position = WordPositions[i];
		float D = Plane.PlaneDot(Position);
		if (D > 0)
		{
			float fDot = D / (Forward | Normal);
			FVector AdjLoc = Position - Forward*fDot;
			WordPositions[i] = AdjLoc;
		}
	}
}

void UMontageMeshComponent::AddProceduralMesh(const FTransform &Tranform, TArray<FVector> &WordPositions, UWallLine *WallLine)
{
	if (bDrawMesh && WallLine->Mesh && WallLine->Mesh->RenderData.IsValid())
	{
		UProceduralMeshComponent *ProceduralMeshComp = NewObject<UProceduralMeshComponent>(GetOwner());
		ProceduralMeshComponents.Add(ProceduralMeshComp);

		int32 NumVert = WallLine->Positions.Num();

		TArray<FVector>	Positions;
		Positions.SetNum(NumVert);

		FQuat Rotation = Tranform.GetRotation();
		FTransform InvertTransform = Tranform.Inverse();

		for (int32 i = 0; i < NumVert; ++i)
		{
			Positions[i] = Tranform.InverseTransformPosition(WordPositions[i]);
		}
		
		ProceduralMeshComp->CreateMeshSection_LinearColor(0, Positions, WallLine->Indices, WallLine->Normals, WallLine->UVs, WallLine->Colors, WallLine->Tangents, true);
		ProceduralMeshComp->SetWorldTransform(Tranform);
		ProceduralMeshComp->RegisterComponentWithWorld(GetWorld());
		ProceduralMeshComp->SetCastShadow(false);
		
		UMaterialInterface *Material = WallLine->Mesh->GetMaterial(0);
		if (Material)
		{
			ProceduralMeshComp->SetMaterial(0, Material);
		}
	}
}

void UMontageMeshComponent::AddStaticMesh(const FTransform &Transform, UStaticMesh *Mesh)
{
	if (bDrawMesh && Mesh)
	{
		UStaticMeshComponent *MeshComponent = NewObject<UStaticMeshComponent>(GetOwner());
		if (MeshComponent != NULL)
		{
			MeshComponent->SetStaticMesh(Mesh);
			MeshComponent->SetWorldTransform(Transform);
			MeshComponent->RegisterComponentWithWorld(GetWorld());
			StaticMeshComponents.Add(MeshComponent);
		}
	}
}


void UMontageMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (int i = 0; i < StaticMeshComponents.Num(); ++i)
	{
		UStaticMeshComponent *Mesh = StaticMeshComponents[i];
		if (Mesh)
		{
			Mesh->UnregisterComponent();
		}
	}

	for (int i = 0; i < ProceduralMeshComponents.Num(); ++i)
	{
		UProceduralMeshComponent *ProcMesh = ProceduralMeshComponents[i];
		if (ProcMesh)
		{
			ProcMesh->UnregisterComponent();
		}
	}

	StaticMeshComponents.Empty();
	ProceduralMeshComponents.Empty();

	Super::EndPlay(EndPlayReason);
}



USkirtingMeshComponent::USkirtingMeshComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer), bHasASeg(false)
{
}

FSkirtingNode& USkirtingMeshComponent::GetSkirtingSaveNode()
{
	return SkirtingDataNode;
}

void USkirtingMeshComponent::UpdateMeshByNode()
{
	DoClearWork();
	if (0 == SkirtingDataNode.SkirtingSegments.Num())
	{
		return;
	}

	bHasASeg = true;

	// closed skirting segments orientation
	bool bClosedSkirting = SkirtingDataNode.IsClosedSkirting();
	if (bClosedSkirting)
	{
		// judge polygon orientation
		TArray<FVector2D> PolyPnts;
		for (auto &SkirtingSeg : SkirtingDataNode.SkirtingSegments)
		{
			PolyPnts.Add(SkirtingSeg.SkirtingStartCorner.Position);
		}
		SkirtingDataNode.AutoHeadingDirByClosedPath(PolyPnts);
	}

	FBox BoundingMesh;
	if (InnerStaticMeshType)
	{
		BoundingMesh = InnerStaticMeshType->LocalBounds;
	}
	else if (InnerMeshType)
	{
		BoundingMesh = InnerMeshType->GetBoundingBox();
	}
	
	FVector MeshSize = BoundingMesh.GetSize();
	float SkirtingMeshLength = MeshSize.X;
	float SkirtingMeshWidth = MeshSize.Y;
	float SkirtingMeshHeight = MeshSize.Z;

	SkirtingDataNode.CopyAttrToSegNode();
	FVector2D LastSkirtingSegDir;
	if (bClosedSkirting)
	{
		FSkirtingSegNode &NextSegData = SkirtingDataNode.SkirtingSegments[0];
		FVector2D NextSegStart2End = NextSegData.SkirtingEndCorner.Position - NextSegData.SkirtingStartCorner.Position;
		NextSegStart2End.Normalize();
		// left turn counter clockwise, right turn clockwise
		FSkirtingSegNode &LastSegData = SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1];
		FVector2D LastSegDir = LastSegData.SkirtingEndCorner.Position - LastSegData.SkirtingStartCorner.Position;
		FVector2D Start2EndReverse= -LastSegDir;
		float NormalRadian =
			ULinearEntityLibrary::UnwindRadian2Normal((ULinearEntityLibrary::GetRotatedOfVector2D(NextSegStart2End) -
				ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndReverse)));
		float RotateAngle = FMath::RadiansToDegrees(0.5f * NormalRadian);
		LastSkirtingSegDir =
			Start2EndReverse.GetRotated(RotateAngle);
	}
	else
	{
		LastSkirtingSegDir 
			= SkirtingDataNode.SkirtingSegments[0].SkirtingEndCorner.Position - SkirtingDataNode.SkirtingSegments[0].SkirtingStartCorner.Position;
		LastSkirtingSegDir = -LastSkirtingSegDir.GetRotated(HALF_CIRCLE_ANGLE);
	}
	
	LastSkirtingSegDir.Normalize();
	FVector2D LastSkirtingSegNormal = -LastSkirtingSegDir;
	switch (SkirtingDataNode.HeadingDir)
	{
	case ECgHeadingDirection::ELeftTurn:
	{
		
	}
	break;
	case ECgHeadingDirection::ERightTurn:
	{
		LastSkirtingSegDir *= -1;
	}
	break;
	default:
		break;
	}

	FVector SkirtingLocalZ = FVector::UpVector;
	//if (ESkirtingType::SkirtingTopLine == SkirtingDataNode.SkirtingType)
	//{
	//	//SkirtingLocalZ *= -1;
	//}
	int32 SkirtingSegCnt = SkirtingDataNode.SkirtingSegments.Num();

	if (SkirtingSegCnt > 0)
	{
		for (int32 SegIndex = 0; SegIndex < SkirtingSegCnt; ++SegIndex)
		{
			FSkirtingSegNode &SkirtingSegmentData = SkirtingDataNode.SkirtingSegments[SegIndex];
			USkirtingSegComponent *SkirtingSegment = NewObject<USkirtingSegComponent>(GetOwner());
			if (SkirtingSegment != nullptr)
			{
				SkirtingSegment->RegisterComponentWithWorld(GetWorld());
				SkirtingSegComponents.Add(SkirtingSegment);
				SkirtingSegment->ResetMesh(InnerStaticMeshType);
				SkirtingSegment->ResetStaticMesh(InnerMeshType);
				FSkirtingSegNode &SkirtingSegNode = SkirtingSegment->GetRelatedNode();
				SkirtingSegNode = SkirtingSegmentData;
				FVector2D Start2EndSkirting = SkirtingSegNode.SkirtingEndCorner.Position - SkirtingSegNode.SkirtingStartCorner.Position;
				Start2EndSkirting.Normalize();
				float Start2EndAngle = ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndSkirting);

				// calculate angle info and clipped plane normal
				SkirtingSegNode.StartSectionAngle = ULinearEntityLibrary::GetRotatedOfVector2D(LastSkirtingSegDir) - Start2EndAngle;
				SkirtingSegNode.StartClippedPlaneDir = FVector::CrossProduct(SkirtingLocalZ, FVector(LastSkirtingSegNormal, 0));
				int32 NextSegIndex = (SegIndex + 1) % SkirtingSegCnt;
				if (NextSegIndex != 0 || bClosedSkirting)
				{
					FSkirtingSegNode &NextSegData = SkirtingDataNode.SkirtingSegments[NextSegIndex];
					FVector2D NextSegStart2End = NextSegData.SkirtingEndCorner.Position - NextSegData.SkirtingStartCorner.Position;
					NextSegStart2End.Normalize();
					// left turn counter clockwise, right turn clockwise
					FVector2D Start2EndReverse = -Start2EndSkirting;
					float NormalRadian =
						ULinearEntityLibrary::UnwindRadian2Normal((ULinearEntityLibrary::GetRotatedOfVector2D(NextSegStart2End) -
							ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndReverse)));
					float RotateAngle = FMath::RadiansToDegrees(0.5f * NormalRadian);
					LastSkirtingSegDir =
						Start2EndReverse.GetRotated(RotateAngle);
					//LastSkirtingSegDir = NextSegStart2End - Start2EndSkirting;
					LastSkirtingSegDir.Normalize();
					LastSkirtingSegNormal = -LastSkirtingSegDir;
					switch (SkirtingDataNode.HeadingDir)
					{
					case ECgHeadingDirection::ELeftTurn:
					{

					}
					break;
					case ECgHeadingDirection::ERightTurn:
					{
						LastSkirtingSegDir *= -1.0f;
					}
					break;
					default:
						break;
					}

					SkirtingSegNode.EndSectionAngle = ULinearEntityLibrary::GetRotatedOfVector2D(LastSkirtingSegDir) - Start2EndAngle;
					SkirtingSegNode.EndClippedPlaneDir = -FVector::CrossProduct(SkirtingLocalZ, FVector(LastSkirtingSegNormal, 0));
				}
				else
				{
					switch (SkirtingDataNode.HeadingDir)
					{
					case ECgHeadingDirection::ELeftTurn:
					{
						SkirtingSegNode.EndSectionAngle = -HALF_CIRCLE_DEG;
					}
					break;
					case ECgHeadingDirection::ERightTurn:
					{
						SkirtingSegNode.EndSectionAngle = HALF_CIRCLE_DEG;
					}
					break;
					default:
						break;
					}
					SkirtingSegNode.EndClippedPlaneDir = FVector(Start2EndSkirting, 0);
				}

				SkirtingSegment->UpdateMeshByNode();
			}
		}	
	}
	else
	{
		return;
	}
	
}


void USkirtingMeshComponent::UpdateMeshByNodes(bool IsComponent)
{
	DoClearWork();
	if (0 == SkirtingDataNode.SkirtingSegments.Num())
	{
		return;
	}

	bHasASeg = true;

	// closed skirting segments orientation
	bool bClosedSkirting = SkirtingDataNode.IsClosedSkirting();
	if (bClosedSkirting)
	{
		// judge polygon orientation
		TArray<FVector2D> PolyPnts;
		for (auto &SkirtingSeg : SkirtingDataNode.SkirtingSegments)
		{
			PolyPnts.Add(SkirtingSeg.SkirtingStartCorner.Position);
		}
		SkirtingDataNode.AutoHeadingDirByClosedPath(PolyPnts);
	}

	FBox BoundingMesh;
	if (InnerStaticMeshType)
	{
		BoundingMesh = InnerStaticMeshType->LocalBounds;
	}
	else if (InnerMeshType)
	{
		BoundingMesh = InnerMeshType->GetBoundingBox();
	}

	FVector MeshSize = BoundingMesh.GetSize();
	float SkirtingMeshLength = MeshSize.X;
	float SkirtingMeshWidth = MeshSize.Y;
	float SkirtingMeshHeight = MeshSize.Z;

	SkirtingDataNode.CopyAttrToSegNode();
	FVector2D LastSkirtingSegDir;
	if (bClosedSkirting)
	{
		FSkirtingSegNode &NextSegData = SkirtingDataNode.SkirtingSegments[0];
		FVector2D NextSegStart2End = NextSegData.SkirtingEndCorner.Position - NextSegData.SkirtingStartCorner.Position;
		NextSegStart2End.Normalize();
		// left turn counter clockwise, right turn clockwise
		FSkirtingSegNode &LastSegData = SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1];
		FVector2D LastSegDir = LastSegData.SkirtingEndCorner.Position - LastSegData.SkirtingStartCorner.Position;
		FVector2D Start2EndReverse = -LastSegDir;
		float NormalRadian =
			ULinearEntityLibrary::UnwindRadian2Normal((ULinearEntityLibrary::GetRotatedOfVector2D(NextSegStart2End) -
				ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndReverse)));
		float RotateAngle = FMath::RadiansToDegrees(0.5f * NormalRadian);
		LastSkirtingSegDir =
			Start2EndReverse.GetRotated(RotateAngle);
	}
	else
	{
		LastSkirtingSegDir
			= SkirtingDataNode.SkirtingSegments[0].SkirtingEndCorner.Position - SkirtingDataNode.SkirtingSegments[0].SkirtingStartCorner.Position;
		LastSkirtingSegDir = -LastSkirtingSegDir.GetRotated(HALF_CIRCLE_ANGLE);
	}

	LastSkirtingSegDir.Normalize();
	FVector2D LastSkirtingSegNormal = -LastSkirtingSegDir;
	switch (SkirtingDataNode.HeadingDir)
	{
	case ECgHeadingDirection::ELeftTurn:
	{

	}
	break;
	case ECgHeadingDirection::ERightTurn:
	{
		LastSkirtingSegDir *= -1;
	}
	break;
	default:
		break;
	}

	FVector SkirtingLocalZ = FVector::UpVector;
	//if (ESkirtingType::SkirtingTopLine == SkirtingDataNode.SkirtingType)
	//{
	//	//SkirtingLocalZ *= -1;
	//}
	int32 SkirtingSegCnt = SkirtingDataNode.SkirtingSegments.Num();

	if (SkirtingSegCnt > 0)
	{
		for (int32 SegIndex = 0; SegIndex < SkirtingSegCnt; ++SegIndex)
		{
			FSkirtingSegNode &SkirtingSegmentData = SkirtingDataNode.SkirtingSegments[SegIndex];
			USkirtingSegComponent *SkirtingSegment = NewObject<USkirtingSegComponent>(GetOwner());
			if (SkirtingSegment != nullptr)
			{
				SkirtingSegment->RegisterComponentWithWorld(GetWorld());
				SkirtingSegComponents.Add(SkirtingSegment);
				SkirtingSegment->ResetMesh(InnerStaticMeshType);
				SkirtingSegment->ResetStaticMesh(InnerMeshType);
				FSkirtingSegNode &SkirtingSegNode = SkirtingSegment->GetRelatedNode();
				SkirtingSegNode = SkirtingSegmentData;
				FVector2D Start2EndSkirting = SkirtingSegNode.SkirtingEndCorner.Position - SkirtingSegNode.SkirtingStartCorner.Position;
				Start2EndSkirting.Normalize();
				float Start2EndAngle = ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndSkirting);

				// calculate angle info and clipped plane normal
				SkirtingSegNode.StartSectionAngle = ULinearEntityLibrary::GetRotatedOfVector2D(LastSkirtingSegDir) - Start2EndAngle;
				SkirtingSegNode.StartClippedPlaneDir = FVector::CrossProduct(SkirtingLocalZ, FVector(LastSkirtingSegNormal, 0));
				int32 NextSegIndex = (SegIndex + 1) % SkirtingSegCnt;
				if (NextSegIndex != 0 || bClosedSkirting)
				{
					FSkirtingSegNode &NextSegData = SkirtingDataNode.SkirtingSegments[NextSegIndex];
					FVector2D NextSegStart2End = NextSegData.SkirtingEndCorner.Position - NextSegData.SkirtingStartCorner.Position;
					NextSegStart2End.Normalize();
					// left turn counter clockwise, right turn clockwise
					FVector2D Start2EndReverse = -Start2EndSkirting;
					float NormalRadian =
						ULinearEntityLibrary::UnwindRadian2Normal((ULinearEntityLibrary::GetRotatedOfVector2D(NextSegStart2End) -
							ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndReverse)));
					float RotateAngle = FMath::RadiansToDegrees(0.5f * NormalRadian);
					LastSkirtingSegDir =
						Start2EndReverse.GetRotated(RotateAngle);
					//LastSkirtingSegDir = NextSegStart2End - Start2EndSkirting;
					LastSkirtingSegDir.Normalize();
					LastSkirtingSegNormal = -LastSkirtingSegDir;
					switch (SkirtingDataNode.HeadingDir)
					{
					case ECgHeadingDirection::ELeftTurn:
					{

					}
					break;
					case ECgHeadingDirection::ERightTurn:
					{
						LastSkirtingSegDir *= -1.0f;
					}
					break;
					default:
						break;
					}

					SkirtingSegNode.EndSectionAngle = ULinearEntityLibrary::GetRotatedOfVector2D(LastSkirtingSegDir) - Start2EndAngle;
					SkirtingSegNode.EndClippedPlaneDir = -FVector::CrossProduct(SkirtingLocalZ, FVector(LastSkirtingSegNormal, 0));
				}
				else
				{
					switch (SkirtingDataNode.HeadingDir)
					{
					case ECgHeadingDirection::ELeftTurn:
					{
						SkirtingSegNode.EndSectionAngle = -HALF_CIRCLE_DEG;
					}
					break;
					case ECgHeadingDirection::ERightTurn:
					{
						SkirtingSegNode.EndSectionAngle = HALF_CIRCLE_DEG;
					}
					break;
					default:
						break;
					}
					SkirtingSegNode.EndClippedPlaneDir = FVector(Start2EndSkirting, 0);
				}

				SkirtingSegment->UpdateMeshByComponentNode(IsComponent);
			}
		}
	}
	else
	{
		return;
	}

}

void USkirtingMeshComponent::BuildFromPnts(const TArray<FVector> &InPolylinePnts, ECgHeadingDirection InHeadingDir, bool bInClosed,
	UModelFile *InDefaultType)
{

}

void USkirtingMeshComponent::SetOrientation(ECgHeadingDirection InHeadingDir)
{
	SkirtingDataNode.HeadingDir = InHeadingDir;

	UpdateMeshByNode();
}

bool USkirtingMeshComponent::AddHeadPnt(const FVector &HeadPnt, ECgHeadingDirection InHeadingDir)
{
	bool bDoAdd = false;
	if (SkirtingDataNode.SkirtingSegments.Num() != 0)
	{
		auto &NextSeg = SkirtingDataNode.SkirtingSegments[0];
		auto NextDir = NextSeg.SkirtingEndCorner.Position - NextSeg.SkirtingStartCorner.Position;
		auto CurrentDir = FVector2D(HeadPnt) - NextSeg.SkirtingStartCorner.Position;
		NextDir.Normalize();
		CurrentDir.Normalize();
		if (!CurrentDir.Equals(NextDir, SmallPositiveNumber) && !CurrentDir.IsNearlyZero(SmallPositiveNumber))
		{
			FSkirtingSegNode SegNode;
			SegNode.SkirtingStartCorner.Position = FVector2D(HeadPnt);
			SegNode.SkirtingEndCorner = NextSeg.SkirtingStartCorner;
			SkirtingDataNode.SkirtingSegments.Insert(SegNode, 0);

			UpdateMeshByNode();
			bDoAdd = true;
		}
	}
	
	return bDoAdd;
}

bool USkirtingMeshComponent::AddTailPnt(const FVector &TailPnt, ECgHeadingDirection InHeadingDir)
{
	bool bDoAdd = false;
	if (bHasASeg)
	{
		if (SkirtingDataNode.SkirtingSegments.Num() != 0)
		{
			// judge if we can do adding
			auto& LastSeg = SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1];
			auto LastDir = LastSeg.SkirtingStartCorner.Position - LastSeg.SkirtingEndCorner.Position;
			LastDir.Normalize();
			auto CurrentDir = FVector2D(TailPnt) - LastSeg.SkirtingEndCorner.Position;
			CurrentDir.Normalize();
			if (!LastDir.Equals(CurrentDir, SmallPositiveNumber) && !CurrentDir.IsNearlyZero(SmallPositiveNumber))
			{
				FSkirtingSegNode SegNode;
				SegNode.SkirtingStartCorner = LastSeg.SkirtingEndCorner;
				SegNode.SkirtingEndCorner.Position = FVector2D(TailPnt);
				SkirtingDataNode.SkirtingSegments.Add(SegNode);
				bDoAdd = true;
			}
		}
	}
	else
	{
		bDoAdd = true;
		if (SkirtingDataNode.SkirtingSegments.Num() == 0)
		{
			FSkirtingSegNode SegNode;
			SegNode.SkirtingStartCorner.Position = FVector2D(TailPnt);
			SkirtingDataNode.SkirtingSegments.Add(SegNode);
		}
		else
		{
			FSkirtingSegNode& SkirtingSeg = SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1];
			FVector2D CurrentDir = FVector2D(TailPnt) - SkirtingSeg.SkirtingStartCorner.Position;
			if (!CurrentDir.IsNearlyZero(SmallPositiveNumber))
			{
				SkirtingSeg.SkirtingEndCorner.Position = FVector2D(TailPnt);
				bHasASeg = true;
			}
		}
	}
	
	if (bHasASeg && bDoAdd)
	{
		UpdateMeshByNode();
	}
	
	return bDoAdd;
}

void USkirtingMeshComponent::SetClosed(bool bInClose)
{
	if (SkirtingDataNode.SkirtingSegments.Num() > 1)
	{
		FSkirtingSegNode SegNode;
		SegNode.SkirtingStartCorner = SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1].SkirtingEndCorner;
		SegNode.SkirtingEndCorner = SkirtingDataNode.SkirtingSegments[0].SkirtingStartCorner;
		FVector2D CurrentDir = SegNode.SkirtingEndCorner.Position - SegNode.SkirtingStartCorner.Position;

		FSkirtingSegNode &NextSeg = SkirtingDataNode.SkirtingSegments[0];
		FVector2D NextDir = NextSeg.SkirtingEndCorner.Position - NextSeg.SkirtingStartCorner.Position;
		NextDir.Normalize();
		CurrentDir.Normalize();
		// check if we should close
		if (!CurrentDir.Equals(NextDir, SmallPositiveNumber) && !CurrentDir.IsNearlyZero(SmallPositiveNumber))
		{
			SkirtingDataNode.SkirtingSegments.Add(SegNode);
			SkirtingDataNode.bClosed = bInClose;

			UpdateMeshByNode();
		}
	}
}

void USkirtingMeshComponent::MovePntByIndex(const FVector2D &NewPosition, int32 PntIndex)
{
	if (PntIndex == -1)
	{
		PntIndex = SkirtingDataNode.SkirtingSegments.Num();
	}

	if (PntIndex < 0 || PntIndex > SkirtingDataNode.SkirtingSegments.Num())
	{
		return;
	}

	// find relate corner and change its position
	if (0 != PntIndex)
	{
		SkirtingDataNode.SkirtingSegments[PntIndex - 1].SkirtingEndCorner.Position = NewPosition;
	}
	
	if (PntIndex != SkirtingDataNode.SkirtingSegments.Num())
	{
		SkirtingDataNode.SkirtingSegments[PntIndex].SkirtingStartCorner.Position = NewPosition;
	}

	UpdateMeshByNode();
}

void USkirtingMeshComponent::ResetMeshType(UModelFile *InSkirtingType)
{
	InnerStaticMeshType = InSkirtingType;

	ClearMaterialOnSkirtingMesh();
}

void USkirtingMeshComponent::ResetStaticMeshType(UStaticMesh *InSkirtingType)
{
	InnerMeshType = InSkirtingType;

	ClearMaterialOnSkirtingMesh();
}

void USkirtingMeshComponent::ChangeMaterial(UMaterialInterface *NewMaterial)
{
	for (auto &SkirtingSegComp : SkirtingSegComponents)
	{
		SkirtingSegComp->ChangeMaterial(NewMaterial);
	}
}

void USkirtingMeshComponent::ResetMeshByID(const FString &ResModelID)
{
	SkirtingDataNode.SkirtingMeshID = ResModelID;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (ResMgr)
	{
		UModelFile *ResModel = Cast<UModelFile>(ResMgr->FindRes(ResModelID));
		ResetMeshType(ResModel);
	}
}

void USkirtingMeshComponent::ChangeMatByID(const FString &ResMatID)
{
	SkirtingDataNode.MaterialID = ResMatID;
	for (auto &SkirtingSeg : SkirtingSegComponents)
	{
		SkirtingSeg->ChangeMatByID(ResMatID);
	}
}

void USkirtingMeshComponent::DeleteSkirtingSegByIndices(const TArray<int32> &SkirtingIndices2Del)
{

}

void USkirtingMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoClearWork();

	Super::EndPlay(EndPlayReason);
}

ESkirtingSnapType USkirtingMeshComponent::GetPntSnap(const FVector2D &SnapPnt, ESkirtingType InSkirtingType) const
{
	if (InSkirtingType != SkirtingDataNode.SkirtingType || SkirtingDataNode.IsClosedSkirting())
	{
		return ESkirtingSnapType::SnapNull;
	}
	ESkirtingSnapType SnapType = ESkirtingSnapType::SnapNull;
	if (SkirtingDataNode.SkirtingSegments.Num() > 0)
	{
		if (SkirtingDataNode.SkirtingSegments[0].SkirtingStartCorner.Position.Equals(SnapPnt, WALL_AREA_SNAP_TOL))
		{
			SnapType = ESkirtingSnapType::SnapHead;
		}
		else if (SkirtingDataNode.SkirtingSegments[SkirtingDataNode.SkirtingSegments.Num() - 1].SkirtingEndCorner.Position.Equals(SnapPnt, WALL_AREA_SNAP_TOL))
		{
			SnapType = ESkirtingSnapType::SnapTail;
		}
	}

	return SnapType;
}

bool USkirtingMeshComponent::DoSnapFirstPnt(const FVector2D &Pnt2Judge)
{
	auto SnapPnts = GetPathPnts();
	if (SnapPnts.Num() < 3)
	{
		return false;
	}

	return (SnapPnts[0].Equals(Pnt2Judge, WALL_AREA_SNAP_TOL));
}

TArray<FVector2D> USkirtingMeshComponent::GetPathPnts() const
{
	TArray<FVector2D> PntsOnPath;
	if (SkirtingDataNode.SkirtingSegments.Num() > 0)
	{
		PntsOnPath.Add(SkirtingDataNode.SkirtingSegments[0].SkirtingStartCorner.Position);
		for (auto &SkirtingSeg : SkirtingDataNode.SkirtingSegments)
		{
			PntsOnPath.Add(SkirtingSeg.SkirtingEndCorner.Position);
		}
	}
	
	return PntsOnPath;
}

void USkirtingMeshComponent::DoClearWork()
{
	for (auto &SkirtingSegComp : SkirtingSegComponents)
	{
		if (SkirtingSegComp)
		{
			SkirtingSegComp->DoClearWork();
			SkirtingSegComp->DestroyComponent();
		}
	}
	SkirtingSegComponents.Empty();
}

void USkirtingMeshComponent::ClearMaterialOnSkirtingMesh()
{
	SkirtingDataNode.MaterialID.Empty();
}
