 
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProcMeshFunctionLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "SolidGeometryFunctionLibrary.generated.h"




USTRUCT(BlueprintType)
struct FOnceGeometry
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FVector Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int32 Triangles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FVector Normals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FVector2D UV0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FProcMeshTangent Tangents;
};


UCLASS()
class AJDR_API USolidGeometryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "SolidGeometry", meta = (WorldContext = "WorldContextObject", DevelopmentOnly))
		static void TwoLineMinDistance(UObject* WorldContextObject, FVector Line1Point, FVector Line1Direction, FVector Line2Point, FVector Line2Direction, FVector &out_PointOfIntersection, FVector &out_1To2Direction, float &out_Distance,bool Debug);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry", meta = (WorldContext = "WorldContextObject", DevelopmentOnly))
		static void TwoLineMinDistance_StraightLine(UObject* WorldContextObject, FVector Line1StartPoint, FVector Line1EndPoint, FVector Line2StartPoint, FVector Line2EndPoint, FVector &out_PointOfIntersection, FVector &out_1To2Direction, float &out_Distance, bool Debug);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry", meta = (WorldContext = "WorldContextObject", DevelopmentOnly))
		static  FVector GetFourPointCenter(UObject* WorldContextObject, FVector Point1, FVector Point2, FVector Point3, FVector Point4, bool Debug);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry")
		static FVector SurfaceNormal(USceneComponent* Ver1, USceneComponent* Ver2, USceneComponent* Ver3);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry")
		static FVector SurfaceNormal_Location(FVector Ver1, FVector Ver2, FVector Ver3);

	UFUNCTION(BlueprintCallable, Category = "SolidGeometry", meta = (WorldContext = "WorldContextObject", DevelopmentOnly))
		static void spawnbrush(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry")
    	static bool TriangulatePoly(TArray<int32>& OutTris, const TArray<FOnceGeometry>& PolyVertList, int32 VertBase, const FVector& PolyNormal);

	UFUNCTION(BlueprintCallable, Category = "SolidGeometry", meta = (WorldContext = "WorldContextObject", DevelopmentOnly))
		static void SliceMesh(UObject* WorldContextObject, FBlueprintBox Box, FProcMeshConfig InConfig, FVector PlanePosition, FVector PlaneNormal,
			bool bCreateOtherHalf, bool bCreateCap, FProcMeshConfig& OutBaseConfig, bool& BaseValid, FProcMeshConfig& OutOtherConfig, bool& OtherValid, FProcMeshConfig& OutCapConfig);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry")
		static FBlueprintBox GetProcMeshBox(int32 Index, UProceduralMeshComponent* ProceduralMesh);

	UFUNCTION(BlueprintPure, Category = "SolidGeometry")
		static void TransformToPlane(const FTransform& ActorLocation, FVector Location, FRotator Rotation, FVector& OutLocation, FVector& OutNormal);

	UFUNCTION(BlueprintCallable, Category = "SolidGeometry")
		static 	UProceduralMeshComponent* AddProceduralMeshComponent(AActor* Actor);

	static void TransformPnts(TArray<FVector> &OutTransformedPnts, const TArray<FVector> &InPnts, const FTransform &Transform);

	static void ClipPntsByPlaneAndDirection(TArray<FVector> &OutClippedPnts, const TArray<FVector> &InPnts, 
		const FVector &PntOnPlane, const FVector &NormalOfPlane,
		const FVector &Direction2Project);
};
