// geometry config node
#pragma once
#include "ProceduralMeshComponent.h"
#include "GeometryConfig.generated.h"

// geometry config
USTRUCT(Blueprintable)
struct FGeometryConfigCPP
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Vertex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Triangles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> Normals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> UV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FProcMeshTangent> Tangents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MatPath;
};