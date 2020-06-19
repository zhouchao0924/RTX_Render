// Fill out your copyright notice in the Description page of Project Settings.

#include "AJBlueprintFunctionLibrary.h"
#include <stdio.h>
#include <math.h>
#include "Math/UnrealMathUtility.h"
#include <stdlib.h>
#include <iostream>
#include "PhysicsEngine/PhysicsSettings.h"
#include "Misc/DateTime.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "Building/BuildingComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "../Plugins/PeExtendedToolKit/Source/PeExtendedToolKit/Public/PeExtendedToolKitBPLibrary.h"
#include "SocketSubsystem.h"

using namespace std;

DEFINE_LOG_CATEGORY_STATIC(AJDR, Warning, All);


#define USE_WINDOWS 0

#if USE_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <shellapi.h>
#include "HideWindowsPlatformAtomics.h"	
#endif

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)


void UAJBlueprintFunctionLibrary::openFolder(FString foldername)
{
	FString name = TEXT("Explorer ");

	name.Append(foldername);
	FString str("/");
	const TCHAR* c1 = *str;
	FString str2("\\");
	const TCHAR* c2 = *str2;
	FString name2 = name.Replace(c1,c2);
	system(TCHAR_TO_UTF8(*name2));
};

void UAJBlueprintFunctionLibrary::openUrl(FString url)
{
	//ShellExecute(NULL, "open", "cmd.exe", "start chrome.exe", NULL, SW_SHOWDEFAULT);
	FString name = TEXT("start chrome.exe ");
	name.Append(url);
	system(TCHAR_TO_UTF8(*name));
};

bool UAJBlueprintFunctionLibrary::InsidePolygon(TArray<FVector2D> points, FVector2D p)
{
	/*bool result = false;
	for (int i = 0; i < points.Num() - 1; i++)
	{
		if ((((points[i + 1].Y <= p.Y) && (p.Y < points[i].Y)) || ((points[i].Y <= p.Y) && (p.Y < points[i + 1].Y))) && (p.X < (points[i].X - points[i + 1].X) * (p.Y - points[i + 1].Y) / (points[i].Y - points[i + 1].Y) + points[i + 1].X))
		{
			result = !result;
		}
	}
	return result;*/
	TArray<FVector2D> pointsR;
	for (int i = 0; i < points.Num(); i++) {
		FVector2D r = FVector2D(round(points[i].X * 10) / 10, round(points[i].Y * 10) / 10);
		pointsR.Add(r);
	}
	FVector2D pR = FVector2D(round(p.X * 10) / 10, round(p.Y * 10) / 10);

	int nCount = pointsR.Num();
	int nCross = 0;      
	for (int i = 0; i < nCount; i++) {
		FVector2D p1 = pointsR[i];
		
		FVector2D p2 = pointsR[(i + 1) % nCount];        // 求解 y=p.y 与 p1p2 的交点
		if ( p1.Y == p2.Y ) // p1p2 与 y=p0.y平行        
			continue;        
		if ( pR.Y < MIN(p1.Y, p2.Y) ) // 交点在p1p2延长线上        
			continue;       
		if ( pR.Y >= MAX(p1.Y, p2.Y) ) // 交点在p1p2延长线上       
			continue;        // 求交点的 X 坐标 --------------------------------------------------------------      
		double x = (double)(pR.Y - p1.Y) * (double)(p2.X - p1.X) / (double)(p2.Y - p1.Y) + p1.X;        
		if ( x > pR.X )         
			nCross++; // 只统计单边交点    
	}       
	// 单边交点为偶数，点在多边形之外 ---     
	bool isInBoundary = IsPointInPolygonBoundary(points, p);
	bool isInVector = pointsR.Contains(pR);
	return (nCross % 2 == 1)||isInBoundary || isInVector; 

}

bool UAJBlueprintFunctionLibrary::IsPointInPolygonBoundary(TArray<FVector2D> points, FVector2D p) {
	TArray<FVector2D> pointsR;
	for (int i = 0; i < points.Num(); i++) {
		FVector2D r = FVector2D(round(points[i].X * 10) / 10, round(points[i].Y * 10) / 10);
		pointsR.Add(r);
	}
	FVector2D pR = FVector2D(round(p.X * 10) / 10, round(p.Y * 10) / 10);

	for (int i = 0; i < pointsR.Num(); i++) {
		FVector2D p1 = pointsR[i];
		FVector2D p2 = pointsR[(i + 1) % pointsR.Num()];
		// 取多边形任意一个边,做点point的水平延长线,求解与当前边的交点个数  

		// point 在p1p2 底部 --> 无交点  
		if(pR.Y <MIN(p1.Y, p2.Y))
			continue;
		// point 在p1p2 顶部 --> 无交点  
		if(pR.Y>MAX(p1.Y, p2.Y))
			continue;

		// p1p2是水平线段,要么没有交点,要么有无限个交点  
		if(p1.Y == p2.Y) {
			double minX = MIN(p1.X, p2.X);
			double maxX = MAX(p1.X, p2.X);
			// point在水平线段p1p2上,直接return true  
			if ((pR.Y  == p1.Y) && (pR.X >= minX && pR.X <= maxX)) {
				return true;
			}
		} else {
			double x  = (pR.Y - p1.Y) * (p2.X - p1.X) / (p2.Y  - p1.Y) + p1.X;
		if (x == pR.X)
			return true;
		}
	}
	return false;
}

TArray<FVector2D> UAJBlueprintFunctionLibrary::SpreadPolygon(TArray<FVector2D> points, int dist)
{
	UE_LOG(AJDR, Log, TEXT("dist is %d"), dist);

	TArray<FVector2D> dpList;
	TArray<FVector2D> ndpList;
	TArray<FVector2D> newList;

	int nCount = points.Num();
	UE_LOG(AJDR, Log, TEXT("dist is %d"), dist);
	for (int i = 0; i < nCount; i++) {
		FVector2D p1 = points[(i == nCount - 1) ? 0 : (i + 1)];
		FVector2D p2 = points[i];
		FVector2D p;
		p.X = p1.X - p2.X;
		p.Y = p1.Y - p2.Y;
		UE_LOG(AJDR, Log, TEXT("x is %f, y is %f"), p.X, p.Y);
		dpList.Add(p);
	}
	for (int i = 0; i < nCount; i++) {
		FVector2D p = dpList[i] * (1.0 / sqrt(dpList[i].X*dpList[i].X + dpList[i].Y*dpList[i].Y));
		UE_LOG(AJDR, Log, TEXT("x is %f, y is %f"), p.X, p.Y);
		ndpList.Add(p);
	}

	int startIndex;
	int endIndex;
	for (int i = 0; i < nCount; i++) {
		startIndex = (i == 0) ? (nCount - 1) : (i - 1);
		endIndex = i;

		float sina = ndpList[startIndex].X*ndpList[endIndex].Y - ndpList[startIndex].Y*ndpList[endIndex].X;
		int length = dist / sina;
		UE_LOG(AJDR, Log, TEXT("sina is %f, length is %d"), sina, length);
		FVector2D vector = ndpList[endIndex] - ndpList[startIndex];
		FVector2D p = points[i] + vector*length;
		newList.Add(p);
	}

	return newList;
}

TArray<FVector2D> UAJBlueprintFunctionLibrary::IntersectPolygon(TArray<FVector2D> polygon1, TArray<FVector2D> polygon2, TArray<FVector2D> outpoints)
{
	TArray<FVector2D> polygon1R;
	for (int i = 0; i < polygon1.Num(); i++) {
		FVector2D r = FVector2D(round(polygon1[i].X * 10) / 10, round(polygon1[i].Y * 10) / 10);
		polygon1R.Add(r);
	}
	TArray<FVector2D> polygon2R;
	for (int i = 0; i < polygon2.Num(); i++) {
		FVector2D r = FVector2D(round(polygon2[i].X * 10) / 10, round(polygon2[i].Y * 10) / 10);
		polygon2R.Add(r);
	}

	TArray<FVector2D> ops = TArray<FVector2D>();
	int nCount = polygon1R.Num();
	for (int i = 0; i < nCount; i++) {
		FVector2D tp11 = polygon1R[i];
		FVector p11 = FVector(tp11.X, tp11.Y,0);
		int end = i == nCount - 1 ? 0 : i + 1;
		FVector2D tp12 = polygon1R[end];
		FVector p12 = FVector(tp12.X, tp12.Y, 0);
		int mCount = polygon2R.Num();
		for (int j = 0; j < mCount; j++) {
			FVector2D tp21 = polygon2R[j];
			FVector p21 = FVector(tp21.X, tp21.Y, 0);
			int endj = j == mCount - 1 ? 0 : j + 1;
			FVector2D tp22 = polygon2R[endj];
			FVector p22 = FVector(tp22.X, tp22.Y, 0);
			FVector outpoint;
			bool isIntersect = FMath::SegmentIntersection2D(p11, p12, p21, p22, outpoint);
			if (isIntersect) {
				ops.Add(FVector2D(outpoint.X, outpoint.Y));
			}
		}
	}
	return ops;
}

bool UAJBlueprintFunctionLibrary::Conv_StringToTransform(const FString &InStr, FTransform & outtransform)
{

	FString left, right, left1, right1, left2, right2;
	bool success;
	FVector localScale, localPos;
	FRotator localRote;

	success = UKismetStringLibrary::Split(InStr, FString(TEXT("Scale")), left, right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (success) {
		bool cv, cv1, cv2, cv3, cv4;
		UKismetStringLibrary::Conv_StringToVector(right, localScale, cv);
		cv1 = UKismetStringLibrary::Split(left, FString(TEXT("Rotation:")), left1, right1, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		UKismetStringLibrary::Conv_StringToRotator(right1, localRote, cv2);
		if (cv2) {
			cv3 = UKismetStringLibrary::Split(left1, FString(TEXT("Translation:")), left2, right2, ESearchCase::IgnoreCase, ESearchDir::FromStart);
			if (cv3) {
				UKismetStringLibrary::Conv_StringToVector(right2, localPos, cv4);
			}

		}
		if (cv4) {
			outtransform = FTransform(localRote, localPos, localScale);
			return true;
		}
	}
	return false;
}


//VertexLocs:
/*
   1--------2
  /|       /|
 / |      / |
4--|-----3  |
|  5-----|--6
| /      | /
|/       |/
8--------7
*/
void UAJBlueprintFunctionLibrary::GetBoundPointsByBoxExtent(const FVector& BoxExtent, const FTransform& Center, TArray<FVector>& BoundPoints)
{
	BoundPoints.Add(FVector(BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));//1
	BoundPoints.Add(FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z));//2
	BoundPoints.Add(FVector(-BoxExtent.X, BoxExtent.Y, BoxExtent.Z));//3
	BoundPoints.Add(FVector(-BoxExtent.X, -BoxExtent.Y, BoxExtent.Z));//4
	BoundPoints.Add(FVector(BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));//5
	BoundPoints.Add(FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));//6
	BoundPoints.Add(FVector(-BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));//7
	BoundPoints.Add(FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));//8
	//旋转
	for (FVector& Item : BoundPoints)
	{
		Item = Center.GetRotation().RotateVector(Item);
	}

	int i = 0;
	//转换为世界位置
	for (FVector& Item : BoundPoints)
	{
		Item += Center.GetLocation();
		//UE_LOG(LogTemp, Log, TEXT("Boundpoint[%d], pos[%s]"), i, *(Item.ToString()));
		i++;
	}
}

void UAJBlueprintFunctionLibrary::GetBoundPointsByBoxBound(const FVector& Min, const FVector& Max, const FTransform& Center, TArray<FVector>& BoundPoints)
{
	FVector Orgin = (Min + Max) / 2;
	FVector BoxExtent = (Max - Min) / 2;
	BoxExtent.X = FMath::Abs(BoxExtent.X);
	BoxExtent.Y = FMath::Abs(BoxExtent.Y);
	BoxExtent.Z = FMath::Abs(BoxExtent.Z);
	GetBoundPointsByBoxExtent(BoxExtent, Center, BoundPoints);
}

FString UAJBlueprintFunctionLibrary::GetCurrentPCIPAddress()
{
	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(NAME_None);
	if (socketSubsystem == nullptr) {
		return TEXT("127.0.0.1");
	}

	bool bCanBindAll = false;
	TSharedRef<FInternetAddr> addrInfo = socketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll);

	return addrInfo->ToString(false);
}

bool UAJBlueprintFunctionLibrary::AssignMeshComponent(UActorComponent *Component, int32 FaceIndex, UMaterialInterface *NewMaterial)
{
	UStaticMeshComponent *MeshComp = Cast<UStaticMeshComponent>(Component);

	if (MeshComp && NewMaterial && FaceIndex>=0)
	{
		uint32 face = (uint32)FaceIndex*3;
		UStaticMesh *StaticMesh = MeshComp->GetStaticMesh();
		if (StaticMesh)
		{
			for (int32 LODIndex = 0; LODIndex < StaticMesh->RenderData->LODResources.Num(); ++LODIndex)
			{
				FStaticMeshLODResources &LOD = StaticMesh->RenderData->LODResources[LODIndex];
				for (int32 i = 0; i < LOD.Sections.Num(); ++i)
				{
					FStaticMeshSection &Section = LOD.Sections[i];
					if (face >= Section.FirstIndex && face < (Section.FirstIndex + 3*Section.NumTriangles))
					{
						MeshComp->SetMaterial(Section.MaterialIndex, NewMaterial);
						return true;
					}
				}
			}
		}
	}
	return false;
}

UActorComponent *UAJBlueprintFunctionLibrary::GetComponentByName(AActor *InActor, TSubclassOf<UActorComponent> FoundClass, const FString &CompName)
{
	if (!FoundClass)
	{
		FoundClass = UActorComponent::StaticClass();
	}

	TArray<UActorComponent *> Components = InActor->GetComponentsByClass(FoundClass);
	for (int32 i = 0; i < Components.Num(); ++i)
	{
		UActorComponent *Comp = Components[i];
		if (Comp->GetName() == CompName)
		{
			return Comp;
		}
	}

	return NULL;
}

FString UAJBlueprintFunctionLibrary::GetComponentName(UActorComponent *Comp)
{
	FString Name;
	if (Comp!=NULL)
	{
		Name = Comp->GetName();
	}
	return Name;
}

FVector2D UAJBlueprintFunctionLibrary::DirectionInWorld(FVector2D mvector)
{
	bool b1 = FVector2D::DotProduct(mvector, FVector2D(1, 1))>=0;
	bool b2 = FVector2D::DotProduct(mvector, FVector2D(1, -1))>=0;
	if (b1&&b2) {
		return FVector2D(1,0);
	}
	if (!b1&&!b2) {
		return FVector2D(-1, 0);
	}
	if (b1&&!b2) {
		return FVector2D(0, 1);
	}
	if (!b1&&b2) {
		return FVector2D(0, -1);
	}
	return FVector2D(0,0);
}

void UAJBlueprintFunctionLibrary::CopyMessageToClipboard(FString text)
{
	FPlatformMisc::ClipboardCopy(*text);
}

FString UAJBlueprintFunctionLibrary::PasteMessageFromClipboard()
{
	FString ClipboardContent;
	FPlatformMisc::ClipboardPaste(ClipboardContent);
	return ClipboardContent;
}


//SegmentIntersection2D
FBox UAJBlueprintFunctionLibrary::GetMeshExtent(UStaticMesh *InMesh)
{
	return InMesh->GetBoundingBox();
	/*if (WallLine && WallLine->Mesh != nullptr && WallLine->Mesh->RenderData.IsValid())
	{
		for (int32 i = 0; i < WallLine->Positions.Num(); ++i)
		{
			FVector Position = WallLine->Positions[i];
			if (Position.X > Length)
			{
				Length = Position.X;
			}
		}
		FVector Ext = WallLine->Mesh->ExtendedBounds.BoxExtent;
		Radius = FMath::Max<float>(Ext.Y, Ext.Z);
	}*/
}

int32 UAJBlueprintFunctionLibrary::GetStringLength(FString fStr,bool &isSuccess)
{
	auto length = 0;
	isSuccess = false;

 	length = fStr.Len();

	isSuccess = true;
	return length;
	return 0;
}

bool UAJBlueprintFunctionLibrary::GetJsonObjectFromFString(FString data, UVaRestJsonObject* VaRestJsonObject)
{
	FString tmpData = data;
	TSharedRef<TJsonReader<TCHAR>> Reader = FJsonStringReader::Create(MoveTemp(tmpData));

	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return false;
	}
	VaRestJsonObject->SetRootObject(JsonObject);

	return true;

}
//
//bool UAJBlueprintFunctionLibrary::CheckNetWorkLink()
//{
//	CoInitialize(NULL);
//	//  通过LAN接口获取网络状态
//	IUnknown *pUnknown = NULL;
//	bool bOnline = true;
//	HRESULT Result = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void **)&pUnknown);
//	if (SUCCEEDED(Result))
//	{
//		INetworkListManager *pNetworkListManager = NULL;
//		if (pUnknown)
//			Result = pUnknown->QueryInterface(IID_INetworkListManager, (void **)&pNetworkListManager);
//		if (SUCCEEDED(Result))
//		{
//			VARIANT_BOOL IsConnect = VARIANT_FALSE;
//			if (pNetworkListManager)
//				Result = pNetworkListManager->get_IsConnectedToInternet(&IsConnect);
//			if (SUCCEEDED(Result))
//			{
//				bOnline = (IsConnect == VARIANT_TRUE) ? true : false;
//			}
//		}
//		if (pNetworkListManager)
//			pNetworkListManager->Release();
//	}
//	if (pUnknown)
//		pUnknown->Release();
//	CoUninitialize();
//	return bOnline;
//}
FString UAJBlueprintFunctionLibrary::DateTimeToString(FDateTime DateTime)
{
	FString TimeString;
	TimeString = FString::FromInt(DateTime.GetYear()) + "/" + FString::FromInt(DateTime.GetMonth()) + "/" + FString::FromInt(DateTime.GetDay()) + " " + FString::FromInt(DateTime.GetHour()) + ":" + FString::FromInt(DateTime.GetMinute()) + ":" + FString::FromInt(DateTime.GetSecond());
	return TimeString;
}

bool UAJBlueprintFunctionLibrary::EqualEqual_SizeSize(FSize A, FSize B, float ErrorTolerance)
{
	return A.Equals(B,ErrorTolerance);
}


float UAJBlueprintFunctionLibrary::GetAreaFromPoints(TArray<FVector2D> Points)
{
	TArray<FVector2D> TPolygons = Points;
	float OutS = 0;
	if (TPolygons.Num() > 2)
	{
		for (int i = 0; i < TPolygons.Num(); ++i)
		{
			OutS += FVector2D::CrossProduct(TPolygons[i], TPolygons[(i + 1) % TPolygons.Num()]);
		}
	}
	OutS /= 2;

	return abs(OutS);
}

bool UAJBlueprintFunctionLibrary::CheckIsShowPrice(ESlateVisibility &VisibleState)
{
	FString FilePath = FPaths::ProjectSavedDir() + "Config/WindowsNoEditor/DefaultShowPrice.ini";
	if (FPaths::FileExists(FilePath))
	{
		FString IsShowPrice;
		FFileHelper::LoadFileToString(IsShowPrice, *FilePath);
		VisibleState = IsShowPrice.ToBool() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		return IsShowPrice.ToBool();
	}
	else
	{
		FString DefaultShow = "true";
		FFileHelper::SaveStringToFile(DefaultShow, *FilePath);
		VisibleState = ESlateVisibility::Visible;
		return true;
	}
}

void UAJBlueprintFunctionLibrary::SetIsShowPrice(bool IsShow)
{
	FString FilePath = FPaths::ProjectSavedDir() + "Config/WindowsNoEditor/DefaultShowPrice.ini";
	if (IsShow)
	{
		FFileHelper::SaveStringToFile("true", *FilePath);
	}
	else
	{
		FFileHelper::SaveStringToFile("false", *FilePath);
	}
}

bool UAJBlueprintFunctionLibrary::Adsorption(UObject* WorldContextObject, const FVector InActorLoction, const FVector InActorOrigin, const FVector InMoudelFilBounds, const FVector InActorBounds, const FRotator InOrientation, const FVector InDirection, const TArray<AActor*> ActorsToIgnore, const TArray<UBuildingComponent*> AreaComponent, FVector& NewLocation)
{
	if (InDirection == FVector::ZeroVector)
	{
		return false;
	}
	FRotator Local_TempOrientation = InOrientation;
	FVector Local_TempMoudelFilBounds = FVectorAbsValue(InMoudelFilBounds);
	if (Local_TempMoudelFilBounds.X<=0|| Local_TempMoudelFilBounds.Y<=0|| Local_TempMoudelFilBounds.Z<=0)
	{
		Local_TempMoudelFilBounds = InActorBounds;
		Local_TempOrientation = FRotator::ZeroRotator;
	}

	FVector Local_HitPos;
	FVector Local_ReverseHitPos;
	FVector Local_HalfSize(GetHalfSizeByDirection(InDirection, Local_TempMoudelFilBounds));
	
	TMap<UBuildingComponent*, ECollisionEnabled::Type> Local_AreaComponentMap;
	for (UBuildingComponent* CurrentAreaComponent : AreaComponent)
	{
		Local_AreaComponentMap.Add(CurrentAreaComponent, CurrentAreaComponent->GetCollisionEnabled());
		CurrentAreaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (LaunchRay(WorldContextObject, Local_HitPos, InActorOrigin, InDirection, Local_TempOrientation, Local_TempMoudelFilBounds, ActorsToIgnore))
	{
		for (UBuildingComponent* CurrentAreaComponent : AreaComponent)
		{
			ECollisionEnabled::Type Local_CurrentType = *Local_AreaComponentMap.Find(CurrentAreaComponent);
			CurrentAreaComponent->SetCollisionEnabled(Local_CurrentType);
		}

		NewLocation = CalculateActorPosition(InDirection, Local_HitPos, InActorOrigin, InActorBounds, InActorLoction);

		return true;
	}

	for (UBuildingComponent* CurrentAreaComponent : AreaComponent)
	{
		ECollisionEnabled::Type Local_CurrentType = *Local_AreaComponentMap.Find(CurrentAreaComponent);
		CurrentAreaComponent->SetCollisionEnabled(Local_CurrentType);
	}
	
	return false;
}

void UAJBlueprintFunctionLibrary::GetOverlapActors(UObject* WorldContextObject, const FVector InActorOrigin, const FVector InMoudelFilBounds, const FVector InActorBounds, const FRotator InOrientation, TArray<AActor*> &ActorsToIgnore, TArray<ARoomActor*> &AreaToIgnore,TArray<UBuildingComponent*> &AreaComponent)
{	
	FRotator Local_TempOrientation = InOrientation;
	FVector Local_TempMoudelFilBounds = FVectorAbsValue(InMoudelFilBounds);
	if (Local_TempMoudelFilBounds.X <= 0 || Local_TempMoudelFilBounds.Y <= 0 || Local_TempMoudelFilBounds.Z <= 0)
	{
		Local_TempMoudelFilBounds = InActorBounds;
		Local_TempOrientation = FRotator::ZeroRotator;
	}

	TArray<FHitResult> Local_OutHits;
	TArray<UBuildingComponent*> Local_CurrentAreaComponentList;
	if (MultiLaunchRay(WorldContextObject, Local_OutHits, InActorOrigin, FVector::ZeroVector, Local_TempOrientation, Local_TempMoudelFilBounds, ActorsToIgnore))
	{
		for (FHitResult CurrentOutHit : Local_OutHits)
		{
			ARoomActor* Local_HitArea = Cast<ARoomActor>(CurrentOutHit.Actor);
			if (Local_HitArea != nullptr)
			{
				AreaToIgnore.Add(Local_HitArea);
				UBuildingComponent* Local_CurrentAreaComponent = Cast<UBuildingComponent>(CurrentOutHit.Component);
				if (Local_CurrentAreaComponent != nullptr)
				{
					AreaComponent.Add(Local_CurrentAreaComponent);
					continue;
				}
			}
			ActorsToIgnore.AddUnique((CurrentOutHit.Actor).Get());
		}
	}
}

bool UAJBlueprintFunctionLibrary::IsPolyClockWise(const TArray<FVector2D>& InPolygons)
{
	double d = 0.f;

	for (int i = 0; i < InPolygons.Num(); i++)
	{
		d += -0.5f * ((InPolygons[(i + 1) % InPolygons.Num()].Y + InPolygons[i].Y) * (InPolygons[(i + 1) % InPolygons.Num()].X - InPolygons[i].X));
	}

	return d < 0.f;
}

void UAJBlueprintFunctionLibrary::ChangeClockwise(TArray<FVector2D>& InPolygons)
{
	TArray<FVector2D> Temp;
	Temp.Add(InPolygons[0]);
	for (int i = InPolygons.Num() - 1; i > 0; --i)
	{
		Temp.Add(InPolygons[i]);
	}
	InPolygons.Reserve(Temp.Num());
	InPolygons = Temp;
}


bool UAJBlueprintFunctionLibrary::LaunchRay(UObject * WorldContextObject, FVector& HitPos, const FVector InActorOrigin, const FVector InDirection, const FRotator InOrientation, const FVector InHalfSize, const TArray<AActor*>& ActorsToIgnore)
{
	const int OFFESTVALUE = 20000;
	FHitResult OutHit;
	FVector Local_Start(InActorOrigin);
	FVector Local_End(InActorOrigin + InDirection * OFFESTVALUE);

	static const FName BoxTraceSingleName(TEXT("BoxTraceSingle_Adsorption"));
	FCollisionQueryParams Params(BoxTraceSingleName, SCENE_QUERY_STAT_ONLY(AJBlueprintFunctionLibrary), false);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	//Params.bTraceAsyncScene = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const bool  bHit = World ? World->SweepSingleByChannel(OutHit, Local_Start, Local_End, InOrientation.Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(InHalfSize), Params) : false;
	if (bHit)
	{
		HitPos = OutHit.ImpactPoint;
	}
	return bHit;
}

bool UAJBlueprintFunctionLibrary::MultiLaunchRay(UObject * WorldContextObject,TArray<FHitResult>& OutHits, const FVector InActorOrigin, const FVector InDirection, const FRotator InOrientation, const FVector InHalfSize, const TArray<AActor*>& ActorsToIgnore)
{
	const int OFFESTVALUE = 20000;
	FVector Local_Start(InActorOrigin);
	FVector Local_End(InActorOrigin + InDirection * OFFESTVALUE);

	static const FName BoxTraceSingleName(TEXT("BoxTraceSingle_Adsorption"));
	FCollisionQueryParams Params(BoxTraceSingleName, SCENE_QUERY_STAT_ONLY(AJBlueprintFunctionLibrary), false);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	//Params.bTraceAsyncScene = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const bool  bHit = World ? World->SweepMultiByChannel(OutHits, Local_Start, Local_End, InOrientation.Quaternion(), ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeBox(InHalfSize), Params) : false;

	return bHit;

}

FVector UAJBlueprintFunctionLibrary::CalculateActorPosition(const FVector InDirection, const FVector InHitPos, const FVector InActorOrigin, const FVector InActorBounds, const FVector InActorLocation)
{
	FVector Local_TempDirection = FVectorAbsValue(InDirection);
	FVector Local_RealCheckLocation(InActorOrigin + InActorBounds* InDirection);
	FVector Local_TempRealCheckLocation;
	if (IsPositiveDirection(InDirection))
	{
		Local_TempRealCheckLocation = Local_RealCheckLocation - InActorLocation;
	}
	else
	{
		Local_TempRealCheckLocation = InActorLocation - Local_RealCheckLocation;
	}
	FVector Local_OffsetPos(InHitPos*Local_TempDirection - Local_TempRealCheckLocation * InDirection);
 
	FVector Local_TempActorPosition = InActorLocation - InActorLocation * Local_TempDirection;
	FVector Local_ActorPosition = Local_TempActorPosition + Local_OffsetPos;

	return Local_ActorPosition; 
}

FVector UAJBlueprintFunctionLibrary::FVectorAbsValue(const FVector InFVector)
{
	FVector Local_TempFVector = FVector(FMath::Abs(InFVector.X), FMath::Abs(InFVector.Y), FMath::Abs(InFVector.Z));
	return Local_TempFVector;
}

FVector UAJBlueprintFunctionLibrary::GetHalfSizeByDirection(const FVector InDirection, const FVector InBounds)
{
	FVector Local_HalfSize;
	FVector Local_TempDirection = FVectorAbsValue(InDirection);
	if (Local_TempDirection == FVector::ForwardVector)
	{
		Local_HalfSize = FVector(1, InBounds.Y, InBounds.Z);
	}
	else if (Local_TempDirection == FVector::RightVector)
	{
		Local_HalfSize = FVector(InBounds.X, 1, InBounds.Z);
	}
	else
	{
		Local_HalfSize = FVector(InBounds.X, InBounds.Y, 1);
	}
	return Local_HalfSize;
}

bool UAJBlueprintFunctionLibrary::IsPositiveDirection(const FVector InFVector)
{
	if (InFVector.X>0 || InFVector.Y>0 || InFVector.Z>0)
	{
		return true;
	}
	return false;
}

bool UAJBlueprintFunctionLibrary::ComplexCombineArea(const TArray<FVector2D>& InFristArea, const TArray<FVector2D>& InSecondArea, TArray<FVector2D>& OutCombineArea)
{
	OutCombineArea.Empty();

	TArray<TArray<FVector2D>> AllInnerRegions({ InFristArea, InSecondArea });
	TMap<int32, TArray<TArray<FVector2D>>> IndexMapInnerPs;
	TMap<int32, TArray<int32>> RegionMapRegions;

	for (int32 I = 0; I < AllInnerRegions.Num(); ++I)
	{
		TArray<TArray<FVector2D>> InnerBoundaryPSets;
		TArray<int32> CollectPairRegionIndexs;
		for (int32 J = 0; J < AllInnerRegions.Num(); ++J)
		{
			if (J != I)
			{
				TArray<FVector2D> UpdatePolygonFirst, UpdatePolygonSecond, InterSectPs, CombinedPolygon;
				bool ISuccess = FPolygonAlg::CalculateTwoPolygonInterSect(AllInnerRegions[I], AllInnerRegions[J], UpdatePolygonFirst,
					UpdatePolygonSecond, InterSectPs, CombinedPolygon);
				if (ISuccess)
				{
					InnerBoundaryPSets.Add(InterSectPs);
					CollectPairRegionIndexs.AddUnique(J);
				}
			}
		}
		if (InnerBoundaryPSets.Num() > 0)
		{
			IndexMapInnerPs.Add(I, InnerBoundaryPSets);
			RegionMapRegions.Add(I, CollectPairRegionIndexs);
		}
	}


	TArray<int32> CalculatedIndexs;
	if (IndexMapInnerPs.Num() != 0)
	{
		for (int32 RegionIndex = 0; RegionIndex < AllInnerRegions.Num(); ++RegionIndex)
		{
			TArray<FVector2D> UpdateTempRegionPs;
			UpdateTempRegionPs = AllInnerRegions[RegionIndex];
			if (!CalculatedIndexs.Contains(RegionIndex))
			{
				if (IndexMapInnerPs.Contains(RegionIndex))
				{
					TArray<TArray<FVector2D>> TempGroupInnerPs = IndexMapInnerPs[RegionIndex];
					TArray<int32> PairedRegionIndexs = RegionMapRegions[RegionIndex];
					if (PairedRegionIndexs.Num() == 1)
					{
						CalculatedIndexs.AddUnique(RegionIndex);
						TArray<int32> TempPair = RegionMapRegions[PairedRegionIndexs[0]];
						if (TempPair.Num() == 1)
						{
							TArray<int32> CollectInts;
							CalculatedIndexs.AddUnique(TempPair[0]);
							CollectInts.AddUnique(RegionIndex);
							CollectInts.AddUnique(TempPair[0]);
							TArray<FVector2D> BasePolygon, TPolygonA, TPolygonB, Polygons, InterSectPs;

							BasePolygon = UpdateTempRegionPs;
							TArray<FVector2D> TempPairRegion = AllInnerRegions[PairedRegionIndexs[0]];

							if (!IsPolyClockWise(BasePolygon))
							{
								ChangeClockwise(BasePolygon);
							}
							if (!IsPolyClockWise(TempPairRegion))
							{
								ChangeClockwise(TempPairRegion);
							}
							bool IsInterSect = FPolygonAlg::JudgePolygonInterSect(BasePolygon, TempPairRegion);
							bool HasCommonPs = FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);;
							if ((IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);

							}
							else if ((!IsInterSect) && (HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								InterSectPs.Empty();
								Polygons.Empty();
								FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);
							}
							else if ((!IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);
							}

							if (Polygons.Num() != 0)
							{
								OutCombineArea = Polygons;
							}
						}
						else if (TempPair.Num() >= 2)
						{
							TArray<int32> CollectInts;
							CollectInts.AddUnique(RegionIndex);
							for (int32 I = 0; I < TempPair.Num(); ++I)
							{
								CollectInts.AddUnique(TempPair[I]);
								CalculatedIndexs.AddUnique(TempPair[I]);
								TArray<int32> TempNextPair = RegionMapRegions[TempPair[I]];
								for (int32 J = 0; J < TempNextPair.Num(); ++J)
								{
									CollectInts.AddUnique(TempNextPair[J]);
									CalculatedIndexs.AddUnique(TempNextPair[J]);
								}
							}

							TArray<FVector2D> BasePolygon;
							TArray<FVector2D> Polygons;
							BasePolygon = UpdateTempRegionPs;
							for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
							{
								Polygons.Empty();
								TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
								if (CollectInts[_NumF] != RegionIndex)
								{
									if ((BasePolygon.Num() > 0) && (TempPairRegion.Num() > 0))
									{
										if (!IsPolyClockWise(BasePolygon))
										{
											ChangeClockwise(BasePolygon);
										}
										if (!IsPolyClockWise(TempPairRegion))
										{
											ChangeClockwise(TempPairRegion);
										}
										FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
										BasePolygon = Polygons;
									}
								}
							}
							if (Polygons.Num() != 0)
							{
								OutCombineArea = Polygons;
							}
						}
					}
					else if (PairedRegionIndexs.Num() >= 2)
					{
						TArray<int32> CollectInts;
						CollectInts.AddUnique(RegionIndex);
						CalculatedIndexs.AddUnique(RegionIndex);
						int32 BaseCount = 1, EndCount = 0;
						TArray<int32> UpdateRegionIndexs;
						while (EndCount != BaseCount)
						{
							for (int32 I = 0; I < PairedRegionIndexs.Num(); ++I)
							{
								CollectInts.AddUnique(PairedRegionIndexs[I]);
								CalculatedIndexs.AddUnique(PairedRegionIndexs[I]);
								BaseCount = CollectInts.Num();
								TArray<int32> TempNextPair = RegionMapRegions[PairedRegionIndexs[I]];
								for (int32 P = 0; P < TempNextPair.Num(); ++P)
								{
									if (!CollectInts.Contains(TempNextPair[P]))
									{
										CollectInts.AddUnique(TempNextPair[P]);
										CalculatedIndexs.AddUnique(TempNextPair[P]);
										UpdateRegionIndexs.AddUnique(TempNextPair[P]);
									}
								}
							}

							EndCount = CollectInts.Num();
							if (UpdateRegionIndexs.Num() != 0)
							{
								PairedRegionIndexs.Empty();
								for (int32 J = 0; J < UpdateRegionIndexs.Num(); ++J)
								{
									TArray<int32> TempArray = RegionMapRegions[UpdateRegionIndexs[J]];
									for (int32 K = 0; K < TempArray.Num(); ++K)
									{
										PairedRegionIndexs.AddUnique(TempArray[K]);
									}
								}
							}
						}

						TArray<FVector2D> BasePolygon;
						TArray<FVector2D> Polygons;
						BasePolygon = UpdateTempRegionPs;
						for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
						{
							Polygons.Empty();
							TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
							if (CollectInts[_NumF] != RegionIndex)
							{
								if (!IsPolyClockWise(BasePolygon))
								{
									ChangeClockwise(BasePolygon);
								}
								if (!IsPolyClockWise(TempPairRegion))
								{
									ChangeClockwise(TempPairRegion);
								}
								FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
								BasePolygon = Polygons;
							}
						}
						if (Polygons.Num() != 0)
						{
							OutCombineArea = Polygons;
						}
					}
				}
			}
		}

	}
	else
	{
		TArray<FVector2D> AreaArray;
		TArray<FVector2D> Intersence;

		TArray<FVector2D> PolygonA, PolygonB;
		if (InFristArea.Num() < InSecondArea.Num())
		{
			PolygonA = InFristArea;
			PolygonB = InSecondArea;
		}
		else if (InFristArea.Num() > InSecondArea.Num())
		{
			PolygonA = InSecondArea;
			PolygonB = InFristArea;
		}
		else
		{
			float L1 = 0.0, L2 = 0.0;
			int32 NextI = 0;
			for (int32 I = 0; I < InFristArea.Num(); I++)
			{
				if (I == InFristArea.Num() - 1)
				{
					NextI = 0;
				}
				else
				{
					NextI = I + 1;
				}
				L1 = L1 + FVector2D::Distance(InFristArea[I], InFristArea[NextI]);
			}
			int32 NextJ = 0;
			for (int32 J = 0; J < InSecondArea.Num(); J++)
			{
				if (J == InSecondArea.Num() - 1)
				{
					NextJ = 0;
				}
				else
				{
					NextJ = J + 1;
				}
				L2 = L2 + FVector2D::Distance(InSecondArea[J], InSecondArea[NextJ]);
			}

			if (L1 < L2)
			{
				PolygonA = InFristArea;
				PolygonB = InSecondArea;
			}
			else
			{
				PolygonA = InSecondArea;
				PolygonB = InFristArea;
			}
		}

		TArray<FVector2D> TPolygonA;
		TArray<FVector2D> TPolygonB;
		TArray<FVector2D> Polygons;

		if (!IsPolyClockWise(PolygonA))
		{
			ChangeClockwise(PolygonA);
		}
		if (!IsPolyClockWise(PolygonB))
		{
			ChangeClockwise(PolygonB);
		}


		ARoomActor::PolygonsUnionFunction(PolygonA,
			PolygonB,
			TPolygonA,
			TPolygonB,
			Polygons);


		if (Polygons.Num() != 0)
		{
			TArray<FVector2D> Temp_Pints;
			FPolygonAlg::MergeShortEdgesPreProcess(Polygons, Temp_Pints);
			OutCombineArea = Temp_Pints;
		}
	}

	return OutCombineArea.Num() != 0;
}

void UAJBlueprintFunctionLibrary::DisplayLog(const FString& LogStr)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *LogStr);
}

bool UAJBlueprintFunctionLibrary::AddConfig(FString FileName, FString Key, FString Value)
{
	//固定目录存在 ProjectSavedDir()+ "Config/WindowsNoEditor/" 下
	FString SaveDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + "Config/WindowsNoEditor/";
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
	{
		//如果不存在，创建目录
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SaveDirectory);

		//仍然不能创建目录？
		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
		{
			//无法使用指定的目录
			return false;
			//返回假
		}
	}

	if (FileName == "" || FileName == " ") return false;

	//获得完整的文件路径 projectConfig.ini
	FString FullPath;
	if (!(FileName.EndsWith(".ini") || FileName.EndsWith(".txt")))
		FileName += ".ini";

	FullPath = SaveDirectory + FileName;

	FString SaveText = Key + "=" + Value;

	int ArraySize = 0;
	bool bFindKey = false;
	TArray<FString> StringArray;

	UPeExtendedToolKitBPLibrary::LoadStringArrayFromFile(StringArray, ArraySize, FullPath, true);
	for (FString& Each : StringArray)
	{
		FString Loc_Key, Loc_Value;
		Each.Split("=", &Loc_Key, &Loc_Value);
		if (Key == Loc_Key)
		{
			Each = Key + "=" + Value;
			bFindKey = true;
			break;
		}
	}

	if (!bFindKey)
	{
		StringArray.Add(SaveText);
		return UPeExtendedToolKitBPLibrary::SaveStringArrayToFile(SaveDirectory, FileName, StringArray, true);
	}
	else
	{
		return UPeExtendedToolKitBPLibrary::SaveStringArrayToFile(SaveDirectory, FileName, StringArray, true);
	}
}

bool UAJBlueprintFunctionLibrary::ReadConfig(FString FileName, FString Key, FString& OutValue)
{
	FString File;
	FString SaveDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + "Config/WindowsNoEditor/";

	if (FileName == "" || FileName == " ") return false;

	//获得完整的文件路径 projectConfig.ini
	FString FullPath;
	if (FileName.EndsWith(".ini") || FileName.EndsWith(".txt"))
		FullPath = SaveDirectory + FileName;
	else
		FullPath = SaveDirectory + FileName + ".ini";

	TArray<FString> StringArray;
	int ArraySize;
	if (!UPeExtendedToolKitBPLibrary::LoadStringArrayFromFile(StringArray, ArraySize, FullPath, true))
		return false;

	bool bFindKey = false;
	for (FString& Each : StringArray)
	{
		FString Loc_Key, Loc_Value;
		Each.Split("=", &Loc_Key, &Loc_Value);
		if (Key == Loc_Key)
		{
			OutValue = Loc_Value;
			bFindKey = true;
			break;
		}
	}

	return bFindKey;
}

bool UAJBlueprintFunctionLibrary::GetBPInt(const UObject* Obj, FString PropertyName, int32& PropertyValue)
{
	if (Obj)
	{
		UIntProperty* Property = FindField<UIntProperty>(Obj->GetClass(), *PropertyName);
		if (Property)
		{
			PropertyValue = (Property->GetPropertyValue_InContainer(Obj));
			return true;
		}
	}
	return false;
}