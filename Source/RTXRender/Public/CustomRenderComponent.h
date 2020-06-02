// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/ObjectMacros.h"
#include "CustomRenderComponent.generated.h"

class FPrimitiveSceneProxy;

UCLASS( ClassGroup=(Custom), hidecategories = (Object, LOD, Lighting, TextureStreaming), meta=(BlueprintSpawnableComponent, DisplayName = "CustomRender"))
class RTXRENDER_API UCustomRenderComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	
	UCustomRenderComponent(const FObjectInitializer& ObjectInitializer);

	virtual FPrimitiveSceneProxy* CreateSceneProxy();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		bool		bShowAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		bool		bShowBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		FBox		BoxBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		float		BoxThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		float		AxisLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomRender)
		float		AxisSize;
};

class FCustomRenderSceneProxy : public FPrimitiveSceneProxy
{
public:

	FCustomRenderSceneProxy(const UCustomRenderComponent* InComponent)
		:FPrimitiveSceneProxy(InComponent)
	{
		Component = InComponent;
		bWillEverBeLit = false;
	};

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	SIZE_T GetTypeHash() const override;

	virtual uint32 GetMemoryFootprint(void) const override;

	virtual bool CanBeOccluded() const override
	{
		return false;
	}

	uint32 GetAllocatedSize(void) const;

	void OnDraw(FPrimitiveDrawInterface* PDI) const;

public:

	const UCustomRenderComponent* Component;
};