// Copyright? 2017 ihomefnt All Rights Reserved.


#include "SkuComponentActor.h"
#include "HouseCustomActor.h"
#include "ResourceMeshComponent.h"
#include "BuildingGroupMeshComponent.h "
#include "ModelFileComponent.h"
#include "ResourceMgr.h"
#include "../Building/BuildingSystem.h"
#include "HouseFunctionLibrary.h"
#include "HouseFunction/ExpansionDrawing/DynamicTableboard.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASkuComponentActor::ASkuComponentActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Sku = CreateDefaultSubobject<UModelFileComponent>("SkuComponent");
	Sku->AttachTo(RootComponent);
	modelFile = nullptr;
	IsMove = false;
	Table = nullptr;
	myUUID = "";
}

// Called when the game starts or when spawned
void ASkuComponentActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkuComponentActor::SetDynamicTable(ADynamicTableboard* tab)
{
	Table = tab;
}

bool ASkuComponentActor::GetIsMove()
{
	return IsMove;
}

void ASkuComponentActor::InitSkuData(const FSkuData& data, bool bclone)
{
	modelid = data.modelid;
	skuid = data.skuid;
	CategoryId = data.Categoryid;
	Mx = data.MD5;
	if (bclone) {
	myUUID = data.UUID;
	}
	else
	{
		if (data.UUID.Equals("")) {
			SetUUID();
		}
		else
		{
			myUUID = data.UUID;
		}
		
	}
	isclone = bclone;
	CreateSku(Mx, CategoryId,ActorCategoryId);

}

void ASkuComponentActor::SetMove(bool bIs)
{
	IsMove = bIs;
}

ASkuComponentActor* ASkuComponentActor::Clone()
{
	ASkuComponentActor *SkuTmpe = GetWorld()->SpawnActor<ASkuComponentActor>();
	SkuTmpe->CreateSku(Mx, CategoryId, ActorCategoryId);
	SkuTmpe->SetActorTransform(GetActorTransform());
	SkuTmpe->CategoryId = CategoryId;
	SkuTmpe->modelid = modelid;
	SkuTmpe->skuid = skuid;
	SkuTmpe->Mx = Mx;
	SkuTmpe->SetUUID();
	SkuTmpe->isclone = true;
	return SkuTmpe;
}

bool ASkuComponentActor::GetMeshLocation(const FString & MeshName, FVector & Loaction)
{
	if (modelFile)
	{
		auto Modellist = modelFile->GetSubModels();
		for (auto i : Modellist)
		{
			if (i->ModelName == MeshName)
			{
				Loaction = i->Bounds.GetCenter() + GetActorLocation();
				return true;
			}
		}
	}


	return false;
}

bool ASkuComponentActor::GetMeshLocationEx(const FString& MD5, const FString & MeshName, FVector & Loaction)
{
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	const char *AnsiResID = TCHAR_TO_ANSI(*MD5);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false);
	if (!pObj)
		return false;
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	const char *meshN = TCHAR_TO_ANSI(*MeshName);
	IMeshObject* pMeshObj = pObj->GetMeshObject(meshN);
	if (!pMeshObj)
		return false;


	auto center = pMeshObj->GetBounds().GetCenter();

	
	Loaction.X = center.x;
	Loaction.Y = center.y;
	Loaction.Z = center.z;
	Loaction = UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), Loaction);
	Loaction.Y = -Loaction.Y;
	Loaction.X = -Loaction.X;
	Loaction += GetActorLocation();
	return true;
}

float ASkuComponentActor::GetWashbowlZ()
{
	float Z = 0.f;
	if (modelFile)
	{
		auto Modellist = modelFile->GetSubModels();
		for (auto i : Modellist)
		{
			if (i->ModelName == "aux_trim_1")
			{
				Z = i->Bounds.Max.Z;
				break;
			}
		}
	}
	//if (Z == 0.f)
	//	Z = GetActorLocation().Z;
	return Z;
}

// Called every frame
void ASkuComponentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkuComponentActor::CreateSku(FString Md5, int32 id,int32 actortypeid)
{
	UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();
	modelFile = Cast<UModelFile>(ResourceMgr->FindRes(Md5, true));
	Mx = Md5;
	ActorCategoryId = actortypeid;

	if (modelFile)
	{
		modelFile->ForceLoad();
		Sku->UpdateModel(modelFile);
		CategoryId = id;
	}
}

void ASkuComponentActor::SetUUID()
{
	/*if (!Mydata)
		return;*/
	int32 Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
	FString uuid = FString::Printf(TEXT("SkuComponentActor_%f"), Seconds + PartialSeconds);
	if (myUUID.Equals(""))
		myUUID = uuid;
	/*if (isclone)
	{
		if (myUUID.Equals(""))
		myUUID = uuid;
	}
	else
	{
		if (Mydata->UUID.Equals(""))
		{
			Mydata->UUID = uuid;
		}
		myUUID = Mydata->UUID;
	}*/
	
}


ADynamicTableboard* ASkuComponentActor::ChangeSku(FSkuData data)
{
	Mx = data.MD5;
	Sku->ChangeModel(data.MD5);
	modelid = data.modelid;
	CategoryId = data.Categoryid;
	skuid = data.skuid;
	if (Table)
	{
		auto Actor = GetWorld()->SpawnActor<ADynamicTableboard>();

		TArray<FVector2D> Holelist;
		TArray <TArray<FVector2D>> Holelists;
		GetHole(Holelist);
		Holelists.Add(Holelist);
		Actor->CreateMesh(Table->PointList, Holelists, 1.5f, Table->mMx, Table->Hight, Table->Ids, ActorCategoryId);
		Actor->SetFunctionManage(Table->manage);
		for(int i = 0; i < Table->Getbreakwaters().Num(); ++i)
			Actor->Addbreakwaters(Table->Getbreakwaters()[i]);
		Table->Destroy();
		Table = Actor;
		return Table;
		//Table->UpdataTable(Holelist);
	}
	return nullptr;
}

//char* StrimName[2] = { "aux_trim_1","aux_trim_0" };
bool ASkuComponentActor::GetHole(TArray<FVector2D>& Holelist)
{
	char* StrimName[] = { "aux_trim_1","aux_trim_0" };
	auto location = GetActorLocation();
	auto rot = GetActorRotation();
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	if (!SDK)
		return false;

	const char *AnsiResID = TCHAR_TO_ANSI(*Mx);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false, true, false);

	if (!pObj)
		return false;
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	if (!pGeometry)
		return false;
	TArray<FVector2D> OutLine;
	for (int i = 0; i <  2; ++i)
	{
		IMeshObject* pMeshObj= pObj->GetMeshObject(StrimName[i]);
		if (!pMeshObj)
			return false;

		kArray<kPoint>* OutBoundarys = nullptr;
		unsigned char* OutCloseFlags = nullptr;
		int OutBoundaryCount;
		bool bSucess = pGeometry->GetMesh2DBoundary(pMeshObj, OutBoundarys, OutCloseFlags, OutBoundaryCount, 2);
		if (!bSucess)
			return false;

		kArray<kPoint>& OutBound = OutBoundarys[0];
		
		for (int index = 0; index < OutBound.size(); ++index)
		{
			FVector2D a = FVector2D(OutBound[index].x, OutBound[index].y);
			auto tp = UHouseFunctionLibrary::GetRotationPos(rot,FVector(a.X,a.Y,0));
			tp += location;
			a.Set(tp.X, tp.Y);
			Holelist.Add(a);
		}
		if (Holelist.Num() > 0)
			break;
	}

	return true;
}
float ASkuComponentActor::GetWashbowlZ(const FString& MD5, bool& bIs)
{
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	const char *AnsiResID = TCHAR_TO_ANSI(*MD5);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false,true,false);
	if (!pObj)
		return false;
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	IMeshObject* pMeshObj = pObj->GetMeshObject("aux_trim_1");
	if (!pMeshObj)
	{
		bIs = false;
		return 0.f;
	}
	kBox3D box = pMeshObj->GetBounds();
	float Z = box.MaxEdge.z;
	bIs = true;
	return Z;
}