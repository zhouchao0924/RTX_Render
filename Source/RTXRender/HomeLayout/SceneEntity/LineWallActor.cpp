// Copyright© 2017 ihomefnt All Rights Reserved.


#include "LineWallActor.h"
#include "IPlatformFilePak.h"
#include "../SectionBuilder/SectionBuilderWrapper.h"
#include "../DataLoadSaver/MaterialSaveGame.h"
#include "PeExtendedToolKitBPLibrary.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "../UserInterface/RulerLabelWidget.h"
#include "ResourceMgr.h"
#include "ModelFile.h"
#include "EditorGameInstance.h"
#include "../HouseArchitect/AreaSystem.h"
#include "KismetProceduralMeshLibrary.h"
#include "Building/BuildingSystem.h"
#include "Building/DRModelFactory.h"
#include "Building/DRFunLibrary.h"
#include "../HouseArchitect/WallBuildSystem.h"
#include "Building/DRStruct.h"
#include "Kismet/GameplayStatics.h"
#include "HomeLayout/DataNodes/MaterialNode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Building/HouseComponent/ComponentManagerActor.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"
#include "Data/FunctionLibrary/DROperationHouseFunctionLibrary.h"

// Sets default values
ALineWallActor::ALineWallActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bCreateCollison = true;

	WallRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = WallRootComp;

	TJXMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TJXMesh"));
	TJXMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TJXMesh->SetCastShadow(false);

	LeftLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftLabelWidget"));
	LeftLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	RightLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RightLabelWidget"));
	RightLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ALineWallActor::BeginPlay()
{
	Super::BeginPlay();
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
		{
			WallSystem = GameInst->WallBuildSystem;
		}
	}
	LeftLabelWidget->SetVisibility(false);
	RightLabelWidget->SetVisibility(false);
}

// Called every frame
void ALineWallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALineWallActor::UpdateBuildingData(UBuildingData *Data)
{
	UBuildingComponent *Comp = FindComponentByClass<UBuildingComponent>();
	if (Comp)
	{
		Comp->SetData(Data);
	}
}

void ALineWallActor::InitLineWall_Implementation(UBuildingData* BuildingData)
{
	ResetWallDefaultMaterial();
	if (WallComp3d == nullptr) {
		WallComp3d = NewObject<UBuildingComponent>(this); 
		WallComp3d->ComponentTags.Add(TEXT("Wall3dMesh"));
		WallComp3d->OnNotifyCreateMesh.AddDynamic(this, &ALineWallActor::OnSDKObjectUpdated);
		WallComp3d->RegisterComponent();
		WallComp3d->AttachTo(RootComponent, NAME_None, EAttachLocation::KeepWorldPosition, true);
		WallComp3d->SetData(BuildingData);
	}
}

void ALineWallActor::UpdateMeshByNode()
{
}


double ALineWallActor::GetFaceArea(int32 FaceIndex)
{
	if (WallComp3d && WallComp3d->GetNumSections() > FaceIndex)
	{
		FProcMeshSection* _Array = WallComp3d->GetProcMeshSection(FaceIndex);
		const int _NUM = _Array->ProcIndexBuffer.Num();
		double _Area = 0.0;
		int _a, _b, _c;
		for (int i = 0; i < _NUM; i += 3)
		{
			_a = _Array->ProcIndexBuffer[i];
			_b = _Array->ProcIndexBuffer[(i + 1) % _NUM];
			_c = _Array->ProcIndexBuffer[(i + 2) % _NUM];

			double _AB = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_b].Position).Size2D();
			double _BC = (_Array->ProcVertexBuffer[_b].Position - _Array->ProcVertexBuffer[_c].Position).Size2D();
			double _CA = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_a].Position).Size2D();
			double p = (_AB + _BC + _CA) / 2.0;
			_Area += FMath::Sqrt(p * (p - _AB) * (p - _BC) * (p - _CA));
		}
		return _Area;
	}
	return -1.0;
}

void ALineWallActor::UpdateWallPos()
{
}

TArray<AWall_Boolean_Base*> ALineWallActor::GetOpeningsOnWall()
{
	TArray<AWall_Boolean_Base*> Holes;
	if (WallComp3d)
	{
		TArray<int> _HoleID;
		int _WallID = WallComp3d->Data->GetID();
		UBuildingSystem * _BS = WallComp3d->Data->GetBuildingSystem();
		if (_BS)
		{
			IObject *pWall = _BS->GetObject(_WallID);
			if (pWall)
			{
				IValue & _Value = pWall->GetPropertyValue("Holes");
				const int num = _Value.GetArrayCount();

				for (int i = 0; i < num; ++i)
				{
					IValue & _Hole = _Value.GetField(i);
					IValue & _CValue = _Hole.GetField("HoleID");
					int _ID = _CValue.IntValue();
					_HoleID.Add(_ID);
				}
			}
		}
		TArray<AActor*> _Holes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWall_Boolean_Base::StaticClass(), _Holes);
		for (int j = 0; j < _HoleID.Num(); ++j)
		{
			for (int i = 0; i < _Holes.Num(); ++i)
			{
				AWall_Boolean_Base * _Hole_Base = Cast<AWall_Boolean_Base>(_Holes[i]);
				if (_Hole_Base)
				{
					if (_Hole_Base->BuildingData->GetID() == _HoleID[j])
					{
						Holes.Add(_Hole_Base);
					}
				}
			}
		}

	}
	return Holes;
}

void ALineWallActor::UpdateWallRuler(UPARAM(ref) FPaintContext &Context)
{

}

bool ALineWallActor::DoStartCornerConnectWall() const
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return false;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return false;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return false;
	}

	ObjectID* objectIds(nullptr);

	int connectedWallCount = projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP0(), objectIds);

	return connectedWallCount > 1;
}

bool ALineWallActor::DoEndCornerConnectWall() const
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return false;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return false;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return false;
	}

	ObjectID* objectIds(nullptr);

	int connectedWallCount = projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP1(), objectIds);

	return connectedWallCount > 1;
}

FVector2D ALineWallActor::ConvertScene2LocalPnt(const FVector2D& ScenePnt) const
{
	return ScenePnt;
}

FVector2D ALineWallActor::ConvertLocal2ScenePnt(const FVector2D& LocalPnt) const
{
	return LocalPnt;
}

void ALineWallActor::DoWallBooleanOper()
{
	DoWallBooleanOperationImplByBlueprint();
}

void ALineWallActor::UpdateWall3dMaterial()
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return;
	}

	TArray<FDRMaterial> wallMaterials(solidWallAdapter->GetWallMaterials());
	
	for (int32 MatIndex = 0; MatIndex < wallMaterials.Num(); ++MatIndex)
	{
		SetMat(wallMaterials[MatIndex], WallComp3d, MatIndex);
		SetMaterialUV(WallComp3d, MatIndex);
	}
}

void ALineWallActor::UpdateTJXMaterial()
{
}

FString GetShowLabelByDist(float DistByCM)
{
	FString ShowLabel;

	int32 DistByMM = DistByCM * 10;
	ShowLabel = FString::Printf(TEXT("%d mm"), DistByMM);
	return ShowLabel;
}

void ALineWallActor::UpdateRulerLabel()
{
}

bool ALineWallActor::IsLegalWall() const
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return false;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return false;
	}

	FDRWallPosition wallPositionData = solidWallAdapter->GetWallPositionData();

	return (wallPositionData.StartPos - wallPositionData.EndPos).Size() > LEGAL_WALL_LENGTH;
}

void ALineWallActor::SetMat(FDRMaterial MatNode, UProceduralMeshComponent* PMesh, int32 MatIndex)
{
	if (MatNode.ModelID != 0)
	{
		UResourceMgr * rMgr = UResourceMgr::Instance(this);
		UModelFile * modefile = (UModelFile*)rMgr->FindRes(GetWallResIDByModelID(MatNode.ModelID));
		if (modefile)
		{
			modefile->ForceLoad();
			UMaterialInstanceDynamic * ue4mat = modefile->GetUE4Material(0);
			PMesh->SetMaterial(MatIndex, ue4mat);
		}
	}
	else
	{
		bool bValid = false;
		UMaterialInterface *WallMaterial = nullptr;
		FString defaultWallMat = DEFAULT_WALL_MATERIAL;
		WallMaterial = Cast<UMaterialInterface>(UPeExtendedToolKitBPLibrary::LoadObjectFromAssetPath(UMaterial::StaticClass(),
			nullptr, FName(*defaultWallMat), false, bValid));
		PMesh->SetMaterial(MatIndex, WallMaterial);
	}
}

TArray<FDRMaterial> ALineWallActor::GetWallMaterial()
{
	TArray<FDRMaterial> wallMaterials;
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return wallMaterials;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return wallMaterials;
	}

	wallMaterials = solidWallAdapter->GetWallMaterials();

	return wallMaterials;
}

void ALineWallActor::UpdateWall3d()
{

}

void ALineWallActor::ResetWallDefaultMaterial()
{
	int Local_RoomClassID;
	int Local_CraftID;
	TArray<int> Local_CraftList;

	TArray<FDRMaterial> wallMaterials;
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return;
	}

	wallMaterials = solidWallAdapter->GetWallMaterials();


	for (FDRMaterial &Wallmaterial : wallMaterials)
	{
		if (Wallmaterial.ModelID == 0)
		{
			Wallmaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRWallConfig);
			GetDefaultRoomClassAndCraft(Wallmaterial.ModelID, Local_RoomClassID, Local_CraftID);
			Wallmaterial.RoomClassID = Local_RoomClassID;
			Wallmaterial.CraftID = Local_CraftID;
		}
		GetDefaultRoomClassAndCraft(Wallmaterial.ModelID, Local_RoomClassID, Local_CraftID);
		GetCraftList(Wallmaterial.ModelID, Local_RoomClassID, Local_CraftList);
		if (Wallmaterial.RoomClassID != Local_RoomClassID)
		{
			Wallmaterial.RoomClassID = Local_RoomClassID;
			Wallmaterial.CraftID = Local_CraftID;
		}
		else if (!Local_CraftList.Contains(Wallmaterial.CraftID))
		{
			Wallmaterial.CraftID = Local_CraftList.IsValidIndex(0) ? Local_CraftList[0] : Local_CraftID;
		}
	}

	solidWallAdapter->SetWallMaterials(wallMaterials);
}

void ALineWallActor::SetMaterialUV(UProceduralMeshComponent* PMesh, int32 MatIndex)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GetOwner());
	if (projectDataManager == nullptr) {
		return;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(ObjectId));
	if (solidWallAdapter == nullptr) {
		return;
	}

	FVector2D local_UVOffset;
	FVector2D local_UVScale;
	float local_UVRotation;
	switch (MatIndex)
	{
	case 0:
		local_UVOffset = solidWallAdapter->GetFrontUVPos();
		local_UVScale = solidWallAdapter->GetFrontUVScale();
		local_UVRotation = solidWallAdapter->GetFrontUVAngle();
		break;
	case 1:
		local_UVOffset = solidWallAdapter->GetBackUVPos();
		local_UVScale = solidWallAdapter->GetBackUVScale();
		local_UVRotation = solidWallAdapter->GetBackUVAngle();
		break;
	default:
		local_UVOffset = solidWallAdapter->GetSideUVPos();
		local_UVScale = solidWallAdapter->GetSideUVScale();
		local_UVRotation = solidWallAdapter->GetSideUVAngle();
		break;
	}
	UMaterialInstanceDynamic* local_DynamicMaterial = PMesh->CreateDynamicMaterialInstance(MatIndex);
	local_UVScale = local_UVScale / 100;
	if (local_DynamicMaterial)
	{
		local_DynamicMaterial->SetScalarParameterValue(TEXT("位移X"), local_UVOffset.X);
		local_DynamicMaterial->SetScalarParameterValue(TEXT("位移Y"), local_UVOffset.Y);
		local_DynamicMaterial->SetScalarParameterValue(TEXT("贴图角度"), local_UVRotation);
		local_DynamicMaterial->SetScalarParameterValue(TEXT("法线角度"), local_UVRotation);
	}
}

void ALineWallActor::GetLabelWidget(bool &LeftRuler, bool &RightRuler)
{
	LeftRuler = RightRuler = false;
	if (WallComp3d)
	{
		//UBuildingData *Walldata= WallComp3d->Data;
		if (WallComp3d->WallPlane)
		{
			if (WallComp3d->WallPlane->LeftLabelWidget)
			{
				LeftRuler = WallComp3d->WallPlane->LeftLabelWidget->IsVisible();
			}
			if (WallComp3d->WallPlane->RightLabelWidget)
			{
				RightRuler = WallComp3d->WallPlane->RightLabelWidget->IsVisible();
			}
		}
	}
}

void ALineWallActor::SetRulerVisible(const bool &ShowRuler, const bool IsLeftRuler)
{
	if (WallComp3d)
	{
		//UBuildingData *Walldata= WallComp3d->Data;
		if (WallComp3d->WallPlane)
		{
			if (IsLeftRuler&&WallComp3d->WallPlane->LeftLabelWidget)
			{
				WallComp3d->WallPlane->LeftLabelWidget->SetVisibility(ShowRuler);
				WallComp3d->WallPlane->LeftLineShow = ShowRuler;
			}
			else
				if (WallComp3d->WallPlane->RightLabelWidget)
				{
					WallComp3d->WallPlane->RightLabelWidget->SetVisibility(ShowRuler);
					WallComp3d->WallPlane->RightLineShow = ShowRuler;
				}
		}
	}
}

void ALineWallActor::SetAllRulersVisible(const bool &ShowRuler)
{
	if (WallComp3d)
	{
		if (WallComp3d->WallPlane)
		{
			if (WallComp3d->WallPlane->LeftLabelWidget)
			{
				WallComp3d->WallPlane->LeftLabelWidget->SetVisibility(ShowRuler);
				WallComp3d->WallPlane->LeftLineShow = ShowRuler;
			}
			if (WallComp3d->WallPlane->RightLabelWidget)
			{
				WallComp3d->WallPlane->RightLabelWidget->SetVisibility(ShowRuler);
				WallComp3d->WallPlane->RightLineShow = ShowRuler;
			}
		}
	}
}
void ALineWallActor::GetWallThick(float& Left, float& Right)
{
	if (WallComp3d)
	{
		if (WallComp3d->Data)
		{
			int WallID = WallComp3d->Data->GetID();
			UBuildingSystem *BuildingSystem = WallComp3d->Data->GetBuildingSystem();
			if (BuildingSystem)
			{
				UBuildingData* WallData = BuildingSystem->GetData(WallID);
				if (WallData)
				{
					Left = WallData->GetFloat("ThickLeft");
					Right = WallData->GetFloat("ThickRight");
				}
			}
		}
	}
}

void ALineWallActor::SetWallThick(const bool IsLeft, const float Thick)
{
	if (WallComp3d)
	{
		if (WallComp3d->Data)
		{
			int WallID = WallComp3d->Data->GetID();
			UBuildingSystem *BuildingSystem = WallComp3d->Data->GetBuildingSystem();
			if (BuildingSystem)
			{
				UBuildingData* WallData = BuildingSystem->GetData(WallID);
				if (WallData)
				{
					if (IsLeft)
					{
						WallData->SetFloat("ThickLeft", Thick);
					}
					else
					{
						WallData->SetFloat("ThickRight", Thick);
					}
				}
			}
		}
	}
}

void ALineWallActor::DeleteWall(bool IsDeleteWallPlane)
{
	if (WallComp3d)
	{
		if (WallComp3d->Data)
		{
			int WallID = WallComp3d->Data->GetID();
			if (WallSystem)
			{
				WallSystem->DeleteSlectActor(WallID, IsDeleteWallPlane);
			}
		}
	}
}

const FCopyWallNode& ALineWallActor::GetWallInfo()
{
	TArray<FVector> WallNodes;
	if (WallSystem)
	{
		if (WallComp3d)
		{
			if (WallComp3d->Data)
			{
				TempWallNode.LeftThick = WallComp3d->Data->GetFloat("ThickLeft");
				TempWallNode.RightThick = WallComp3d->Data->GetFloat("ThickRight");
				int WallID = WallComp3d->Data->GetID();
				if (WallSystem->BuildingSystem)
				{
					WallSystem->BuildingSystem->GetWallBorderLines(WallID, WallNodes, TempWallNode.Height);
					if (WallNodes.Num() == 6)
					{
						TempWallNode.StartPos = FVector2D(WallNodes[0]);
						TempWallNode.StartLeft = FVector2D(WallNodes[1]);
						TempWallNode.EndLeft = FVector2D(WallNodes[2]);
						TempWallNode.EndPos = FVector2D(WallNodes[3]);
						TempWallNode.EndRight = FVector2D(WallNodes[4]);
						TempWallNode.StartRight = FVector2D(WallNodes[5]);
						TempWallNode.StartPnt = WallNodes[0];
						TempWallNode.EndPnt = WallNodes[3];
					}
				}
			}
		}
	}
	return TempWallNode;
}

void ALineWallActor::SetIsClosed(const bool s)
{
	if (WallComp3d&&WallComp3d->Data)
	{
		WallComp3d->Data->SetBool("bRoomWall", s);
	}
}

bool ALineWallActor::GetIsClosed()
{
	if (WallComp3d&&WallComp3d->Data)
	{
		return WallComp3d->Data->GetBool("bRoomWall");
	}
	return false;
}

int32 ALineWallActor::GetWallID()
{
	if (WallComp3d&&WallComp3d->Data)
	{
		return WallComp3d->Data->GetID();
	}
	return INDEX_NONE;
}
bool ALineWallActor::IsEndWall()
{
	int32 StartCornerID;
	int32 EndCornerID;
	if (WallComp3d)
	{
		if (WallComp3d->Data)
		{
			int WallID = WallComp3d->Data->GetID();
			UBuildingSystem *BuildingSystem = WallComp3d->Data->GetBuildingSystem();
			if (BuildingSystem)
			{
				UBuildingData* WallData = BuildingSystem->GetData(WallID);
				if (WallData)
				{

					StartCornerID = WallData->GetInt("P0");
					EndCornerID = WallData->GetInt("P1");
					int num = BuildingSystem->GetObject(StartCornerID)->GetPropertyValue("Walls").IntArrayValue().size();
					if (num <= 1) {
						return true;
					}
					num = BuildingSystem->GetObject(EndCornerID)->GetPropertyValue("Walls").IntArrayValue().size();
					if (num <= 1) {
						return true;
					}

				}
			}
		}
	}

	return false;
}

bool ALineWallActor::HasBooleanTypes(const TArray<EOpeningType> booleantypes)
{
	TArray<AWall_Boolean_Base*> TempBoolean = GetOpeningsOnWall();
	for (int32 i = 0; i < TempBoolean.Num(); i++)
	{
		FOpeningSimpleAttr FOpenAttr = TempBoolean[i]->GetOpeningSimpleAttr();
		if (booleantypes.Contains(FOpenAttr.Type)) {
			return true;
		}
	}
	return false;
}

/*
void ALineWallActor::SetHousePlugin(int32 id,AComponentManagerActor* ComponentManager)
{
	if (!ComponentManager)
	{
		HousePlugin.Remove(id);
	}
	else
	{
		HousePlugin.Emplace( id,ComponentManager);
	}
}*/

TArray<AComponentManagerActor*> ALineWallActor::GetHousePlugin() const
{
	TArray<AComponentManagerActor*> OutHousePluginArr;
	TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
	for (int i = 0; i < OutActor.Num(); ++i)
	{
		AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
		if (ComponentManager)
		{
			FComponenetInfo HouseCompInfo = ComponentManager->GetHouseComponentInfo();
			if (HouseCompInfo.ComponenetType != EComponenetType::_Sewer)
			{
				TArray<int32> IDArr = HouseCompInfo.WallID;
				int32 WallID = INDEX_NONE;
				if (WallComp3d&&WallComp3d->Data)
				{
					WallID = WallComp3d->Data->GetID();
				}
				if (IDArr.Num() == 1)
				{
					if (IDArr[0] == WallID)
						OutHousePluginArr.Add(ComponentManager);
				}
				else if (IDArr.Num() == 2)
				{
					if (IDArr[0] == WallID || IDArr[1] == WallID)
					{
						OutHousePluginArr.Add(ComponentManager);
					}
				}
			}
		}
	}
	return OutHousePluginArr;
}

TArray<AComponentManagerActor*> ALineWallActor::GetHousePluginOnWall(const int32& Channel, ARoomActor* Area)
{
	TArray<AComponentManagerActor*> OutHousePluginArr;
	if (Area &&(Channel==4 || Channel==5))
	{
		TArray<AActor*> TempActors= UDRFunLibrary::GetAllHousePlugins(this);
		TArray<AComponentManagerActor*> TempHousePluginArr = Area->GetHousePlugin(TempActors);
		if (TempHousePluginArr.Num() > 0)
		{
			if (WallComp3d&&WallComp3d->Data&&WallComp3d->Data->GetBuildingSystem())
			{
				TArray<FVector> WallNodes;
				int WallID= WallComp3d->Data->GetID();
				float height;
				WallComp3d->Data->GetBuildingSystem()->GetWallBorderLines(WallID, WallNodes, height);
				if (WallNodes.Num() == 6)
				{
					FVector StartPos,EndPos;
					if (Channel == 4)
					{
						StartPos = WallNodes[5];
						EndPos = WallNodes[4];
					}
					else
					{
						StartPos = WallNodes[1];
						EndPos = WallNodes[2];
					}
					for (int i = 0; i < TempHousePluginArr.Num(); ++i)
					{
						if (TempHousePluginArr[i])
						{
							FComponenetInfo HouseCompInfo = TempHousePluginArr[i]->GetHouseComponentInfo();
							if (HouseCompInfo.ComponenetType != EComponenetType::_Sewer)
							{
								float length = UKismetMathLibrary::GetPointDistanceToSegment(FVector(HouseCompInfo.Loc, 0), StartPos, EndPos);
								if (HouseCompInfo.WallID.Num() == 1 && length < HouseCompInfo.Width*0.5 + 2)
								{
									OutHousePluginArr.Add(TempHousePluginArr[i]);
								}
								else
									if (HouseCompInfo.WallID.Num() > 1)
									{
										if (length < HouseCompInfo.Length*0.5 + 2 || length < HouseCompInfo.Width*0.5 + 2)
										{
											OutHousePluginArr.Add(TempHousePluginArr[i]);
										}
									}
							}
						}
					}
				}
			}
		}
	}
	return OutHousePluginArr;
}

TArray<AComponentManagerActor*>ALineWallActor::GetHousePluginOnSurfaceWall(const int32& Channel, const TArray<AComponentManagerActor*>&HousePlugins)
{
	TArray<AComponentManagerActor*> OutHousePluginArr;
	if (Channel == 4 || Channel == 5)
	{
		if (HousePlugins.Num() > 0)
		{
			if (WallComp3d&&WallComp3d->Data&&WallComp3d->Data->GetBuildingSystem())
			{
				TArray<FVector> WallNodes;
				int WallID = WallComp3d->Data->GetID();
				float height;
				WallComp3d->Data->GetBuildingSystem()->GetWallBorderLines(WallID, WallNodes, height);
				if (WallNodes.Num() == 6)
				{
					FVector StartPos, EndPos;
					if (Channel == 4)
					{
						StartPos = WallNodes[5];
						EndPos = WallNodes[4];
					}
					else
					{
						StartPos = WallNodes[1];
						EndPos = WallNodes[2];
					}
					for (int i = 0; i < HousePlugins.Num(); ++i)
					{
						if (HousePlugins[i])
						{
							FComponenetInfo HouseCompInfo = HousePlugins[i]->GetHouseComponentInfo();
							if (HouseCompInfo.ComponenetType != EComponenetType::_Sewer)
							{
								float length = UKismetMathLibrary::GetPointDistanceToSegment(FVector(HouseCompInfo.Loc, 0), StartPos, EndPos);
								if (HouseCompInfo.WallID.Num() == 1 && length < HouseCompInfo.Width*0.5 + 2)
								{
									OutHousePluginArr.Add(HousePlugins[i]);
								}
								else
									if (HouseCompInfo.WallID.Num() > 1)
									{
										if (length < HouseCompInfo.Length*0.5 + 2 || length < HouseCompInfo.Width*0.5 + 2)
										{
											OutHousePluginArr.Add(HousePlugins[i]);
										}
									}
							}
						}
					}
				}
			}
		}
	}
	return OutHousePluginArr;
}

bool ALineWallActor::GetWallStartAndEndPos(const int32& Channel,FVector &Start, FVector &End)
{
	if (WallComp3d&&WallComp3d->Data&&WallComp3d->Data->GetBuildingSystem())
	{
		TArray<FVector> WallNodes;
		int WallID = WallComp3d->Data->GetID();
		float height;
		WallComp3d->Data->GetBuildingSystem()->GetWallBorderLines(WallID, WallNodes, height);
		if (WallNodes.Num() == 6)
		{
			if (Channel == 4)
			{
				Start = WallNodes[5];
				End = WallNodes[4];
				return true;
			}
			else
				if (Channel == 5)
				{
					Start = WallNodes[1];
					End = WallNodes[2];
					return true;
				}
		}
	}
	return false;
}

void ALineWallActor::Destroyed()
{
	TArray<AComponentManagerActor*> OutHousePluginArr;
	TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
	for (int i = 0; i < OutActor.Num(); ++i)
	{
		AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
		if (ComponentManager)
		{
			OutHousePluginArr.Add(ComponentManager);
		}
	}
	TArray<AComponentManagerActor*> Temp= GetHousePluginOnSurfaceWall(4, OutHousePluginArr);
	for (int i = 0; i < Temp.Num(); ++i)
	{
		if (Temp[i])
		{
			Temp[i]->ManualDestroyHouseComponent();
		}
	}
	Temp=GetHousePluginOnSurfaceWall(5, OutHousePluginArr);
	for (int i = 0; i < Temp.Num(); ++i)
	{
		if (Temp[i])
		{
			Temp[i]->ManualDestroyHouseComponent();
		}
	}
	Super::Destroyed();
}