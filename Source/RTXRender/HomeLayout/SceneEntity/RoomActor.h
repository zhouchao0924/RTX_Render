// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "../DataNodes/SkirtingNode.h"
#include "GameFramework/Actor.h"
#include "../DataNodes/MaterialNode.h"
#include "../../Struct/Struct.h"
#include "CGALWrapper/PolygonAlg.h"
#include "Building/DRActor.h"
#include "Building/BuildingComponent.h"
#include "Building/DRStruct.h"
#include "Blueprint/UserWidget.h"
#include "DataType.h"
#include "RoomActor.generated.h"

class USkirtingSegComponent;
class UBuildingData;

UCLASS()
class RTXRENDER_API ARoomActor : public ADRActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void InitRoomActor(EDR_ObjectType ObjectType, UBuildingData* BuildingData);
	void InitRoomActor_Implementation(EDR_ObjectType ObjectType, UBuildingData* BuildingData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void OnInitFinished();
	void OnInitFinished_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void OnInitFloorPlane(UBuildingData* BuildingData);
	void OnInitFloorPlane_Implementation(UBuildingData* BuildingData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void OnInitCeilPlane(UBuildingData* BuildingData);
	void OnInitCeilPlane_Implementation(UBuildingData* BuildingData);

	UFUNCTION(BlueprintCallable, Category = "Judge")
	bool DoesSegmentBelong2Room(const FVector2D &Start, const FVector2D &End);

	float GetSegmentDist2Room(const FVector2D &Start, const FVector2D &End);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
	float GetRoomCeilingHeight() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
	FRoomPath GetRoomPath();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Property"))
	FMaterialNode GetGroundMaterial() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Property"))
	TArray<FMaterialNode> GetRoofMaterial() const;

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void ComputePolygonS(const TArray<FVector> & PointList, float & OutS);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void SHeronFormula(FVector  A, FVector  B, FVector  C,float & Out);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static float IsPolyClockWiseAB(FVector2D  A, FVector2D  O, FVector2D  B, bool isNormalize = false);

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		bool IsSimplePolygon2D(const TArray<FVector2D> & Polygon);

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void ChangeClockwise(TArray<FVector> Polygons, TArray<FVector> & Out);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void ComputePolygon(const TArray<FVector> & PointList, bool isDir, bool & Polygon);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void PolygonMaxPoint(const TArray<FVector> & PointList,FVector &Max);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		void PolygonMinPoint(const TArray<FVector> & PointList, FVector &Min);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		bool PointPolygonIntersection(const TArray<FVector> & PointList, const FVector &Point);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static float PointToLineLegth(FVector A, FVector B, FVector Point);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static bool PointOnLine(FVector Point,FVector LineA, FVector LineB, FVector & FindPos,float Error = -1.f);

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static bool LineInLine(FVector LineAa, FVector LineAb, FVector LineBa, FVector LineBb);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static bool LineInLine2D(FVector2D LineAa, FVector2D LineAb, FVector2D LineBa, FVector2D LineBb);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static bool isLineInLine(FVector LineAa, FVector LineAb, FVector LineBa, FVector LineBb);
	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static void PolygonsUnion(const TArray<FVector2D>& PolygonA,
			const TArray<FVector2D>& PolygonB,
			TArray<FVector2D>& OutPolygonsA,
			TArray<FVector2D>& OutPolygonsB,
			TArray<FVector2D>& Polygons,
			TArray<FVector2D>& Intersection,
			const float Error);

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static void MergePolygonPoints(const TArray<FVector2D>& PolygonA,
			const TArray<FVector2D>& PolygonB,
			TArray<FVector2D>& OutPolygonsA,
			TArray<FVector2D>& OutPolygonsB,
			const float DisBound);

	UFUNCTION(BlueprintCallable, Category = "ComputePolygon")
		static bool PolygonsUnionFunction(const TArray<FVector2D>& PolygonA,
			const TArray<FVector2D>& PolygonB,
			TArray<FVector2D>& OutPolygonsA,
			TArray<FVector2D>& OutPolygonsB,
			TArray<FVector2D>& Polygons);

	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		void CreatePointLightMatrix(const TArray<FVector> & VertexList, float  Spacing, float Hight, float DelRadius);
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		void DrawPointLightMatrix(FColor  Color, float  Size, float  LifeTime, FVector Restrict = FVector(-1.f, -1.f, -1.f));
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		void GetPointLightMatrix(TArray<FVector> & Point);
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		void CreatePointLight(bool bShadows, float Intensity, float Radius, float MinRoughness,float Hight);
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		void SpotProcessing(APointLight *Spot);
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		static bool LocateCenter(const TArray<FVector> InPnts, FVector2D& BetterCenter);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "SktringArea"))
		void SktringInArea();
	UFUNCTION(BlueprintCallable, Category = "Math")
		static void FinishingPolygons(TArray<FVector2D>& Polygons);
	UFUNCTION(BlueprintCallable, Category = "Math")
		static bool isPointInPolygons(const TArray<FVector>& Polygons,const FVector & Point, float Length);
	UFUNCTION(BlueprintCallable, Category = "Math")
		static bool isPointInPolygons2D(const TArray<FVector2D>& Polygons, const FVector2D & Point2D, float Length);

	UFUNCTION(BlueprintCallable, Category = "Math")
		static void PolygonsScale(const TArray<FVector>& Polygons, TArray<FVector>& OutPolygon);
	UFUNCTION(BlueprintCallable, Category = "Math")
		static void PolygonsScale2D(const TArray<FVector2D>& Polygons, TArray<FVector2D>& OutPolygon,float Scale);
	UFUNCTION(BlueprintCallable, Category = "Math")
		float GetRoomAcreage();
	UFUNCTION(BlueprintCallable, Category = "Area")
		bool GetDependsWalls(TArray<ALineWallActor*>& outDependsWalls);
	UFUNCTION(BlueprintCallable, Category = "PointLightMatrix")
		int32 GetRoomDataID();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		FAreaDepends AreaDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FWallDependsArea> WallDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FWindowDependsArea> WindowDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FDoorDependsArea> DoorDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FDoorPocketDependsArea> DoorPocketDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FThresholdDependsArea> ThresholdDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FSkirtingDependsArea> SkirtingDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FSkirtingDependsArea> SkirtingTopLineDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<FHardRiftModelDependsArea> HardRiftModelDepends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<APointLight*> PointLightList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		TArray<USkirtingSegComponent*>	AreaSkirting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		FRoom RoomFloorStruct;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		FRoom RoomCeillStruct;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		UBuildingComponent * PGround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		UBuildingComponent * Roof;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		UBuildingComponent * LitMesh;

		void UpdateBuildingData(UBuildingData *Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "SaveData")
		void BPUpdateBuildingData(UBuildingData *Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "SaveData")
		void BPCreateBuildingData(UBuildingData *Data);
	UFUNCTION(BlueprintCallable)
		void UpdateRoof();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Property"))
		bool GetAreaVerList(TArray<FVector2D>&Slots);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetName();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetUseIndex();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void AddRoom(const  TArray<ARoomActor*> & List, int32 Index, TArray<ARoomActor*> & res);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<class AComponentManagerActor*> GetHousePlugin(const TArray<AActor*>&Actors);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Property"))
		bool GetAreaVer3DList(TArray<FVector>&Slots);
private:

	bool isUpdate;
	TArray<TSharedPtr<TArray<FVector>>> LightMatrix;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Area"))
		class UWidgetComponent* Widget;
	UPROPERTY(BlueprintReadWrite, meta = (Category = "Area"))
		FString RoomUseName;
	UPROPERTY(BlueprintReadWrite, meta = (Category = "Area"))
		int32 RoomUseIndex;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Area")
		int32 ObjectID;
};


USTRUCT(BlueprintType)
struct RTXRENDER_API FRoomList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		TArray<ARoomActor*> List;

	void AddInitRoom(int32 Index, FString & Name,ARoomActor * Room)
	{
		if (Room)
		{
			if (Index > List.Num())
			{
				int32 _Count = Index - List.Num();
				List.AddZeroed(_Count);
			}

			if (Index == 0 && List.Num() == 0)
			{
				List.Add(Room);
				Room->RoomUseName = Name;
				Room->RoomUseIndex = 1;
				Room->SetName();
			}
			else
			{
				int32 _CurIndex = Index - 1;

				if (_CurIndex < 0)
					_CurIndex = 0;

				if (_CurIndex == List.Num())
				{
					List.Add(Room);
				}				
				else if (!List[_CurIndex])
				{
					List[_CurIndex] = Room;
					if (List.Num() == 1)
					{
						Room->RoomUseName = Name;
						Room->RoomUseIndex = Index;
						Room->SetName();
					}
				}				
				else
				{
					bool _isSuccess = false;
					for (int i = 0; i < List.Num(); ++i)
					{
						if (!List[i])
						{
							List[i] = Room;
							_isSuccess = true;
							break;
						}
					}
					if (!_isSuccess)
					{
						List.Add(Room);
					}
				}
			}
			if (List.Num() > 1)
				for (int i = 0; i < List.Num(); ++i)
				{
					if (List[i])
					{
						List[i]->RoomUseName = Name + FString(TEXT("-") + FString::FromInt(i + 1));
						List[i]->RoomUseIndex = i + 1;
						List[i]->SetName();
					}
					
				}		
		}
		//if (Room)
		//{
		//	if (List.Find(Room) != INDEX_NONE) return;
		//	Room->RoomUseIndex = Index;
		//	
		//	if (List.Num() == 0)
		//	{
		//		Room->RoomUseName = Name;
		//		Room->AddRoom(List, Index, List);
		//	}
		//	else if(List.Num() == 1)
		//	{
		//		ARoomActor* _ra = List[0];
		//		if (_ra)
		//		{
		//			_ra->RoomUseName = _ra->RoomUseName + FString(TEXT("-")) + FString::FromInt(1);
		//			_ra->SetName();
		//		}
		//		Room->RoomUseName = Name + FString(TEXT("-")) + FString::FromInt(Index + 1);
		//		Room->AddRoom(List, Index, List);
		//	}
		//	else
		//	{
		//		Room->RoomUseName = Name + FString(TEXT("-")) + FString::FromInt(Index + 1);
		//		Room->AddRoom(List, Index, List);
		//	}
		//	Room->SetName();
		//}
	}

	void AddRoom(FString & Name ,ARoomActor * Room)
	{
		if (Room)
		{
			if (List.Find(Room) != INDEX_NONE) return;
			if (List.Num() == 0)
			{
				Room->RoomUseIndex = List.Num();
				Room->RoomUseName = Name;// + FString(TEXT("_")) + FString::FromInt(List.Num() + 1);
				List.AddUnique(Room);
			}
			else if (List.Num() == 1)
			{
				ARoomActor* _ra = List[0];
				if (_ra)
				{
					_ra->RoomUseName = _ra->RoomUseName + FString(TEXT("-")) + FString::FromInt(1);
					_ra->SetName();
				}
				Room->RoomUseIndex = List.Num();
				Room->RoomUseName = Name + FString(TEXT("-")) + FString::FromInt(List.Num() + 1);
				List.AddUnique(Room);
			}
			else
			{
				Room->RoomUseIndex = List.Num();
				Room->RoomUseName = Name + FString(TEXT("-")) + FString::FromInt(List.Num() + 1);
				List.AddUnique(Room);
			}
			Room->SetName();
		}
	}

	void DelRoom(ARoomActor * Room)
	{
		if (Room)
		{
			int32 _Index = List.Find(Room);
			if (_Index < 0 || List.Num() == 0) return;
			if (List.Num() == 1)
			{
				List.RemoveSwap(Room);
			}
			else if (List.Num() == 2)
			{
				List.RemoveSwap(Room);
				ARoomActor* _ra = List[0];
				if (_ra)
				{
					FString & _Name = _ra->RoomUseName;
					for (int j = _Name.Len() - 1; j >= 0; --j)
					{
						if (_Name[j] == L'-')
						{
							_Name.RemoveAt(j);
							break;
						};
						_Name.RemoveAt(j);
					}
					_ra->RoomUseIndex = 1;
					_ra->SetName();
				}
			}
			else
			{
				int32 _Count = 0;
				List.RemoveSwap(Room);
				for (int i = _Index; i < List.Num(); ++i)
				{
					ARoomActor * _t = List[i];
					if (_t)
					{
						FString & _Name = _t->RoomUseName;
						for (int j = _Name.Len() - 1; j >= 0; --j)
						{
							if (_Name[j] == L'-') break;
							_Name.RemoveAt(j);
						}
						_Name += FString::FromInt(i + 1);
						_t->RoomUseIndex = i;
						_t->RoomUseName = _Name;
						_t->SetName();
					}
				}
			}

		}
	}

};