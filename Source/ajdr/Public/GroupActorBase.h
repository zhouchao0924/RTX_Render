// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Struct.h"
#include "GameFramework/Actor.h"
#include "GroupActorBase.generated.h"

UCLASS(BlueprintType)
class AJDR_API AGroupActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroupActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool Init();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool AddActorToGroup(AActor * Target);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool RemoveActorFromGroup(AActor * Target);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		TArray<AActor*> Copy();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool Select(bool bFromUI = false);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool Polymerization();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool Clean();

	//Set The GroupActor Location To  All Object Centres
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GroupActor)
		bool SetGroupActorPosition();

	UFUNCTION(BlueprintCallable, Category = GroupActor)
		void Lock();

	UFUNCTION(BlueprintCallable, Category = GroupActor)
		void Unlock();

	UFUNCTION(BlueprintPure, Category = GroupActor)
		bool IsLocked();

	// Check if select the given actor does should change to select the group.
	UFUNCTION(BlueprintPure, Category = GroupActor)
		bool ShouldHandleSelect(AActor* InActor);

	// Check the given actor is in this group.
	UFUNCTION(BlueprintPure, Category = GroupActor)
		bool CheckIsInGroup(AActor* InActor);

	// Check whether this group is the root group.
	UFUNCTION(BlueprintPure, Category = GroupActor)
		bool CheckIsRoot();

	// Calculate the actor count in the group, ignore group actor.
	UFUNCTION(BlueprintPure, Category = GroupActor)
		int32 CalculateActorCount();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupActor)
		TArray<AActor*> GroupActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GroupActor)
		TArray<AGroupActorBase*> GroupParentReference;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupActor)
		bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = GroupActor)
		FGroupActorInfo	GroupInfo;
};
