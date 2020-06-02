
#include "DRStruct.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "HomeLayout/SceneEntity/LineWallActor.h"

FDRModelInstance::FDRModelInstance()
	:ModelID(-1)
	, BindID(-1)
	,isShadow(false)
	,ModelType(EDRModelType::EDRNull)
	,Location(FVector::ZeroVector)
	,Rotation(FRotator::ZeroRotator)
	,ResID(TEXT(""))
{
}

FDRComplexModel::FDRComplexModel()
	:ComplexType(EDRComplex::ENull)
{
}

FDRHoleStruct::FDRHoleStruct()
	:HoleID(-1)
	,Location(FVector::ZeroVector)
	,ModelType(EDRModelType::EDRNull)
	,Width(40.f)
	,Hight(40.f)
	,GroundHeight(30.f)
{
}
FDRWallStruct::FDRWallStruct()
	:OriginalDataID(-1)
	,bRoomWall(true)
	,IsSave(false)
{

}


FWallBindInfo::FWallBindInfo()
	:Wall(nullptr)
{

}

FWallSufaceInfo::FWallSufaceInfo()
{
	OwnerRoom = nullptr;
	OwnerWall = nullptr;
}

void FWallSufaceInfo::SetSurFaceType(int32 index)
{
	switch (index)
	{
	case 0:
		Type = EWallSufaceInfoType::EBottom;
		break;
	case 1:
		Type = EWallSufaceInfoType::ETop;
		break;
	case 2:
		Type = EWallSufaceInfoType::EFront;
		break;
	case 3:
		Type = EWallSufaceInfoType::EBack;
		break;
	case 4:
		Type = EWallSufaceInfoType::ELeft;
		break;
	case 5:
		Type = EWallSufaceInfoType::ERight;
		break;
	default:
		Type = EWallSufaceInfoType::ENull;
		break;
	}
}

void FWallSufaceInfo::UpdateSurfaceData()
{
	if (OwnerWall && OwnerWall && OwnerWall->WallComp3d)
	{
		float _Area = 0.f;
		FProcMeshSection* _Array = OwnerWall->WallComp3d->GetProcMeshSection(SurFaceIndex);
		if (_Array)
		{
			//
			SetSurFaceType(SurFaceIndex);
			//
			int _a, _b, _c;
			const int32 _NUM = _Array->ProcIndexBuffer.Num();
			for (int i = 0; i < _NUM; i += 3)
			{
				_a = _Array->ProcIndexBuffer[i];
				_b = _Array->ProcIndexBuffer[(i + 1) % _NUM];
				_c = _Array->ProcIndexBuffer[(i + 2) % _NUM];

				double _AB = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_b].Position).Size();
				double _BC = (_Array->ProcVertexBuffer[_b].Position - _Array->ProcVertexBuffer[_c].Position).Size();
				double _CA = (_Array->ProcVertexBuffer[_c].Position - _Array->ProcVertexBuffer[_a].Position).Size();
				double p = (_AB + _BC + _CA) / 2.0;
				_Area += FMath::Sqrt(p * (p - _AB) * (p - _BC) * (p - _CA));
			}
			Acreage = _Area;
			//Acreage /= 10000.f;
		}
	}
}

void FRoomBindInfo::UpdateSurfaceData()
{
	if (Room && Room->PGround)
	{
		float _Area = 0.f;
		FProcMeshSection* _Array = Room->PGround->GetProcMeshSection(0);
		if (_Array)
		{
			int _a, _b, _c;
			const int32 _NUM = _Array->ProcIndexBuffer.Num();
			for (int i = 0; i < _NUM; i += 3)
			{
				_a = _Array->ProcIndexBuffer[i];
				_b = _Array->ProcIndexBuffer[(i + 1) % _NUM];
				_c = _Array->ProcIndexBuffer[(i + 2) % _NUM];

				double _AB = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_b].Position).Size2D();
				double _BC = (_Array->ProcVertexBuffer[_b].Position - _Array->ProcVertexBuffer[_c].Position).Size2D();
				double _CA = (_Array->ProcVertexBuffer[_c].Position - _Array->ProcVertexBuffer[_a].Position).Size2D();
				double p = (_AB + _BC + _CA) / 2.0;
				_Area += FMath::Sqrt(p * (p - _AB) * (p - _BC) * (p - _CA));
			}
			Acreage = _Area;
			Acreage /= 10000.f;
		}
	}
}

FDRVirtualWallStruct::FDRVirtualWallStruct()
	: IsDelete(false)
	, Index(-1)
{

}

FTempHoleAndModel::FTempHoleAndModel()
	:HoleIndex(-1)
	, ModelIndex(-1)
{

}

FHouseInfoValue::FHouseInfoValue()
	:CommunityID(-1)
	, RegionLevel1ID(-1)
	, RegionLevel2ID(-1)
	, RegionLevel3ID(-1)
{

}
