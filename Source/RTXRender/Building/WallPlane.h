// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "UserWidget.h"
#include "../HomeLayout/UserInterface/RulerLabelWidget.h"
#include "../HomeLayout/UserInterface/UserInputRulerWidget.h"
#include "../HomeLayout/DataNodes/SavedDataNode.h"
#include "Materials/MaterialInstance.h"
#include "WallPlane.generated.h"

UCLASS()
class AJDR_API AWallPlaneBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallPlaneBase();
	UFUNCTION(BlueprintCallable)
	void InitPlane(const TArray<FVector> _TArrayPos, float ZPos, const float& LeftThickness, const float& RightThickness, int32 ObjectID=-1, bool bgeneratecollision = false, bool bIsTick = false);
	UFUNCTION(BlueprintImplementableEvent, Category = "BuildPlane")
	void Createplane();
	UFUNCTION(BlueprintCallable)
	void SetRuler(bool IsShow);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildPlane")
	void SetLabelWidgetRotator(float WidgetAngle);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildPlane")
	void SetWallPlaneMaterial(EWallType Type= EWallType::OriginalWall, bool IsDelete = false,bool IsBearingWall=false,bool IsOpenNewWallType=false);
	UFUNCTION(BlueprintCallable)
		void CalculateSpot(const TArray<FVector> &VerList, TArray<FVector>& OutPosList,float& PlaneArea,float& Height, FVector& StartPos, FVector& EndPos);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildPlane")
		void Delete();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SetInputWidget")
		void SetInputWidget(int32 LastCornerID = -1);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SetInputWidget")
		bool IsShowRuler();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildPlane")
		void AddDeleteWall();
	UFUNCTION(BlueprintCallable)
		void InitDeleteWall();
	UFUNCTION(BlueprintCallable)
		void SetDeleteWallText();
	UFUNCTION(BlueprintCallable)
		void SetDeleteWallLocationAndRotation();
	UFUNCTION(BlueprintCallable)
		void ShowInputRulerWidget();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "WallMesh"))
		USceneComponent* WallRootComp;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> TrianglesID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> VerListPos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> NormalsDir;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProceduralMeshComponent *Plane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
	class UWidgetComponent* LeftLabelWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
	class UWidgetComponent* RightLabelWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
	UUserInputRulerWidget* InputRulerWidget;//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 WallID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bGeneratecollision;

	//¸¨ÖúÏß
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector>LinePos;

	//left¸¨ÖúÏß
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector> LeftLinePos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool LeftLineShow = true;
	//right¸¨ÖúÏß
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector> RightLinePos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool RightLineShow = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInstance* DefaultMaterialInstance;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 StructIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ThicknessLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ThicknessRight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Thickness;

	//widget spinbox position
	UPROPERTY(BlueprintReadWrite)
		FVector SpinBoxPos; 

	UPROPERTY(BlueprintReadWrite)
		int32 LastCornerID = -1;

	FVector2D LastMousePos;
};
