// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingSystem.h"
#include "Building.h"
#include "FeatureWalls.generated.h"


UCLASS()
class UFeatureWall : public UObject
{
	GENERATED_BODY()
	
public:
	static float  EPSILON0;
	static float  EPSILON1;
	struct WallAnnex
	{
		EObjectType type;
		float width;
		float height;
		float thickness;
		float zPos;
		kVector3D location;
		kRotation rotation;
		kVector3D scaling;

		WallAnnex()
		{
			type = EUnkownObject;
			width = 0;
			height = 0;
			thickness = 0;
			zPos = 0;
			location.X = location.Y = location.Z = 0;
			rotation.Pitch = rotation.Yaw = rotation.Roll = 0;
			scaling.X = scaling.Y = scaling.Z = 0;
		}
		WallAnnex(const WallAnnex& o)
		{
			type = o.type;
			width = o.width;
			height = o.height;
			thickness = o.thickness;
			zPos = o.zPos;
			location = o.location;
			rotation = o.rotation;
			scaling = o.scaling;
		}
		WallAnnex& operator=(const WallAnnex& o)
		{
			type = o.type;
			width = o.width;
			height = o.height;
			thickness = o.thickness;
			zPos = o.zPos;
			location = o.location;
			rotation = o.rotation;
			scaling = o.scaling;

			return *this;
		}
		bool operator==(const WallAnnex& o) const
		{
			return (
				type == o.type &&
				Equals_Float(width, o.width, K_ROUNDING_ERROR) &&
				Equals_Float(height, o.height, K_ROUNDING_ERROR) &&
				Equals_Float(thickness, o.thickness, K_ROUNDING_ERROR) &&
				Equals_Float(zPos, o.zPos, K_ROUNDING_ERROR) &&
				location == o.location &&
				Equals_Float(rotation.Pitch, o.rotation.Pitch, K_ROUNDING_ERROR) && Equals_Float(rotation.Yaw, o.rotation.Yaw, K_ROUNDING_ERROR) && Equals_Float(rotation.Roll, o.rotation.Roll, K_ROUNDING_ERROR) &&
				scaling == o.scaling
				);
		}
	};
	struct WallInfo
	{
		kPoint forward;
		kPoint start;
		kPoint end;
		bool inverse;
		TArray<WallAnnex> annex;

		WallInfo()
		{
			forward.X = forward.Y = 0;
			start.X = start.Y = 0;
			end.X = end.Y = 0;
			inverse = false;
			annex.Empty();
		}
		WallInfo(const WallInfo& o)
		{
			forward = o.forward;
			start = o.start;
			end = o.end;
			inverse = o.inverse;
			annex = o.annex;
		}
		WallInfo& operator=(const WallInfo& o)
		{
			forward = o.forward;
			start = o.start;
			end = o.end;
			inverse = o.inverse;
			annex = o.annex;

			return *this;
		}
		bool operator==(const WallInfo& o) const
		{
			//return (forward == o.forward && start == o.start && end == o.end && annex == o.annex);
			return (annex == o.annex);
		}
		float GetLength() const
		{
			return (end - start).Size();
		}
		bool HasAnnex() const
		{
			return (annex.Num() > 0);
		}
		
	};
	struct WallInfoEx : public WallInfo
	{
		ObjectID id;
		EObjectType type;
		//kVector3D location;
		//kRotation rotation;
		//kVector3D scaling;

		int index;
		TArray<int> vID;
		bool hasIgnoreWithStart;
		bool hasIgnoreWithEnd;

		WallInfoEx()
		{
			forward.X = forward.Y = 0;
			start.X = start.Y = 0;
			end.X = end.Y = 0;
			inverse = false;
			annex.Empty();
			//
			id = 0;
			type = EObjectType::EUnkownObject;
			//
			index = 0;
			vID.Empty();
			hasIgnoreWithStart = false;
			hasIgnoreWithEnd = false;
		}
		WallInfoEx(const WallInfoEx& o)
		{
			forward = o.forward;
			start = o.start;
			end = o.end;
			inverse = o.inverse;
			annex = o.annex;
			//
			id = o.id;
			type = o.type;
			//
			index = o.index;
			vID = o.vID;
			hasIgnoreWithStart = o.hasIgnoreWithStart;
			hasIgnoreWithEnd = o.hasIgnoreWithEnd;
		}
		WallInfoEx& operator=(const WallInfoEx& o)
		{
			forward = o.forward;
			start = o.start;
			end = o.end;
			inverse = o.inverse;
			annex = o.annex;
			//
			id = o.id;
			type = o.type;
			//
			index = o.index;
			vID = o.vID;
			hasIgnoreWithStart = o.hasIgnoreWithStart;
			hasIgnoreWithEnd = o.hasIgnoreWithEnd;

			return *this;
		}
		bool operator ==(const WallInfoEx& o) const
		{
			return (o.start.Equals(start) && o.end.Equals(end));
		}
		bool operator <(const WallInfoEx& o) const
		{
			return (GetLength() < o.GetLength());
		}
		bool operator()(const WallInfoEx& A, const WallInfoEx& B) const
		{
			return (A < B);
		}
		bool operator==(int key) const
		{
			if (id == key)
				return true;
			for (int i = 0; i < vID.Num(); i++)
			{
				if (vID[i] == key)
				{
					return true;
				}
			}
			return false;
		}
		WallInfo GetInfo() const
		{
			WallInfo info;
			info.forward = forward;
			info.start = start;
			info.end = end;
			info.inverse = inverse;
			info.annex = annex;
			return info;
		}
		bool IsConnectWith(const WallInfoEx& o) const
		{
			return (end.Equals(o.start));
		}
		bool ConnectState(const WallInfoEx& o, int* flag = nullptr) const
		{
			const bool a0 = end.Equals(o.start, EPSILON1);
			const bool a1 = start.Equals(o.end, EPSILON1);
			const bool b0 = start.Equals(o.start, EPSILON1);
			const bool b1 = end.Equals(o.end, EPSILON1);
			if (nullptr != flag)
			{
				if (a0)
					*flag = 0;
				else if (a1)
					*flag = 1;
				else if (b0)
					*flag = 2;
				else if (b1)
					*flag = 3;
			}

			return (a0 || a1 || b0 || b1);
		}
		bool IsParallel(const WallInfoEx& o, float & val)
		{
			FVector v0 = FVector(end.X, end.Y, 0) - FVector(start.X, start.Y, 0);
			const bool b0 = v0.Normalize();
			FVector v1 = FVector(o.end.X, o.end.Y, 0) - FVector(o.start.X, o.start.Y, 0);
			const bool b1 = v1.Normalize();
			//if (!b0 || !b1)
			//	return false;
			return  FVector::Parallel(v0, v1, EPSILON0);
		}
		bool IsLine(const WallInfoEx& o, int* flag = nullptr)
		{
			float val = 0.f;
			bool a = ConnectState(o, flag);
			bool b = IsParallel(o, val);
			return (a && b);
		}
	};

public:
	// Sets default values for this actor's properties
	UFeatureWall();

	 UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	 static bool InitData(int RoomID);

	 UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	 bool Select(const FVector2D &Location);

	 const TArray<UFeatureWall::WallInfo>& GetInfo() const;

	 static TArray<UFeatureWall::WallInfo> GetWallInfo(UWorld * mpWorld, int RoomID);
	 static bool SelectWall(TArray<UFeatureWall::WallInfo>& vOutInfo, const FVector2D &Location, const TArray<UFeatureWall::WallInfo>& vWallInfo);
	 static UFeatureWall* Singleton();
	 void SetWorld(UWorld *pWorld);

	 UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	 void DrawEdge(float Lifetime = 10.f, float Thickness = 10.f) const;

public:
	//// test
	// UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	// bool GetWalls(int RoomID);


// protected:
	// // Called when the game starts or when spawned
	// virtual void BeginPlay() override;

// public:	
	// // Called every frame
	// virtual void Tick(float DeltaTime) override;

private:
	//bool ProceWalls(const TArray<int>& vID);
	static bool ProceWall(ISuite * pSuite, UFeatureWall::WallInfoEx & info, int id, bool bInverse);
	static bool Combine(UFeatureWall::WallInfoEx* pWall , const UFeatureWall::WallInfoEx& temp);
	static void Filter(UFeatureWall::WallInfoEx* pPre, UFeatureWall::WallInfoEx* pNext, const TArray<int>& vShort);
	static void tempCopy();

public:
	struct TempData
	{
		TArray<UFeatureWall::WallInfo> mvOutInfo;
		ISuite * mpSuite;
		kArray<ObjectID> mvWallID;
		kArray<unsigned char> mvInverse;
		TArray<UFeatureWall::WallInfoEx> mvWallInfo;
		TArray<UFeatureWall::WallInfoEx> mvWallRaw;
		//
		UWorld * mpWorld;
	};
	static TempData gpData;

private:
	//TArray<UFeatureWall::WallInfo> mvOutInfo;
	//ISuite * mpSuite;
	//kArray<ObjectID> mvWallID;
	//TArray<UFeatureWall::WallInfoEx> mvWallInfo;
	//TArray<UFeatureWall::WallInfoEx> mvWallRaw;
	////
	//UWorld * mpWorld;

};

UCLASS()
class AWallWrapper : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWallWrapper();

	//test
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	UFeatureWall* GetFeatureWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//private:
	UPROPERTY(BlueprintReadWrite)
	UFeatureWall* mpFeatureWall;

};
