// Copyright© 2017 ihomefnt All Rights Reserved.


#include "RoomActor.h"
#include "../HouseArchitect/AreaSystem.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "EditorGameInstance.h"
#include "MontageMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "SolidGeometry/SolidGeometryFunctionLibrary.h"
#include "Building/HouseComponent/ComponentManagerActor.h"
#include "Building/BuildingData.h"
#include "Engine/PointLight.h"

// Sets default values
ARoomActor::ARoomActor()
	: Roof(nullptr)
	, PGround(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));
	LitMesh = CreateDefaultSubobject<UBuildingComponent>(TEXT("LitMesh"));
	LitMesh->SetupAttachment(RootComponent);
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARoomActor::BeginPlay()
{
	Super::BeginPlay();

	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (GameInst)
	{
		if (!GameInst->AreaSystem)
		{
			GameInst->AreaSystem = GetWorld()->SpawnActor<AAreaSystem>();
		}
		GameInst->AreaSystem->OnRegionActorSpawned(this);
	}
}

void ARoomActor::Destroyed()
{
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (GameInst)
	{
		GameInst->AreaSystem->OnRegionActorDestroyed(this);
	}

	Super::Destroyed();
}

// Called every frame
void ARoomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomActor::InitRoomActor_Implementation(EDR_ObjectType ObjectType, UBuildingData* BuildingData)
{
	if (BuildingData == nullptr) {
		return;
	}

	switch (ObjectType) {
	case EDR_ObjectType::EDR_CeilPlane:
		OnInitCeilPlane(BuildingData);
		break;
	case EDR_ObjectType::EDR_FloorPlane:
		OnInitFloorPlane(BuildingData);
		break;
	case EDR_ObjectType::EDR_Room:
	{
		if (PGround != nullptr && Roof != nullptr) {
			OnInitFinished();
		}	
		break;
	}
	}
}

void ARoomActor::OnInitFinished_Implementation()
{
}

void ARoomActor::OnInitFloorPlane_Implementation(UBuildingData* BuildingData)
{
	if (PGround == nullptr) {
		PGround = NewObject<UBuildingComponent>(this);
		PGround->ComponentTags.Add(TEXT("PGround"));
		PGround->RegisterComponent();
		PGround->AttachTo(RootComponent, NAME_None, EAttachLocation::KeepWorldPosition, true);
		PGround->SetData(BuildingData);
		PGround->SetCollisionProfileName(TEXT("BlockAll"));
	}

	if (Roof != nullptr) {
		OnInitFinished();
	}
}

void ARoomActor::OnInitCeilPlane_Implementation(UBuildingData* BuildingData)
{
	if (Roof == nullptr) {
		Roof = NewObject<UBuildingComponent>(this);
		Roof->ComponentTags.Add(TEXT("Roof"));
		Roof->RegisterComponent();
		Roof->AttachTo(RootComponent, NAME_None, EAttachLocation::KeepWorldPosition, true);
		Roof->SetData(BuildingData);
		Roof->SetCollisionProfileName(TEXT("BlockAll"));
	}

	if (PGround != nullptr) {
		OnInitFinished();
	}
}

bool ARoomActor::DoesSegmentBelong2Room(const FVector2D &Start, const FVector2D &End)
{
	auto RoomPath = GetRoomPath();
	return ((ULinearEntityLibrary::IsPointInPolygon(Start, RoomPath.InnerRoomPath) != ECgPolyBoundedSide::EON_UNBOUNDED) && 
			(ULinearEntityLibrary::IsPointInPolygon(End, RoomPath.InnerRoomPath) != ECgPolyBoundedSide::EON_UNBOUNDED));
}

float ARoomActor::GetSegmentDist2Room(const FVector2D &Start, const FVector2D &End)
{
	FRoomPath RoomPath = GetRoomPath();
	double StartPntDist = (double)ULinearEntityLibrary::GetSquaredPointDist2Polygon(Start, RoomPath.InnerRoomPath);
	StartPntDist = std::sqrt(StartPntDist);
	double EndPntDist = (double)ULinearEntityLibrary::GetSquaredPointDist2Polygon(End, RoomPath.InnerRoomPath);
	EndPntDist = std::sqrt(EndPntDist);
	double SegmentDist = StartPntDist + EndPntDist;

	return (float)SegmentDist;
}

void  ARoomActor::ComputePolygonS(const TArray<FVector> & PointList,float & OutS)
{
	int num = PointList.Num();
	if (num < 3) OutS = 0.f;
	float Sum = 0.f;
	for (int i = 1; i < num-1; ++i)
	{
		float S = 0.f;
		SHeronFormula(PointList[0], PointList[(i) % num], PointList[(i + 1) % num], S);
		Sum += S;
	}
	OutS = Sum;
}

void ARoomActor::SHeronFormula(FVector  A, FVector  B, FVector  C, float & Out)
{
	float  a = FVector(A - B).Size2D();
	float  b = FVector(B - C).Size2D();
	float  c = FVector(C - A).Size2D();
	float p = (a+b+c) / 2.f;

	Out = FMath::Sqrt((p * (p - a) * (p - b) * (p - c)));
}

float ARoomActor::IsPolyClockWiseAB( FVector2D  A,  FVector2D  O, FVector2D  B, bool isNormalize)
{
	float out = 0.f;
	FVector2D OA = A - O;
	FVector2D OB = B - O;
	if (isNormalize)
	{
		OA.Normalize();
		OB.Normalize();
	}
	out = OA.X*OB.Y - OB.X*OA.Y;
	return out;
}

bool ARoomActor::IsSimplePolygon2D(const TArray<FVector2D>& Polygon)
{
	int32 count = 0;
	for (int i = 1; i < Polygon.Num(); i+=2)
	{
		//UE_LOG(LogTemp,Warning,TEXT("============%d"),i);
		if (IsPolyClockWiseAB(Polygon[i - 1], Polygon[i], Polygon[(i + 1) % Polygon.Num()]) > 0)
			count += 1;
		else
			count -= 1;
	}
	if (count > 0)
		return true;
	return false;
}

void ARoomActor::ChangeClockwise(TArray<FVector> Polygons,TArray<FVector> & Out)
{
	Out.Add(Polygons[0]);
	for (int i = Polygons.Num() - 1; i > 0; --i)
	{
		Out.Add(Polygons[i]);
	}
}

void ARoomActor::ComputePolygon(const TArray<FVector>& PointList,bool isDir, bool & Polygon)
{
	int _index = 0;
	int index = 0;
	for (int i = 0; PointList.Num(); ++i)
	{
		_index = i - 1;
		if (_index < 0)
			_index = PointList.Num()-1;
		if (isDir)
		{
			if (IsPolyClockWiseAB(FVector2D(PointList[_index].X, PointList[_index].Y),
				FVector2D(PointList[i].X, PointList[i].Y),
				FVector2D(PointList[(i + 1) % PointList.Num()].X, PointList[(i + 1) % PointList.Num()].Y)) > 0.f)
			{
				Polygon = false;
				return;
			}
		}
		else
		{
			if (IsPolyClockWiseAB(FVector2D(PointList[_index].X, PointList[_index].Y),
				FVector2D(PointList[i].X, PointList[i].Y),
				FVector2D(PointList[(i + 1) % PointList.Num()].X, PointList[(i + 1) % PointList.Num()].Y)) < 0.f)
			{
				Polygon = false;
				return;
			}
		}
	}
	Polygon = true;
}

void ARoomActor::PolygonMaxPoint(const TArray<FVector>& PointList, FVector & Max)
{
	Max.X = PointList[0].X;
	Max.Y = PointList[0].Y;
	Max.Z = PointList[0].Z;

	for (int i = 1; i < PointList.Num(); ++i)
	{
		if (PointList[i].X > Max.X)
			Max.X = PointList[i].X;
		if (PointList[i].Y > Max.Y)
			Max.Y = PointList[i].Y;
		if (PointList[i].Z > Max.Z)
			Max.Z = PointList[i].Z;
	}
}

void ARoomActor::PolygonMinPoint(const TArray<FVector>& PointList, FVector & Min)
{
	Min.X = PointList[0].X;
	Min.Y = PointList[0].Y;
	Min.Z = PointList[0].Z;

 	for (int i = 1; i < PointList.Num(); ++i)
	{
		if(PointList[i].X < Min.X)
			Min.X = PointList[i].X;
		if (PointList[i].Y < Min.Y)
			Min.Y = PointList[i].Y;
		if (PointList[i].Z < Min.Z)
			Min.Z = PointList[i].Z;
	}
}

bool ARoomActor::PointPolygonIntersection(const TArray<FVector>& PointList, const FVector & Point)
{
	return false;
}
float ARoomActor::PointToLineLegth(FVector A, FVector B, FVector Point)
{
	double cross = (B.X - A.X) * (Point.X - A.X) + (B.Y - A.Y) * (Point.Y-A.Y);
	if (cross <= 0)
		return FMath::Sqrt((Point.X-A.X) * (Point.X - A.X) + (Point.Y - A.Y) * (Point.Y - A.Y) + 0.f);
	double d2 = (B.X - A.X)*(B.X - A.X) + (B.Y - A.Y)*(B.Y - A.Y);
	if (cross >= d2)
		return FMath::Sqrt((Point.X-B.X)*(Point.X-B.X)+(Point.Y - B.Y)*(Point.Y+B.Y)+0.f);
	double r = cross / d2;
	double px = A.X + (B.X - A.X)*r;
	double py = A.Y + (B.Y - A.Y)*r;
	return FMath::Sqrt((Point.X - px) * (Point.X-px) + (Point.Y - py) * (Point.Y-py)+0.f);
}

void ARoomActor::CreatePointLightMatrix(const TArray<FVector>& VertexList, float  Spacing, float Hight, float DelRadius)
{
	FVector Max;
	FVector Min;

	for (int i = 0; i < LightMatrix.Num(); i++)
	{
		LightMatrix[i].Reset();
	}
	LightMatrix.Empty();

	PolygonMaxPoint(VertexList, Max);
	PolygonMinPoint(VertexList, Min);

	//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Max, 50.f, FColor(255, 255, 0, 255), 1000.f);
	//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Min, 50.f, FColor(255, 0, 255, 255), 1000.f);

	int HorNum = FMath::Abs((Max.X - Min.X) / Spacing);
	int VerNum = FMath::Abs((Max.Y - Min.Y) / Spacing);

	TArray<TEnumAsByte<EObjectTypeQuery>> SphereTrace;
	SphereTrace.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> IgnoreActor;

	TArray<FHitResult> OutHit;
	bool is =false;
	FVector BeginPoint(Max.X,Min.Y,0.f);
	for (int i = 0; i < VerNum; ++i)//Y
	{
		TSharedPtr<TArray<FVector>> Count = MakeShareable(new TArray<FVector>);
		for (int j = 0; j < HorNum; ++j)//X
		{
			FVector Location(BeginPoint.X -((j+1)*Spacing),BeginPoint.Y+((i+1)*Spacing), 0.f);

			UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Location, Location,5.f, SphereTrace,false, IgnoreActor,EDrawDebugTrace::None, OutHit,false);
			for (int index = 0; index < OutHit.Num(); ++index)
			{
				if (OutHit[index].Actor == this)
				{
					is = true;
					break;
				}
				
			}
			if (is)
			{
				Count->Add(Location);
				is = false;
			}
			else
			{
				Count->Add(FVector::ZeroVector);
				is = false;
			}
		}

		LightMatrix.Add(Count);
	}
	//-2 有值 -1 无值
	int Top = 0;
	int Bottom = 0;
	int Left = 0;
	int Right = 0;
	for (int i = 0; i < LightMatrix.Num(); ++i)
	{
		for (int j = 0; j < LightMatrix[i]->Num(); ++j)
		{

			if ((*LightMatrix[i])[j] == FVector::ZeroVector)
			continue;
			if ((*LightMatrix[i])[j] != FVector::ZeroVector)
			{
				//Top
				Top = i - 1;
				if (Top >= 0 && (*LightMatrix[Top]).Num() > j)
					if ((*LightMatrix[Top])[j] != FVector::ZeroVector)
						Top = -2;
					else
						Top = -1;
				else
					Top = -1;
				//Bottom
				Bottom = i + 1;
				if (Bottom < LightMatrix.Num() && (*LightMatrix[Bottom]).Num() > j)
					if ((*LightMatrix[Bottom])[j] != FVector::ZeroVector)
						Bottom = -2;
					else
						Bottom = -1;
				else
					Bottom  = - 1;
				//Left
				Left = j - 1;
				if (Left >= 0 && (*LightMatrix[i]).Num() > j)
				{
					if ((*LightMatrix[i])[Left] != FVector::ZeroVector)
						Left = -2;
					else
						Left = -1;
				}
				else
					Left = -1;
				//Right
				Right = j + 1;
				if (Right < LightMatrix[i]->Num() && (*LightMatrix[i]).Num() > j)
					if ((*LightMatrix[i])[Right] != FVector::ZeroVector)
						Right = -2;
					else
						Right = -1;
				else
					Right = -1;
			}
			if (VertexList.Num() > 6)
			{
				//上下
				if (Top == -1 && Bottom == -1)
				{
					(*LightMatrix[i])[j] = FVector::ZeroVector;
					//FVector Pos((*LightMatrix[i])[j]);
					//if (Pos != FVector::ZeroVector)
					//{
					//	Pos.Z = 100.f;
					//	UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Pos, 50, FColor(0, 255, 0, 255), 10000);
					//}
					//Pos.Z = 0.f;		
					//FVector BeginPos = FVector::ZeroVector;
					//FVector EndPos = FVector::ZeroVector;
					//
					//for (int i = 0; i < VertexList.Num(); ++i)
					//{
					//	FVector Top_PointOfIntersection;
					//	FVector Bottom_PointOfIntersection;
					//	FVector out_1To2Direction;
					//	float out_Distance;
					//	//Top
					//	USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X , Pos.Y + 200.f, 0.f), VertexList[i], VertexList[(i + 1) % VertexList.Num()], Top_PointOfIntersection, out_1To2Direction, out_Distance, false);
					//	if (BeginPos == FVector::ZeroVector && Top_PointOfIntersection != FVector::ZeroVector && out_1To2Direction != FVector::ZeroVector )
					//	{
					//		BeginPos = Top_PointOfIntersection;
					//	}
					//	//Bottom
					//	USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X , Pos.Y - 200.f, 0.f), VertexList[i], VertexList[(i + 1) % VertexList.Num()], Bottom_PointOfIntersection, out_1To2Direction, out_Distance, false);
					//	if (EndPos == FVector::ZeroVector && Bottom_PointOfIntersection != FVector::ZeroVector && out_1To2Direction != FVector::ZeroVector)
					//	{
					//		EndPos = Bottom_PointOfIntersection;
					//	}
					//}
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X, Pos.Y + 200.f, 0.f), FColor(255, 255, 0, 255), 1000, 10);
					//
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), BeginPos, 50, FColor(128, 128, 0, 255), 10000);
					//
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X, Pos.Y - 200.f, 0.f), FColor(0, 0, 255, 255), 1000, 10);
					//
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), EndPos, 50, FColor(0, 128, 128, 255), 10000);


				}
				//左右
				if (Left == -1 && Right == -1)
				{
					(*LightMatrix[i])[j] = FVector::ZeroVector;
					//FVector Pos((*LightMatrix[i])[j]);
					//if (Pos != FVector::ZeroVector)
					//{
					//	Pos.Z = 200.f;
					//	UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Pos, 50, FColor(0, 0, 255, 255), 10000);
					//}
					//
					//Pos.Z = 0.f;
					//FVector BeginPos = FVector::ZeroVector;
					//FVector EndPos = FVector::ZeroVector;
					//
					//for (int i = 0; i < VertexList.Num(); ++i)
					//{
					//	FVector Left_PointOfIntersection;
					//	FVector Right_PointOfIntersection;
					//	FVector out_1To2Direction;
					//	float out_Distance;
					//	//Left
					//	//USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X - 200.f, Pos.Y , 0.f), VertexList[i], VertexList[(i + 1) % VertexList.Num()], Left_PointOfIntersection, out_1To2Direction, out_Distance, false);
					//	//if (BeginPos == FVector::ZeroVector && Left_PointOfIntersection != FVector::ZeroVector && out_1To2Direction== FVector::ZeroVector)
					//	//{
					//	//	BeginPos = Left_PointOfIntersection;
					//	//	//continue;
					//	//}
					//	//Right
					//	USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), FVector(Pos.X + 200.f, Pos.Y, 0.f),Pos , VertexList[i], VertexList[(i + 1) % VertexList.Num()], Right_PointOfIntersection, out_1To2Direction, out_Distance, false);
					//	
					//	if (EndPos == FVector::ZeroVector 
					//		&& Right_PointOfIntersection != FVector::ZeroVector 
					//		&& out_1To2Direction == FVector::ZeroVector
					//		&& PointOnLine(Right_PointOfIntersection, Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f),0.f))
					//	{ 
					//		EndPos = Right_PointOfIntersection;
					//	}
					//
					//}
					////0
					////UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X - 200.f, Pos.Y, 0.f), FColor(0, 255, 0, 255), 1000, 10);
					////UKismetSystemLibrary::DrawDebugPoint(GetWorld(), BeginPos, 50, FColor(255, 128, 128, 255), 10000);
					//
					////1
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f), FColor(0, 255, 0, 255), 1000, 10);
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), EndPos, 50, FColor(128, 128, 255, 255), 10000);


				}
				
				//左值
				if (Left == -1 && Right == -2 && (*LightMatrix[i])[j] != FVector::ZeroVector)
				{
					FVector Pos = (*LightMatrix[i])[j];
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f), FColor(0, 255, 0, 255), 1000, 10);
					FVector Left_PointOfIntersection;
					FVector out_1To2Direction;
					float out_Distance;
					for (int index = 0; index < VertexList.Num(); ++index)
					{
						if (LineInLine(Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()])
							&& LineInLine(VertexList[index], VertexList[(index + 1) % VertexList.Num()], Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f)))
						{
							USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X + 200.f, Pos.Y, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()], Left_PointOfIntersection, out_1To2Direction, out_Distance, false);
						}
					}
					if (FVector(Pos - Left_PointOfIntersection).Size() < DelRadius/*(Spacing / 2)*/)
					{
						(*LightMatrix[i])[j] = FVector::ZeroVector;
					}
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Left_PointOfIntersection, 50, FColor(255, 128, 128, 255), 10000);
				}
				//右值
				if (Right == -1 && Left == -2 && (*LightMatrix[i])[j] != FVector::ZeroVector)
				{
					FVector Pos = (*LightMatrix[i])[j];
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X - 200.f, Pos.Y , 0.f), FColor(0,0 , 255, 255), 1000, 10);
					FVector Right_PointOfIntersection;
					FVector out_1To2Direction;
					float out_Distance;
					for (int index = 0; index < VertexList.Num(); ++index)
					{
						if (LineInLine(Pos, FVector(Pos.X - 200.f, Pos.Y, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()])
							&& LineInLine(VertexList[index], VertexList[(index + 1) % VertexList.Num()], Pos, FVector(Pos.X - 200.f, Pos.Y, 0.f)))
							USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X - 200.f, Pos.Y, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()], Right_PointOfIntersection, out_1To2Direction, out_Distance, false);
					}
					if (FVector(Pos - Right_PointOfIntersection).Size() < DelRadius/*(Spacing / 2)*/)
					{
						(*LightMatrix[i])[j] = FVector::ZeroVector;
					}
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Right_PointOfIntersection, 50, FColor(255, 128, 128, 255), 10000);

				}
				//上值
				if (Top == -1 && Bottom == -2 && (*LightMatrix[i])[j] != FVector::ZeroVector)
				{
					FVector Pos = (*LightMatrix[i])[j];
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X , Pos.Y - 200.f, 0.f), FColor(255,0, 0, 255), 1000, 10);
					FVector Top_PointOfIntersection;
					FVector out_1To2Direction;
					float out_Distance;
					for (int index = 0; index < VertexList.Num(); ++index)
					{
						if (LineInLine(Pos, FVector(Pos.X, Pos.Y - 200.f, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()])
							&& LineInLine(VertexList[index], VertexList[(index + 1) % VertexList.Num()], Pos, FVector(Pos.X, Pos.Y - 200.f, 0.f)))
							USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X, Pos.Y - 200.f, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()], Top_PointOfIntersection, out_1To2Direction, out_Distance, false);
					}
					if (FVector(Pos - Top_PointOfIntersection).Size() < DelRadius/*(Spacing / 2)*/)
					{
						(*LightMatrix[i])[j] = FVector::ZeroVector;
					}
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Top_PointOfIntersection, 50, FColor(255, 128, 128, 255), 10000);

				}
				//下值
				if (Bottom == -1 && Top == -2 && (*LightMatrix[i])[j] != FVector::ZeroVector)
				{
					FVector Pos = (*LightMatrix[i])[j];
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), Pos, FVector(Pos.X, Pos.Y + 200.f, 0.f), FColor(255,255, 0, 255), 1000, 10);
					FVector Bottom_PointOfIntersection;
					FVector out_1To2Direction;
					float out_Distance;
					for (int index = 0; index < VertexList.Num(); ++index)
					{
						if (LineInLine(Pos, FVector(Pos.X, Pos.Y + 200.f, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()])
							&& LineInLine(VertexList[index], VertexList[(index + 1) % VertexList.Num()], Pos, FVector(Pos.X, Pos.Y + 200.f, 0.f)))
							USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(GetWorld(), Pos, FVector(Pos.X, Pos.Y + 200.f, 0.f), VertexList[index], VertexList[(index + 1) % VertexList.Num()], Bottom_PointOfIntersection, out_1To2Direction, out_Distance, false);
					}
					if (FVector(Pos - Bottom_PointOfIntersection).Size() < DelRadius/*(Spacing / 2)*/)
					{
						(*LightMatrix[i])[j] = FVector::ZeroVector;
					}
					//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), Bottom_PointOfIntersection, 50, FColor(255, 128, 128, 255), 10000);

				}
			}

		}
	}
	
	for (int i = 0; i < LightMatrix.Num(); ++i)
	{
		for (int j = 0; j < LightMatrix[i]->Num(); ++j)
		{
			FVector Pos = (*LightMatrix[i])[j];
			if (Pos != FVector::ZeroVector)
			{
				for (int k = 0; k < VertexList.Num(); ++k)
				{
					if (FVector(Pos - VertexList[k]).Size() < (DelRadius / 2))
					{
						(*LightMatrix[i])[j] = FVector::ZeroVector;
					}
				}
			}
		}
	}
}

void ARoomActor::DrawPointLightMatrix( FColor  Color,  float  Size,  float  LifeTime,  FVector  Restrict)
{
	for (int i = 0; i < LightMatrix.Num(); ++i)
	{
		for (int j = 0; j < LightMatrix[i]->Num(); ++j)
		{
			if((*LightMatrix[i])[j] != FVector::ZeroVector)
				UKismetSystemLibrary::DrawDebugPoint(GetWorld(),(*LightMatrix[i])[j],Size,Color, LifeTime);
		}
	}
}

void ARoomActor::GetPointLightMatrix(TArray<FVector> & Point)
{
	Point.Empty();
	for (int i = 0; i < LightMatrix.Num(); ++i)
	{
		for (int j = 0; j < LightMatrix[i]->Num(); ++j)
		{
			if ((*LightMatrix[i])[j] != FVector::ZeroVector)
			Point.Add((*LightMatrix[i])[j]);
		}
	}
}

void ARoomActor::CreatePointLight(bool bShadows, float Intensity, float Radius, float MinRoughness, float Hight)
{
	FTransform Transform;
	Transform.SetScale3D(FVector(1.f,1.f,1.f));
	GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), Transform);
}

void ARoomActor::SpotProcessing(APointLight * Spot)
{
	if (Spot)
	{

	}
}

bool ARoomActor::PointOnLine(FVector Point, FVector LineA, FVector LineB,FVector & FindPos, float Error)
{
	FindPos = FVector::ZeroVector;
	if (Error == -1)
	{
		Point.Y += 100.f;
		FindPos = Point;
		if (isLineInLine(Point, FindPos, LineA, LineB)
			&& isLineInLine(LineA, LineB, Point, FindPos))
			return true;
		return false;
	}
	else
	{
		FindPos = UKismetMathLibrary::FindClosestPointOnSegment(Point, LineA, LineB);
		if (FVector(FindPos - Point).Size() == 0.f)
		{
			if (isLineInLine(Point, FVector(Point.X, Point.Y+50.f,Point.Z), LineA, LineB)
				&& isLineInLine(LineA, LineB, Point, FVector(Point.X, Point.Y + 50.f, Point.Z)))
				return true;
		}
		else if (FVector(FindPos - Point).Size() <= Error)
		{
			if (isLineInLine(Point, FindPos, LineA, LineB)
				&& isLineInLine(LineA, LineB, Point, FindPos))
				return true;
		}
	}
	return false;
}

bool ARoomActor::LineInLine(FVector LineAa, FVector LineAb, FVector LineBa, FVector LineBb)
{
	float fA = (LineBa.Y - LineAa.Y) * (LineAa.X - LineAb.X) - (LineBa.X - LineAa.X) * (LineAa.Y -  LineAb.Y);
	float fB = (LineBb.Y - LineAa.Y) * (LineAa.X - LineAb.X) - (LineBb.X - LineAa.X) * (LineAa.Y - LineAb.Y);
	if (fA * fB > 0)
		return false;
	return true;
}

bool ARoomActor::LineInLine2D(FVector2D LineAa, FVector2D LineAb, FVector2D LineBa, FVector2D LineBb)
{
	if (LineInLine(FVector(LineAa,0.f), FVector(LineAb, 0.f), FVector(LineBa, 0.f), FVector(LineBb, 0.f))
		&& LineInLine(FVector(LineBa, 0.f), FVector(LineBb, 0.f), FVector(LineAa, 0.f), FVector(LineAb, 0.f)))
		return true;
	return false;
}

bool ARoomActor::isLineInLine(FVector LineAa, FVector LineAb, FVector LineBa, FVector LineBb)
{
	if (LineInLine(LineAa, LineAb, LineBa, LineBb)
		&& LineInLine(LineBa, LineBb, LineAa, LineAb))
		return true;
	return false;
}

void ARoomActor::PolygonsUnion(const TArray<FVector2D>& PolygonA, 
	const TArray<FVector2D>& PolygonB , 
	TArray<FVector2D>& OutPolygonsA,
	TArray<FVector2D>& OutPolygonsB,
	TArray<FVector2D>& Polygons, 
	TArray<FVector2D>& Intersection, 
	const float Error)
{
	TArray <FVector2D> TPolygonA = PolygonA;
	TArray <FVector2D> TPolygonB = PolygonB;

	auto FindIntersection = [&Error](TArray<FVector2D>& TPolygonA, TArray<FVector2D>& TPolygonB, TArray<FVector2D>& TIntersection)
	{
		FVector FindPos;
		for (int i = 0; i < TPolygonA.Num(); ++i)
		{
			for (int j = 0; j < TPolygonB.Num(); ++j)
			{
				if (PointOnLine(FVector(TPolygonA[i], 0.f), FVector(TPolygonB[j], 0.f), FVector(TPolygonB[(j + 1) % TPolygonB.Num()], 0.f), FindPos, Error))
				{
					TIntersection.AddUnique(TPolygonA[i]);
				}
			}
		}
	};
	FindIntersection(TPolygonA, TPolygonB, Intersection);
	FindIntersection(TPolygonB, TPolygonA, Intersection);

	if (Intersection.Num() == 0)
	{
		Polygons.Empty();
		Intersection.Empty();
		OutPolygonsA.Empty();
		OutPolygonsB.Empty();
		return;
	}
	
	auto InsertIntersection = [&Error](TArray<FVector2D>& TPolygon, TArray<FVector2D>& TIntersection)
	{
		FVector FindPos;
		for (int i = 0; i < TIntersection.Num(); ++i)
		{
			for (int j = 0; j < TPolygon.Num(); ++j)
			{
				if (TPolygon[j] == TIntersection[i] || FVector2D(TPolygon[j] - TIntersection[i]).Size() <= Error)
				{
					break;
				}
				else if (PointOnLine(FVector(TIntersection[i], 0.f), FVector(TPolygon[j], 0.f), FVector(TPolygon[(j + 1) % TPolygon.Num()], 0.f), FindPos, Error))
				{
					TPolygon.Insert(FVector2D(FindPos.X, FindPos.Y), j + 1);
				}
			}
		}
	};

	InsertIntersection(TPolygonA, Intersection);
	InsertIntersection(TPolygonB, Intersection);

	auto Filter = [&Error](TArray<FVector2D> & TPolygon)
	{
		TArray<FVector2D>  T;
		for (int i = 0; i < TPolygon.Num(); ++i)
		{
			if (FVector2D(TPolygon[(i + 1) % TPolygon.Num()] - TPolygon[i]).Size() > Error)
			{
				T.Add(TPolygon[i]);
			}
		}
		TPolygon.Empty();
		TPolygon = T;
	};

	Filter(TPolygonA);
	Filter(TPolygonB);

	auto InInterIndex = [&Error](TArray<FVector2D> & Intersection, FVector2D & Point, int & Index)mutable->bool
	{
		for (int i = 0; i < Intersection.Num(); ++i)
		{
			if (Intersection[i] == Point || FVector2D(Point - Intersection[i]).Size() <= Error)
			{
				Index = i;
				return true;
			}
		}
		return false;
	};

	auto Remove = [&InInterIndex](TArray<FVector2D> & TPolygon, TArray<FVector2D> & TIntersection)
	{
		int Index = 0;
		TArray<FVector2D> T;
		for (int i = 0; i < TPolygon.Num(); ++i)
		{
			if (InInterIndex(TIntersection, TPolygon[i], Index) && !InInterIndex(TIntersection, TPolygon[(i + 1) % TPolygon.Num()], Index))
			{
				T.AddUnique(TPolygon[i]);
			}
			else if (!InInterIndex(TIntersection, TPolygon[i], Index)
				&& InInterIndex(TIntersection, TPolygon[(i + 1) % TPolygon.Num()], Index))
			{
				T.AddUnique(TPolygon[i]);
				T.AddUnique(TPolygon[(i + 1) % TPolygon.Num()]);
			}
			else if (!InInterIndex(TIntersection, TPolygon[i], Index)
				&& !InInterIndex(TIntersection, TPolygon[(i + 1) % TPolygon.Num()], Index))
			{
				T.AddUnique(TPolygon[i]);
			}
		}
		TPolygon.Empty();
		TPolygon = T;
	};

	Remove(TPolygonA, Intersection);
	Remove(TPolygonB, Intersection);

	//头---顺时针---尾
	auto Sort = [&Intersection, &InInterIndex](TArray<FVector2D> & TPolygon)
	{
		int Index = 0;
		int Ti = 0;
		bool _isStart = false;
		TArray<FVector2D> T;
		for (int i = 0; i < TPolygon.Num(); )
		{
			int index = Ti + i;
			index = index % TPolygon.Num();
			if (index < 0)
			{
				Intersection.Empty();
				return;
			}
			if (!_isStart
				&& InInterIndex(Intersection, TPolygon[index], Index)
				&& !InInterIndex(Intersection, TPolygon[(index + 1) % TPolygon.Num()], Index))
			{
				_isStart = true;
				T.Add(TPolygon[index]);
				++i;
			}
			else if (_isStart
				&& !InInterIndex(Intersection, TPolygon[index], Index)
				&& !InInterIndex(Intersection, TPolygon[(index + 1) % TPolygon.Num()], Index))
			{
				T.Add(TPolygon[index]);
				++i;
			}
			else if (_isStart
				&& !InInterIndex(Intersection, TPolygon[index], Index)
				&& InInterIndex(Intersection, TPolygon[(index + 1) % TPolygon.Num()], Index))
			{
				T.Add(TPolygon[index]);
				T.AddUnique(TPolygon[(index + 1) % TPolygon.Num()]);
				++i;
				++i;
			}
			else
			{
				++Ti;
			}
		}
		TPolygon.Empty();
		TPolygon = T;
	};
	Sort(TPolygonA);
	Sort(TPolygonB);
	Intersection.Empty();

	auto PUnion = [&Intersection, &Polygons, &InInterIndex](TArray<FVector2D> & TPolygonA, TArray<FVector2D> & TPolygonB)
	{
		bool isSwitch = true;
		int Num = TPolygonA.Num() + TPolygonB.Num();
		int TA = 0;
		int TB = 0;
		int index = 0;
		for (int i = 0; i < Num; ++i)
		{
			if (isSwitch)
			{
				for (int pa = 0; pa < TPolygonA.Num(); ++pa)
				{
					index = pa + TA;
					index = index % TPolygonA.Num();
					if (InInterIndex(TPolygonB, TPolygonA[index], TB))
					{
						++TB;
						Polygons.AddUnique(TPolygonA[index]);
						Intersection.AddUnique(TPolygonA[index]);
						isSwitch = false;
						break;
					}
					else
					{
						Polygons.AddUnique(TPolygonA[index]);
					}
				}
			}
			else
			{
				for (int pb = 0; pb < TPolygonB.Num(); ++pb)
				{
					index = pb + TB;
					index = index % TPolygonB.Num();
					if (InInterIndex(TPolygonA, TPolygonB[index], TA))
					{
						++TA;
						Intersection.AddUnique(TPolygonB[index]);
						Polygons.AddUnique(TPolygonB[index]);
						isSwitch = true;
						break;
					}
					else
					{
						Polygons.AddUnique(TPolygonB[index]);
					}
				}
			}
		}
	};
	auto PSortUnion = [&Intersection, &Polygons, &InInterIndex](TArray<FVector2D> & TPolygonA, TArray<FVector2D> & TPolygonB)
	{
		for (int i = 0; i < TPolygonA.Num() - 1; ++i)
		{
			Polygons.Add(TPolygonA[i]);
		}
		for (int i = 0; i < TPolygonB.Num() - 1; ++i)
		{
			Polygons.Add(TPolygonB[i]);
		}
		TPolygonA[TPolygonA.Num() - 1] = TPolygonB[0];
		TPolygonB[TPolygonB.Num() - 1] = TPolygonA[0];
		Intersection.Add(TPolygonA[0]);
		Intersection.Add(TPolygonB[0]);
	};
	PSortUnion(TPolygonA, TPolygonB);
	OutPolygonsA = TPolygonA;
	OutPolygonsB = TPolygonB;
}


void ARoomActor::MergePolygonPoints(const TArray<FVector2D>& PolygonA,
	const TArray<FVector2D>& PolygonB,
	TArray<FVector2D>& OutPolygonsA,
	TArray<FVector2D>& OutPolygonsB,
	const float DisBound)
{
	TArray<FVector2D> BasePolygon, OtherPolygon;
	if (PolygonA.Num() >= PolygonB.Num())
	{
		BasePolygon = PolygonA;
		OtherPolygon = PolygonB;
	}
	else
	{
		BasePolygon = PolygonB;
		OtherPolygon = PolygonA;
	}
	int32 NextPIndex = 0;
	for (int32 Index = 0; Index < OtherPolygon.Num(); Index++)
	{

		for (int32 PIndex = 0; PIndex < BasePolygon.Num(); PIndex++)
		{
			if (PIndex == BasePolygon.Num() - 1)
			{
				NextPIndex = 0;
			}
			else
			{
				NextPIndex = PIndex + 1;
			}
			FVector CloseP = UKismetMathLibrary::FindClosestPointOnSegment(FVector(OtherPolygon[Index], 0), FVector(BasePolygon[PIndex], 0), FVector(BasePolygon[NextPIndex], 0));
			FVector ToVec(CloseP.X - OtherPolygon[Index].X, CloseP.Y - OtherPolygon[Index].Y, 0);
			float Dis = sqrt(ToVec.SizeSquared());
			if (Dis <= DisBound)
			{
				OtherPolygon[Index] = FVector2D(CloseP.X, CloseP.Y);
				break;
			}
			else
			{
				continue;
			}
		}
	}
	OutPolygonsA = BasePolygon;
	OutPolygonsB = OtherPolygon;
}


bool ARoomActor::PolygonsUnionFunction(const TArray<FVector2D>& PolygonA,
	const TArray<FVector2D>& PolygonB,
	TArray<FVector2D>& OutPolygonsA,
	TArray<FVector2D>& OutPolygonsB,
	TArray<FVector2D>& Polygons)
{
	bool Success = FPolygonAlg::CreatePolygonUnion(PolygonA, PolygonB, OutPolygonsA, OutPolygonsB, Polygons);
	if (!Success)
	{
		return false;
	}
	return true;
}


bool ARoomActor::LocateCenter(const TArray<FVector> InPnts, FVector2D& BetterCenter)
{
	TArray<FVector2D> To2DPnts;
	for (int32 Index = 0; Index < InPnts.Num(); Index++)
	{
		To2DPnts.Add(FVector2D(InPnts[Index].X, InPnts[Index].Y));
	}
	bool IsGetted = FPolygonAlg::LocateSuitableCenter(To2DPnts, BetterCenter);
	return IsGetted;
}

void ARoomActor::FinishingPolygons(TArray<FVector2D>& Polygons)
{
	TArray<FVector2D> TPolygons;
	for (int i = 0; i < Polygons.Num(); i++)
	{
		FVector2D A(Polygons[i]);
		FVector2D O(Polygons[(i + 1) % Polygons.Num()]);
		FVector2D B(Polygons[(i + 2) % Polygons.Num()]);
		float res = IsPolyClockWiseAB(A, O, B, true);
		if (res != 0.f  || ((res > -0.03f && res < 0.03f)))
		{
			TPolygons.Add(O);
		}
	}
	Polygons.Empty();
	Polygons = TPolygons;
}

bool ARoomActor::isPointInPolygons(const TArray<FVector>& Polygons, const FVector & Point, float Length)
{
	int Left = 0;
	int Right = 0;
	for (int i = 0; i < Polygons.Num(); i++)
	{
		if (LineInLine(Polygons[i], Polygons[(i + 1) % Polygons.Num()], Point, FVector(Point.X, Point.Y + Length, Point.Z))
			&& LineInLine(Point, FVector(Point.X, Point.Y + Length, Point.Z), Polygons[i], Polygons[(i + 1) % Polygons.Num()]))
		{
			++Left;
		}
		if (LineInLine(Polygons[i], Polygons[(i + 1) % Polygons.Num()], Point, FVector(Point.X, Point.Y - Length, Point.Z))
			&& LineInLine(Point, FVector(Point.X, Point.Y - Length, Point.Z), Polygons[i], Polygons[(i + 1) % Polygons.Num()]))
		{
			++Right;
		}
	}
	if (Left == 0 && Right == 0)
		return false;

	if(Left%2==0 && Right%2==0)
		return false;
	return true;
}

bool ARoomActor::isPointInPolygons2D(const TArray<FVector2D>& Polygons, const FVector2D & Point2D, float Length)
{
	int Left = 0;
	int Right = 0;
	for (int i = 0; i < Polygons.Num(); i++)
	{
		if (LineInLine2D(Polygons[i], Polygons[(i + 1) % Polygons.Num()], Point2D, FVector2D(Point2D.X, Point2D.Y + Length))
			&& LineInLine2D(Point2D, FVector2D(Point2D.X, Point2D.Y + Length), Polygons[i], Polygons[(i + 1) % Polygons.Num()]))
		{
			++Left;
		}
		if (LineInLine2D(Polygons[i], Polygons[(i + 1) % Polygons.Num()], Point2D, FVector2D(Point2D.X, Point2D.Y - Length))
			&& LineInLine2D(Point2D, FVector2D(Point2D.X, Point2D.Y - Length), Polygons[i], Polygons[(i + 1) % Polygons.Num()]))
		{
			++Right;
		}
	}
	if (Left % 2 == 0 && Right % 2 == 0)
		return false;
	return true;
}

float ARoomActor::GetRoomAcreage()
{
	float Acreage = 0.f;
	if (PGround)
	{
		float _Area = 0.f;
		FProcMeshSection* _Array = PGround->GetProcMeshSection(0);
		if (_Array)
		{
			int _a, _b, _c;
			const int32 _NUM = _Array->ProcIndexBuffer.Num();
			for (int i = 0; i < _NUM; i += 3)
			{
				_a = _Array->ProcIndexBuffer[i];
				_b = _Array->ProcIndexBuffer[(i + 1) % _NUM];
				_c = _Array->ProcIndexBuffer[(i + 2) % _NUM];

				double _AB = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_b].Position).Size2D();
				double _BC = (_Array->ProcVertexBuffer[_b].Position - _Array->ProcVertexBuffer[_c].Position).Size2D();
				double _CA = (_Array->ProcVertexBuffer[_c].Position - _Array->ProcVertexBuffer[_a].Position).Size2D();
				double p = (_AB + _BC + _CA) / 2.0;
				_Area += FMath::Sqrt(p * (p - _AB) * (p - _BC) * (p - _CA));
			}
			Acreage = _Area;
			Acreage /= 10000.f;
			return Acreage;
		}
	}
	return -1.f;
}

void ARoomActor::PolygonsScale(const TArray<FVector>& Polygons, TArray<FVector>& OutPolygon)
{
	for (int i = 0; i < Polygons.Num(); i++)
	{
		FVector2D A(Polygons[i]);
		FVector2D O(Polygons[(i + 1) % Polygons.Num()]);
		FVector2D B(Polygons[(i + 2) % Polygons.Num()]);
		FVector2D OA(A - O);
		FVector2D OB(B - O);
		FVector2D OC(OA + OB);
		OA.Normalize();
		OB.Normalize();
		OC.Normalize();
		float out = OA.X*OB.Y - OB.X*OA.Y;
		if (out > 0.f)
		{
			FVector2D T(O + (OC * -20.f));
			if (FVector2D(O - T).Size() > 0.02)
				OutPolygon.Add(FVector(T, 0.f));
		}
		else
		{
			FVector2D T(O + (OC * 20.f));
			if (FVector2D(O - T).Size() > 0.02)
				OutPolygon.Add(FVector(T, 0.f));
		}
	}
}

void ARoomActor::PolygonsScale2D(const TArray<FVector2D>& Polygons, TArray<FVector2D>& OutPolygon, float Scale)
{
	OutPolygon.Empty();
	for (int i = 0; i < Polygons.Num(); i++)
	{
		FVector2D A(Polygons[i]);
		FVector2D O(Polygons[(i + 1) % Polygons.Num()]);
		FVector2D B(Polygons[(i + 2) % Polygons.Num()]);
		FVector2D OA(A - O);
		FVector2D OB(B - O);
		FVector2D OC(OA + OB);
		OA.Normalize();
		OB.Normalize();
		OC.Normalize();
		float out = OA.X*OB.Y - OB.X*OA.Y;
		if (out > 0.f)
		{
			FVector2D T(O + (OC * -Scale));
			if (FVector2D(O - T).Size() > 0.02)
				OutPolygon.Add(T);
		}
		else
		{
			FVector2D T(O + (OC *  Scale));
			if (FVector2D(O - T).Size() > 0.02)
				OutPolygon.Add(T);
		}
	}
}

void ARoomActor::UpdateBuildingData(UBuildingData *Data)
{
	if (PGround && Roof && LitMesh)
	{
		if (isUpdate && Data->GetObjectType() == EObjectType::ECeilPlane)
		{
			if (Roof->Data == Data)
			{
				Roof->SetData(Data);
				FProcMeshSection * _Roof = Roof->GetProcMeshSection(0);
				if (_Roof)
				{
					TArray<FVector> Ver;
					TArray<FVector> Nor;
					TArray<FVector2D> UV0;

					for (int i = 0; i < _Roof->ProcVertexBuffer.Num(); ++i)
					{
						Ver.Add(FVector(_Roof->ProcVertexBuffer[i].Position.X, _Roof->ProcVertexBuffer[i].Position.Y,260.f));
						Nor.Add(_Roof->ProcVertexBuffer[i].Normal);
						UV0.Add(_Roof->ProcVertexBuffer[i].UV0);
					}
					//Roof->CreateCustomizeMesh(1, Ver,_Roof->ProcIndexBuffer,Nor, UV0, true);
				}
				
			}
			isUpdate = false;
		}
		else if (Data->GetObjectType() == EObjectType::EFloorPlane)
		{
			if(PGround->Data == Data)
				PGround->SetData(Data);
		}
		BPUpdateBuildingData(Data);
	}
}

void ARoomActor::UpdateRoof()
{
	isUpdate = true;
	if(Roof->Data)
		Roof->Data->Update();
}

bool ARoomActor::GetDependsWalls(TArray<ALineWallActor*>& outDependsWalls)
{
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	FRoomPath path = GetRoomPath();
	if (WallsInScene.Num() == 0 || path.InnerRoomPath.Num() < 3)
	{
		return false;
	}
	for (ALineWallActor* WallActor : WallsInScene)
	{
		int32 WallID = WallActor->GetWallID();
		UBuildingSystem *BuildingSystem = WallActor->WallComp3d->Data->GetBuildingSystem();
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		if (!WallData) {
			continue;
		}
		int StartCornerID = WallData->GetInt("P0");
		int EndCornerID = WallData->GetInt("P1");
		if (StartCornerID <= 0 || EndCornerID <= 0) {
			continue;
		}
		FVector2D PStartPos = BuildingSystem->GetData(StartCornerID)->GetVector2D("Location");
		FVector2D PEndPos = BuildingSystem->GetData(EndCornerID)->GetVector2D("Location");
		FVector2D wallcenter = 0.5f*(PEndPos + PStartPos);

		float TempWallLeftThick = 0.f;
		float TempWallRighThick = 0.f;
		if (WallActor->WallComp3d)
		{
			TempWallLeftThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickLeft"));
			TempWallRighThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickRight"));
		}
		float thick = TempWallLeftThick + TempWallRighThick;
		FVector2D WallVec = PEndPos - PStartPos;
		WallVec.Normalize();
		FVector2D OutVec = WallVec.GetRotated(90);
		FVector2D checkpoint1 = wallcenter + 2 * thick*OutVec;
		FVector2D checkpoint2 = wallcenter - 2 * thick*OutVec;
		bool isinner = FPolygonAlg::JudgePointInPolygon(path.InnerRoomPath, checkpoint1) || FPolygonAlg::JudgePointInPolygon(path.InnerRoomPath, checkpoint2);
		if (isinner) {
			outDependsWalls.AddUnique(WallActor);
		}
	}

	return outDependsWalls.IsValidIndex(0);
}

int32 ARoomActor::GetRoomDataID()
{
	if (PGround)
	{
		if (PGround->Data)
		{
			return PGround->Data->GetInt("RoomID");
		}
	}
	return -1;
}

TArray<AComponentManagerActor*> ARoomActor::GetHousePlugin(const TArray<AActor*>&Actors)
{
	TArray<AComponentManagerActor*> OutHousePluginArr;
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance&&GameInstance->WallBuildSystem)
	{
		//TArray<AActor*> OutActor;
		//UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
		for (int i = 0; i < Actors.Num(); ++i)
		{
			AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(Actors[i]);
			if (ComponentManager)
			{
				FComponenetInfo HouseCompInfo = ComponentManager->GetHouseComponentInfo();
				FVector2D Loc = HouseCompInfo.Loc;
				TArray<FVector> Slots;
				if (GetAreaVer3DList(Slots))
				{
					if (GameInstance->WallBuildSystem->JudgePointInPolygon(Slots,Loc))
					{
						OutHousePluginArr.Add(ComponentManager);
					}
				}
			}
		}
	}
	return OutHousePluginArr;
}