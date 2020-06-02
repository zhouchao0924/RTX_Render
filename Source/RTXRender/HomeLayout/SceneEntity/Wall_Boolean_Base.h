 

#pragma once

#include "../DataNodes/SkirtingNode.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../../Struct/Struct.h"
#include "Building/BuildingData.h"
#include "Building/DRStruct.h"
#include "Building/DRActor.h"
#include "Building/BooleanBase.h"
#include "Components/BoxComponent.h"
//#include "./FurnitureModelActor.h"
#include "Wall_Boolean_Base.generated.h"

UCLASS()
class RTXRENDER_API AWall_Boolean_Base : public ADRActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Root, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* B_RootComponent;

  	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	    class UArrowComponent* Left_Arrow;
 
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* Right_Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
       class UStaticMeshComponent* Lit_Cube_01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
       class UStaticMeshComponent* Lit_Cube_02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UStaticMeshComponent* Lit_Cube_03;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UStaticMeshComponent* Lit_Cube_04;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UProceduralMeshComponent* LitUp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UProceduralMeshComponent* Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UProceduralMeshComponent* Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UProceduralMeshComponent* Up;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	   class UProceduralMeshComponent* Under;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
       class UStaticMeshComponent* Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class  UStaticMeshComponent* End;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
    	class USceneComponent* Cur_Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Cur_Under;

public:	

	// 初始化
	AWall_Boolean_Base();

	// 游戏开始
	virtual void BeginPlay() override;
	
	// Tick事件
	virtual void Tick( float DeltaSeconds ) override;


	UFUNCTION(BlueprintPure, Category = "FUNCTION")
	FVector GetVertex(float VectorAddValue , int32 Index);

	UFUNCTION(BlueprintCallable, Category = "FUNCTION")
	    void Construct();

	UFUNCTION(BlueprintCallable, Category = "FUNCTION")
	    void Hidden(bool isHidden);


	UFUNCTION(BlueprintCallable, Category = "FUNCTION")
		TArray<FVector2D>VertexToUV(TArray<FVector>InVector, bool YZ, FVector2D Scale, bool IsWorld=false);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallConfig, meta = (AllowPrivateAccess = "true"))
		float WallHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallConfig, meta = (AllowPrivateAccess = "true"))
		FVector WallLocation;

	//墙体厚度_Left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallConfig, meta = (AllowPrivateAccess = "true"))
		float Wall_Thick_Left;

	//墙体厚度_Right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WallConfig, meta = (AllowPrivateAccess = "true"))
		float Wall_Thick_Right;

	//Boolean Left 宽度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		float LeftWidth=50;

	//Boolean Right 宽度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		float RightWidth=50;

	//Boolean Right 高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		float Height=100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	    float Length;

	//Boolean 离地
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		float GroundHeight;

	//Boolean 是否贴紧地面
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		bool PasteTheGround;

	//水平翻转
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	    bool HorizontalFlip;
	//垂直翻转
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		bool VerticalFlip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Temp, meta = (AllowPrivateAccess = "true"))
		bool IsSelect;

	//Boolean 颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		FLinearColor Color= FLinearColor(0.055,0.055,0.055,1);

	//Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
		FString Name="Name";

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		UBuildingData* BuildingData;
	UPROPERTY(BlueprintReadWrite)
		class AFurnitureModelActor* ComModel;
	UPROPERTY(BlueprintReadOnly)
		FDRHoleStruct HoleStruct;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		int32 ObjectID;
	UPROPERTY(BlueprintReadWrite)
		FDeleteHoleInfo DeleteHoleInfo;
public:
	UFUNCTION(BlueprintCallable, meta = (Category = "Hole"))
		void SetHoleStruct(FDRHoleStruct const & Hole);


	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Judge"))
	bool DoesBelong2Wall(class ALineWallActor *InOwnerWall2Judge) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Judge"))
	bool IsDoorOrGroundWindow() const;

	UFUNCTION(BlueprintCallable, meta = (Category = "Attributes"))
	FOpeningSimpleAttr GetOpeningSimpleAttrBlueprint() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Attributes"))
	FOpeningSimpleAttr GetOpeningSimpleAttr() const; 

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Attributes"))
	AFurnitureModelActor* GetOpeningRelateModel() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Iray"))
	FString GetMeshMat(UProceduralMeshComponent* Mesh);

	void SetBuildingData(UBuildingData * Data);
	void UpdateBuildingData(UBuildingData * Data);
	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "BuildingDataUpdate"))
	void BPUpdateBuildingData(UBuildingData * Data);
	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "BuildingDataUpdate"))
		void UpdateHoleModel();
	UFUNCTION(BlueprintCallable, meta = (Category = "Attributes"))
	void AutoSetWindowForward();
	UFUNCTION(BlueprintCallable, meta = (Category = "Attributes"))
	void IsNeedAdaptation();
private:

	TArray<FColor>NullVColors;
	TArray<FProcMeshTangent>NullTangents;
 
	//旋转转换
	FRotator RPYTOPRY(FRotator Rotation);
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetVerticalflip();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool Gethorizontalflip();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		EDRModelType IsDoororBayWindow();
	UFUNCTION(BlueprintCallable)
		float GetMaxZPos();
	UFUNCTION(BlueprintCallable)
		float GetMaxWidth();
	UFUNCTION(BlueprintCallable)
		float GetMaxHeight();
	UFUNCTION(BlueprintCallable)
		float GetZPos();
	UFUNCTION(BlueprintCallable)
		float GetWidth();
	UFUNCTION(BlueprintCallable)
		float GetHeight();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetBayWindowLength();
	UFUNCTION(BlueprintCallable)
		void SetBayWindowLength(const float &Lm);
	UFUNCTION(BlueprintCallable)
		void SetZPos(const float &z);
	UFUNCTION(BlueprintCallable)
		void SetWidth(const float &w);
	UFUNCTION(BlueprintCallable)
		void SetHeight(const float &h);
	UFUNCTION(BlueprintCallable)
		void ShowUI(bool IsShow = false);
	UFUNCTION(BlueprintCallable)
		void DeleteSelf();
	UFUNCTION(BlueprintCallable)
		int32 GetHoleID();
	UFUNCTION(BlueprintCallable)
		void SetNewWallID(int32 ID);
	UFUNCTION(BlueprintCallable)
		void MoveComponent(bool IsSelected = false);
	UFUNCTION(BlueprintCallable)
		void Init();
	UFUNCTION(BlueprintCallable)
		void ReadyMove(bool IsReadyMove = false);
	UFUNCTION(BlueprintCallable)
		void Selected(const FVector2D MousePos,bool IsSelected = false);
	UFUNCTION(BlueprintCallable)
		void ClearAllSelect(bool bIsIgnoreSelf);
	UFUNCTION(BlueprintCallable)
		void UpDataWallID(int32 ID);
	UFUNCTION(BlueprintCallable)
		void GetMoveCurWallID(int32 &ID);
	UFUNCTION(BlueprintCallable)
		void GetCurrentHoleWidth(float &Width);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBooleanBase *BooleanBaseHUDWidget;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsSelect;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsMove;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsReadyMove;
	UFUNCTION(BlueprintCallable)
		void SetVerticalflip(bool filp);
	UFUNCTION(BlueprintCallable)
		void Sethorizontalflip(bool filp);
	UFUNCTION(BlueprintCallable)
		void UpdataUMGflip();
	UFUNCTION(BlueprintCallable)
		int GetDependsWallId();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsOriginalHole();
	UFUNCTION(BlueprintCallable)
		void SetHoleStatusType(const EHolestatusType CurrentHolestatusType);
};
