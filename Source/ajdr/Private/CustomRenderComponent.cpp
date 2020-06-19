// Copyright? 2017 ihomefnt All Rights Reserved.

#include "CustomRenderComponent.h"
#include "PrimitiveViewRelevance.h"
#include "SceneManagement.h"
#include "PrimitiveSceneProxy.h"

UCustomRenderComponent::UCustomRenderComponent(const FObjectInitializer& ObjectInitializer)
{
	bShowAxis = false;
	bShowBounds = false;
}

FPrimitiveSceneProxy * UCustomRenderComponent::CreateSceneProxy()
{
	return new FCustomRenderSceneProxy(this);
}

void FCustomRenderSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily & ViewFamily, uint32 VisibilityMap, FMeshElementCollector & Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_PDIDrawSceneProxy_GetDynamicMeshElements);
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];
			FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
			OnDraw(PDI);
		}
	}
}

FPrimitiveViewRelevance FCustomRenderSceneProxy::GetViewRelevance(const FSceneView * View) const
{
	FPrimitiveViewRelevance ViewRelevance;

	ViewRelevance.bDrawRelevance = IsShown(View);
	ViewRelevance.bShadowRelevance = IsShadowCast(View);
	ViewRelevance.bDisableOffscreenRendering = false;
	ViewRelevance.bDynamicRelevance = true;
	// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
	//ViewRelevance.bSeparateTranslucencyRelevance = ViewRelevance.bNormalTranslucencyRelevance = true;
	return ViewRelevance;
}

SIZE_T FCustomRenderSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

uint32 FCustomRenderSceneProxy::GetMemoryFootprint(void) const
{
	return(sizeof(*this) + GetAllocatedSize());
}

uint32 FCustomRenderSceneProxy::GetAllocatedSize(void) const
{
	return FPrimitiveSceneProxy::GetAllocatedSize();
}

void FCustomRenderSceneProxy::OnDraw(FPrimitiveDrawInterface * PDI) const
{
	if (Component)
	{
		if (Component->bShowAxis)
		{
			FVector ComponentLocation = Component->GetComponentLocation();

			FTransform XAxisTransform(FVector(1.0f, 0, 0), FVector(0, 1.0f, 0), FVector(0, 0, 1.0f), ComponentLocation);
			DrawDirectionalArrow(PDI, XAxisTransform.ToMatrixWithScale(), FLinearColor::Red, Component->AxisLength, Component->AxisSize, SDPG_Foreground);

			FTransform YAxisTransform(FVector(0, 1.0f, 0), FVector(0, 0, 1.0f), FVector(1.0f, 0, 0), ComponentLocation);
			DrawDirectionalArrow(PDI, YAxisTransform.ToMatrixWithScale(), FLinearColor::Green, Component->AxisLength, Component->AxisSize, SDPG_Foreground);

			FTransform ZAxisTransform(FVector(0, 0, 1.0f), FVector(1.0f, 0, 0), FVector(0, 1.0f, 0), ComponentLocation);
			DrawDirectionalArrow(PDI, ZAxisTransform.ToMatrixWithScale(), FLinearColor::Blue, Component->AxisLength, Component->AxisSize, SDPG_Foreground);
		}

		if (Component->bShowBounds)
		{
			DrawWireBox(PDI, Component->BoxBounds, FLinearColor::White, SDPG_Foreground, Component->BoxThickness);
		}
	}
}
