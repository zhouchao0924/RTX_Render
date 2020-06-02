// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../DataNodes/WallNode.h"
#include "../DataNodes/SavedDataNode.h"
#include "Blueprint/UserWidget.h"
#include "../../Struct/Struct.h"
#include "Building/BuildingComponent.h"
#include "Building/DRActor.h"
#include "Wall_Boolean_Base.h"
#include "LineWallActor.generated.h"

class UWallBuildSystem;
USTRUCT(BlueprintType)
struct FCopyWallNode
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FVector StartPnt;
	UPROPERTY(BlueprintReadOnly)
		FVector EndPnt;
	UPROPERTY(BlueprintReadOnly)
		float LeftThick;
	UPROPERTY(BlueprintReadOnly)
		float RightThick;
	UPROPERTY(BlueprintReadOnly)
		float Height;
	UPROPERTY(BlueprintReadOnly)
		FVector2D StartPos;
	UPROPERTY(BlueprintReadOnly)
		FVector2D EndPos;
	UPROPERTY(BlueprintReadOnly)
		FVector2D StartLeft;
	UPROPERTY(BlueprintReadOnly)
		FVector2D StartRight;
	UPROPERTY(BlueprintReadOnly)
		FVector2D EndLeft;
	UPROPERTY(BlueprintReadOnly)
		FVector2D EndRight;
};


// B_Wall
UCLASS()
class RTXRENDER_API ALineWallActor : public ADRActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALineWallActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
		void UpdateBuildingData(UBuildingData *Data);
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void InitLineWall(UBuildingData* BuildingData);
	void InitLineWall_Implementation(UBuildingData* BuildingData);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnSDKObjectUpdated();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateMeshByNode();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateWallPos();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateWallRuler(UPARAM(ref) FPaintContext &Context);

	UFUNCTION(BlueprintPure, Category = "LineWallActor")
		bool DoStartCornerConnectWall() const;

	UFUNCTION(BlueprintPure, Category = "LineWallActor")
		bool DoEndCornerConnectWall() const;

	UFUNCTION(BlueprintPure, Category = "WallNode")
		FVector2D ConvertScene2LocalPnt(const FVector2D& ScenePnt) const;

	UFUNCTION(BlueprintPure, Category = "WallNode")
		FVector2D ConvertLocal2ScenePnt(const FVector2D& LocalPnt) const;

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void DoWallBooleanOper();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DoWallBooleanOperation"))
		void DoWallBooleanOperationImplByBlueprint();

	UFUNCTION(BlueprintCallable, meta = (Category = "Property"))
		TArray<AWall_Boolean_Base*> GetOpeningsOnWall();

	TArray<FDRMaterial> GetWallMaterial();

	double GetFaceArea(int32 FaceIndex);
	
	virtual void Destroyed();

	void UpdateWall3d();

	UFUNCTION(BlueprintImplementableEvent, Category = "Modify")
		void RemoveFromRecord();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		bool IsLegalWall() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetDefaultRoomClassAndCraft"))
		void GetDefaultRoomClassAndCraft(int ModelID, int &RoomClassID, int &CraftID);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetCraftList"))
		void GetCraftList(int ModelId, int ClassRoomID, TArray<int> &CraftID);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetWallResIDByModelID"))
		FString GetWallResIDByModelID(int32 ModelID);

protected:

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateWall3dMaterial();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateTJXMaterial();

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void UpdateRulerLabel();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "LoadMatByPath"))
		UMaterialInterface* LoadMaterialByPath(const FString &WallMatPath);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DoShowWallRuler"))
		bool DoShowWallRuler() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateBWallByNode"))
		void UpdateBWallByNode();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetWallSystem"))
		class UWallBuildSystem* GetWallSystem() const;

	void SetMat(FDRMaterial MatNode, UProceduralMeshComponent* PMesh, int32 MatIndex);

	void SetMaterialUV(UProceduralMeshComponent* PMesh, int32 MatIndex);

	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void ResetWallDefaultMaterial();

public:
	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "WallMesh"))
		USceneComponent* WallRootComp;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "WallMesh"))
		UProceduralMeshComponent* WallComp2d;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "WallMesh"))
		UBuildingComponent* WallComp3d;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "TJXMesh"))
		UProceduralMeshComponent* TJXMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "WallMesh"))
		bool bCreateCollison;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
		class UWidgetComponent* LeftLabelWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
		class UWidgetComponent* RightLabelWidget;

protected:
	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "Property"))
		FSavedDataNode SavedWallProperty;

public:
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void GetLabelWidget(bool &LeftRuler, bool &RightRuler);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void SetRulerVisible(const bool &ShowRuler, const bool IsLeftRuler);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void SetAllRulersVisible(const bool &ShowRuler);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void GetWallThick(float& Left, float& Right);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void SetWallThick(const bool IsLeft, const float Thick);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void DeleteWall(bool IsDeleteWallPlane = true);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		const FCopyWallNode& GetWallInfo();
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		bool GetIsClosed();
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		void SetIsClosed(const bool s);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LineWallActor")
		int32 GetWallID();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LineWallActor")
		bool IsEndWall();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LineWallActor")
		bool HasBooleanTypes(const TArray<EOpeningType> booleantypes);
	//UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		//void SetHousePlugin(int32 id, AComponentManagerActor* ComponentManager);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LineWallActor")
		TArray<class AComponentManagerActor*> GetHousePlugin() const;
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		TArray<class AComponentManagerActor*> GetHousePluginOnWall(const int32& Channel, ARoomActor* Area);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		TArray<class AComponentManagerActor*> GetHousePluginOnSurfaceWall(const int32& Channel,const TArray<class AComponentManagerActor*>&HousePlugins);
	UFUNCTION(BlueprintCallable, Category = "LineWallActor")
		bool GetWallStartAndEndPos(const int32& Channel,FVector &Start, FVector &End);
public:
	UWallBuildSystem *WallSystem;
	FCopyWallNode TempWallNode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		int32 ObjectId;
};