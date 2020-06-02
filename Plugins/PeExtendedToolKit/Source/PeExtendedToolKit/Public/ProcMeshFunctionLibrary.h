// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshFunctionLibrary.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct FProcMeshConfig
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		TArray<FVector> Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		TArray<int32> Triangles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		TArray<FVector> Normals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		TArray<FVector2D> UV0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		TArray<FProcMeshTangent> Tangents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FString MaterialName;

};
USTRUCT(BlueprintType)
struct FBlueprintBox
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	FVector Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	FVector Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	uint8 IsValid;

};
/**
 * 
 */
UCLASS()
class PEEXTENDEDTOOLKIT_API UProcMeshFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//�ϲ�ProceduralMesh
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ProceduralMesh")
		static void MergeProceduralMesh(UProceduralMeshComponent* ProceduralMesh, UProceduralMeshComponent* Target, FVector Offset, bool CreateCollision);
	//��ȡProceduralMesh Section
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ProceduralMesh")
		static FProcMeshConfig GetProcMeshSection(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex);
	//�ϲ�ProceduralMesh����
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ProceduralMesh")
		static FProcMeshConfig MergeProceduralMeshConfig(FProcMeshConfig Target, FProcMeshConfig Parameter, FVector Offset);

	//��ȡProceduralMesh�Ķ��㡢���ߡ�UV�������б�
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ProceduralMesh")
		static void GetProceduralMeshVertexList(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents, bool& bCreateCollision, bool& SectionVisible);
	//��ȡProceduralMesh��ָ�����㡢���ߡ�UV������ ��Ϣ
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|ProceduralMesh")
		static void GetProceduralMeshVertexIndex(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index, FVector &Vertices, FVector& Normals, FVector2D& UV0, FColor& VertexColors, FProcMeshTangent& Tangents, bool& EnableCollision, bool& SectionVisible);
	//��ȡProceduralMesh��ָ����������Ϣ
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|ProceduralMesh")
		static int32 GetProceduralMeshTriangles(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index);
	//��ȡProceduralMesh�Ķ�������
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|ProceduralMesh")
		static int32 GetProceduralMeshNum_Vertice(UProceduralMeshComponent* ProcMesh, int32 SectionIndex);
	//��ȡProceduralMesh������������
	UFUNCTION(BlueprintPure, Category = "ExtendedContent|ProceduralMesh")
		static int32 GetProceduralMeshNum_Triangles(UProceduralMeshComponent* ProcMesh, int32 SectionIndex);

	
	UFUNCTION(BlueprintCallable, Category = "ExtendedContent|ProceduralMesh")
		static TArray<FProcMeshConfig> InputObjMode(FString Path, bool MirrorZ);

	UFUNCTION(BlueprintPure, Category = "ExtendedContent|String")
		static int32 SpecifiedCharacterNumber(FString InString, FString Character);


	UFUNCTION(BlueprintPure, Category = "ExtendedContent|ProceduralMesh")
		static bool ObjModeIsTriangular(FString Path);

	
};
