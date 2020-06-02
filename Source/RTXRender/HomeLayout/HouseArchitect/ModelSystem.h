#pragma once

#include "../SceneEntity/FurnitureModelActor.h"
#include "ModelSystem.generated.h"


UCLASS(BlueprintType, Blueprintable)
class AJDR_API AModelSystem : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AModelSystem();
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	const TArray<AFurnitureModelActor*>& GetModelsInWorld() const;

	friend class AFurnitureModelActor;
	void OnModelActorSpawned(AFurnitureModelActor* Actor);

	void OnModelActorDestroyed(AFurnitureModelActor* Actor);

private:
	UPROPERTY()
	TArray<AFurnitureModelActor*> ModelsInWorld;
};
