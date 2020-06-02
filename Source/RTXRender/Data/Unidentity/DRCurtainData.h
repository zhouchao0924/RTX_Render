
#pragma once

#include "DRData.h"
#include "DRCurtainData.generated.h"

UCLASS(BlueprintType)
class UDRCurtainData :public UDRData
{
	GENERATED_UCLASS_BODY()
public:
	int32 GetChunkID() override { return CHUNK_DR_CurtainData; }

	UFUNCTION(BlueprintPure)
	FString GetRomebar1() { return Romebar1; }

	UFUNCTION(BlueprintPure)
	FString GetCloth1() { return Cloth1; }

	UFUNCTION(BlueprintPure)
	FString GetRomebar2() { return Romebar2; }

	UFUNCTION(BlueprintPure)
	FString GetCloth2() { return Cloth2; }

	UFUNCTION(BlueprintPure)
	float GetLength() { return Length; }

	UFUNCTION(BlueprintPure)
	float GetHeight() { return Height; }

	UFUNCTION(BlueprintPure)
	float GetRomerbarHeadLength() { return RomerbarHeadLength; }

	UFUNCTION(BlueprintPure)
	float GetRomerbarBodyLength() { return RomerbarBodyLength; }

	UFUNCTION(BlueprintPure)
	float GetDistanceBetweenBar() { return DistanceBetweenBar; }

	UFUNCTION(BlueprintPure)
	bool GetIsBlind() { return IsBlind; }

	UFUNCTION(BlueprintPure)
	float GetBlindLength() { return BlindLength; }

	UFUNCTION(BlueprintPure)
	int32 GetClothPosition() { return ClothPosition; }

	void SerializeDataByVersion(ISerialize &Ar, int32 Ver) override;
protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	FTransform						WorldTransform;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Romebar1;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Cloth1;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Romebar2;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Cloth2;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							Length;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							Height;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							RomerbarHeadLength;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							RomerbarBodyLength;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							DistanceBetweenBar;
	UPROPERTY(Transient, BlueprintReadOnly)
	bool							IsBlind;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							BlindLength;
	UPROPERTY(Transient, BlueprintReadOnly)
	int32							ClothPosition;
};

