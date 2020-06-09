// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CGALWrapper/CgTypes.h"
#include "ProceduralMeshComponent.h"
#include "Model/ModelFile.h"
#include "HomeLayout/DataNodes/SkirtingNode.h"
#include "MontageMeshComponent.generated.h"

//TODO: change name to skirting line
UCLASS(Blueprintable)
class UWallLine : public UObject
{
	GENERATED_UCLASS_BODY()
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif
	void ChangeLineType(UStaticMesh *InNewMeshType);

protected:
	void OnLineTypeChanged();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh *Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<int32> Indices;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> Positions;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> Normals;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FProcMeshTangent> Tangents;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector2D> UVs;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FLinearColor> Colors;
};


UCLASS(Blueprintable)
class UStaticMeshInfoExtract : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	void ResetStaticMesh(UStaticMesh *InNewMeshType);

	void ResetMXMesh(UModelFile *InNewMesh);

protected:
	void ClearDataInfo();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UModelFile *MxMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh *Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<int32> Indices;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> Positions;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> Normals;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FProcMeshTangent> Tangents;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FVector2D> UVs;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TArray<FLinearColor> Colors;
};



UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class USkirtingSegComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	FSkirtingSegNode& GetRelatedNode();

	UFUNCTION(BlueprintCallable, Category = "Construct")
	void UpdateMeshByNode();

	UFUNCTION(BlueprintCallable, Category = "Construct")
	void UpdateMeshByComponentNode(bool HasComponent = true);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ResetMesh(UModelFile *SkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ResetStaticMesh(UStaticMesh *StaticMeshType);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ChangeMaterial(UMaterialInterface *NewMaterial);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ChangeMatByID(const FString &ResMatID);

	FString GetMatID();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


protected:
// ue4 assert version
	void BuildOneSkirtingSeg(const FVector &StartPnt, const FVector &EndPnt, 
		const FVector &StartClippedPlaneDir, const FVector &EndClippedPlaneDir,
		FTransform &SkirtingTransform, UStaticMesh *InMeshType, float FullSkirtingLength,
		float SkirtingMeshLength, const FVector &Start2EndNormal);

	void AddProceduralMesh(UStaticMesh *InMesh, const FTransform &InTransform, 
		const FVector &StartPos, const FVector &StartClippedPlaneDir,
		const FVector &EndPos, const FVector &EndClippedPlaneDir);

	void AddProceduralMeshByStaticMeshInfo(UStaticMeshInfoExtract *InStaticMeshInfo, 
		const TArray<FVector> &PntsOfStaticMesh, const FTransform &Transform);

	void AddStaticMesh(UStaticMesh *InMesh, const FTransform &InTransform);

protected:
	// mx file version
	void BuildOneSkirtingSegByMX(const FVector &StartPnt, const FVector &EndPnt,
		const FVector &StartClippedPlaneDir, const FVector &EndClippedPlaneDir,
		FTransform &SkirtingTransform, UModelFile *InMeshType, float FullSkirtingLength,
		float SkirtingMeshLength, const FVector &Start2EndNormal);

	void AddProceduralMeshByMX(UModelFile *InMesh, const FTransform &InTransform,
		const FVector &StartPos, const FVector &StartClippedPlaneDir,
		const FVector &EndPos, const FVector &EndClippedPlaneDir);

	void AddStaticMeshByMX(UModelFile *InMesh, const FTransform &InTransform);

private:
	friend class USkirtingMeshComponent;
	void DoClearWork();

private:
	UPROPERTY(EditAnywhere)
	TArray<UProceduralMeshComponent*>	ProceduralMeshs;
	UPROPERTY(EditAnywhere)
	TArray<UModelFileComponent*>		StaticModelFileComps;
	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent*>		StaticMeshComps;
	UPROPERTY(Transient)
	UModelFile							*InnerStaticMeshType;
	UPROPERTY(Transient)
	UStaticMesh							*InnerMeshType;
	UPROPERTY(Transient)
	FSkirtingSegNode					SkirtingSegNode;
};



UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class UMontageMeshComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "MontageMesh") /* �����?��ʱ������ */
	void AddPolgonMesh(const TArray<FVector> &PolygonLine, TSubclassOf<UWallLine> WallLineType, float InnerOffset, bool bFloor = true, bool bCW = true, bool bClosed = false);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
protected:
	void GetCCW(TArray<uint8> &bCCW, const TArray<FVector> &PolygonLine);
	void DoAddPolgonMesh(const FVector &PlaneNormal, const TArray<FVector> &PolygonLine, int32 iStart, int32 Num, UWallLine *WallLine, bool bCW, bool bClosed);
	void GetMeshLength(UWallLine *WallLine, float &MeshLength, float &Radius);
	FVector GetSplitPlane(const FVector &StartDir, const FVector &EndDir, const FVector &PlaneDir);
	void SortPolygonLine(TArray<FVector> &SoredPolyonLine, const TArray<FVector> &PolygonLine, float InnerOffset, bool bCW, bool bClose);
	void CreateSegment(const FVector &StartPlaneDir, const FVector &EndPlaneDir, const FVector &PolygonPlaneNormal, const FVector &StartPos, const FVector &EndPos, UWallLine *WallLine, float MeshLength, float BackOffset, float ForwardOffset, bool bCW, bool bFlipEnd);
	void PlaneClipVertex(TArray<FVector> &WordPositions, const FVector &BasePos, const FVector &Normal, const FVector &Forward);
	void GetMeshPositions(const FTransform & Transform, TArray<FVector> &WordPositions, UWallLine *WallLine);
	void AddProceduralMesh(const FTransform &Tranform, TArray<FVector> &WordPositions, UWallLine *WallLine);
	void AddStaticMesh(const FTransform &Transform, UStaticMesh *Mesh);
	void RemoveAuxPoint(TArray<FVector> &NewPoints, const TArray<FVector> &PolygonLine);
protected:
	UPROPERTY(Transient)
	TArray<UStaticMeshComponent *>		StaticMeshComponents;
	UPROPERTY(Transient)
	TArray<UProceduralMeshComponent*>	ProceduralMeshComponents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MontageMesh")
	uint8 bDrawDebug:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MontageMesh")
	uint8 bDrawMesh : 1;
};



UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class USkirtingMeshComponent : public UMontageMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	FSkirtingNode& GetSkirtingSaveNode();

	UFUNCTION(BlueprintCallable, Category = "Construct")
	void UpdateMeshByNode();

	UFUNCTION(BlueprintCallable, Category = "Construct")
	void UpdateMeshByNodes(bool IsComponent = true);

	UFUNCTION(BlueprintCallable, Category = "Construct")
	void BuildFromPnts(const TArray<FVector> &InPolylinePnts, ECgHeadingDirection InHeadingDir, bool bInClosed,
		UModelFile *InDefaultType = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void SetOrientation(ECgHeadingDirection InHeadingDir);

	UFUNCTION(BlueprintCallable, Category = "Build")
	bool AddHeadPnt(const FVector &HeadPnt, ECgHeadingDirection InHeadingDir);

	UFUNCTION(BlueprintCallable, Category = "Build")
	bool AddTailPnt(const FVector &TailPnt, ECgHeadingDirection InHeadingDir);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void SetClosed(bool bInClose = true);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void MovePntByIndex(const FVector2D &NewPosition, int32 PntIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ResetMeshType(UModelFile *InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ResetStaticMeshType(UStaticMesh *InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ChangeMaterial(UMaterialInterface *NewMaterial);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ResetMeshByID(const FString &ResModelID);
	
	UFUNCTION(BlueprintCallable, Category = "Setting")
	void ChangeMatByID(const FString &ResMatID);

	UFUNCTION(BlueprintCallable, Category = "Setting")
	void DeleteSkirtingSegByIndices(const TArray<int32> &SkirtingIndices2Del);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	ESkirtingSnapType GetPntSnap(const FVector2D &SnapPnt, ESkirtingType InSkirtingType) const;

	UFUNCTION(BlueprintPure, Category = "Judge")
	bool DoSnapFirstPnt(const FVector2D &Pnt2Judge);

	UFUNCTION(BlueprintPure, Category = "Property")
	TArray<FVector2D> GetPathPnts() const;

	UFUNCTION(BlueprintCallable, Category = "Setting")
	UModelFile* GetModelFile() {
		return InnerStaticMeshType;
	};
private:
	void DoClearWork();

	void ClearMaterialOnSkirtingMesh();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<USkirtingSegComponent*>	SkirtingSegComponents;

	UPROPERTY(BlueprintReadWrite)
	FSkirtingNode SkirtingDataNode;

	UPROPERTY(Transient)
	UModelFile							*InnerStaticMeshType;
	UPROPERTY(Transient)
	UStaticMesh							*InnerMeshType;

	bool bHasASeg;
};


