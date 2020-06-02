// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "CraftPavingStructures.h"
#include "CraftPaving/IPatternEditor.h"
#include "CraftPavingComponent.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Object, LOD), meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class BUILDINGSDK_API UCraftPavingComponent : public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void UpdatePolygon();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool AddShape(const FShapeOptions& Options, int32& ShapeId);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool SetShapeOptions(int32 ShapeId, const FShapeOptions& Options);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		FShapeOptions GetShapeOptions(int32 ShapeId);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool TestHitShape(const FVector& StartPoint, const FVector& EndPoint, const TArray<AActor*>& IgnoreActors, int32& ShapeId, int32& SectionId);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void RemoveShape(int32 ShapeId);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void SetIntervalSize(float IntervalSize);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void GetShapeOffsetExpression(int32 ShapeId, bool bIsOffsetX, FString& Expression);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool SetShapeOffsetExpression(int32 ShapeId, const FString& Expression, bool bIsOffsetX);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void GetJointExpression(bool bIsHorizontal, FString& Expression);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool SetJointExpression(bool bIsHorizontal, const FString& Expression);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool CheckAliasIsValid(const FString& Alias);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool GetAttributeAlias(int32 ShapeId, EAttributeTypes Type, FString& Alias);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void SetShapeAttributeAlias(int32 ShapeId, EAttributeTypes Type, const FString& Alias);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void RemoveShapeAttributeAlias(int32 ShapeId, EAttributeTypes Type);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void SetShapeAngle(int32 ShapeId, float NewAngle);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool SavePattern(const FString& Path, const FString& FileName);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		bool LoadPattern(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void ReleasePattern();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		FString GetResId();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		int32 GetModifyVersion();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		int32 GetLocalVersion();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void AddLocalVersion(int32 Number);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void ResetModifyFlag();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void GetPavingJoint(FVector& HorizontalJoint, FVector& VerticalJoint);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		void SetPavingJoint(const FVector& HorizontalJoint, const FVector& VerticalJoint);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		TArray<int32>GetAllShapeIds();

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		int32 GetSectionIdByShapeId(int32 ShapeId);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
	bool GetShapePreviewColor(int32 ShapeId, FColor& Color);

	UFUNCTION(BlueprintCallable, Category = CraftPaving)
	bool SetShapePreviewColor(int32 ShapeId, const FColor& Color);

	IPatternObject* GetInternalObject() { return mPatternObject; };

protected:

	void UpdateShapePolygon();
	void UpdateIntervalPolygon();

	FShapeOptions ConvertOptions(std::shared_ptr<ShapeOptions> Options);
	std::shared_ptr<ShapeOptions> ConvertOptions(const FShapeOptions& Options);

	bool IsPointInsidePolygon(const TArray<FVector>& Polygon, const FVector& CheckPoint);

private:
	IPatternEditor* mPatternEditor;
	IPatternObject* mPatternObject;

	// <SectionId, ShapeId>
	TMap<uint32, uint32> mShapeIdMap;
};
