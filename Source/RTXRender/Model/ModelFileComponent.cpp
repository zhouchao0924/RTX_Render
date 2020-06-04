
#include "ModelFileComponent.h"
#include "ResourceMgr.h"
#include "DRGameMode.h"
#include "Building/DRFunLibrary.h"
#include "Model/ModelFile.h"
#include "Model/ModelPartComponent.h"
#include <corecrt_math.h>

UModelFileComponent::UModelFileComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	, AdjustType(ECenterAdjustType::Original)
	, BuildData(nullptr)
{
}

void UModelFileComponent::SetData(UBuildingData * Data,FDRComplexModel Model)
{
	BuildData = Data;
	ComplexModel = Model;
}

void UModelFileComponent::SetModelAlignType(ECenterAdjustType InAdjustType)
{
	UModelFile *Model = GetModel();
	if (Model)
	{
		FTransform RelativeModelTransform = GetOffset(Model, InAdjustType);
		for (int32 i = 0; i < Parts.Num(); ++i)
		{
			UModelPartComponent *Comp = Parts[i];
			if (Comp)
			{
				Comp->SetRelativeTransform(RelativeModelTransform);
			}
		}
	}
	AdjustType = InAdjustType;
}

FTransform UModelFileComponent::GetOffset(UModelFile *Model, ECenterAdjustType AlignType)
{
	FVector Offset = Model->GetOffsetByCenterType(AlignType);
	return FTransform(FRotator(0, 180.0f, 0), Offset, Model->Scale3D);
}

FTransform UModelFileComponent::GetOffset(const FBox &Bounds, const FVector &Scale3D, ECenterAdjustType AlignType)
{
	FVector Offset = UModelFile::GetOffsetByCenterType(Bounds, AlignType);
	return FTransform(FRotator(0, 180.0f, 0), Offset, Scale3D);
}

FTransform UModelFileComponent::GetRelativeTransformModeFile(UModelFile *Model)
{
	if (AdjustType == ECenterAdjustType::Original)
	{
		if (fpclassify(Model->Scale3D.Z) == FP_NAN)
		{
			Model->Scale3D.Z = 1.0f;
		}
		if (fpclassify(Model->Offset.Z) == FP_NAN)
		{
			Model->Offset.Z = 1.0f;
		}
		return FTransform(FRotator(0, 180.0f, 0), Model->Offset, Model->Scale3D);
	}
	return  GetOffset(Model, AdjustType);
}

void UModelFileComponent::AddClipPlane(const FPlane &InPlane)
{
	ClipPlanes.Add(InPlane);
}

void UModelFileComponent::UpdateModel(UModelFile *Model)
{
	if (Model!=NULL)
	{
		for (int32 i = 0; i < Parts.Num(); ++i)
		{
			UModelPartComponent *Comp = Parts[i];
			Comp->DestroyComponent();
		}

		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();

		Parts.Empty();
		FTransform RelativeModelTransform = GetRelativeTransformModeFile(Model);

		FVector Size = Model->GetLocalBounds().GetSize();

		for (int i = 0; i < Model->m_Models.Num(); ++i)
		{
			FModel *SubModel = Model->m_Models[i];
			int32 AuxPrefixIndex = SubModel? SubModel->ModelName.Find(TEXT("aux_")) : 0;
			if (AuxPrefixIndex == 0)
			{
				continue;
			}

			UModelPartComponent *Comp = NewObject<UModelPartComponent>(GetOwner());
			if (Comp)
			{
				Comp->RegisterComponentWithWorld(GetWorld());
				Comp->SetRelativeTransform(RelativeModelTransform);
				Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
				Comp->UpdateModel(ClipPlanes, Forward, Size.X, Model, i);
				Parts.Add(Comp);
			}
		}

		ModelResID = Model->GetResID();
		if (UpdateModelDone.IsBound()) {
			UpdateModelDone.Broadcast();
		}
	}
}

void UModelFileComponent::ChangeModelData(const int32& ModelID, FString& ResID)
{
	ComplexModel.ModelID = ModelID;
	ComplexModel.ResID = ResID;
	if (BuildData)
	{
		UBuildingSystem* BS = BuildData->GetBuildingSystem();
		if (BS)
		{
			FString ModelIDStr = TEXT("ModelID");
			FString ResIDStr = TEXT("ResID");
			UDRFunLibrary::SetObjIntValue(BS, BuildData->GetID(), ModelIDStr, ModelID);
			UDRFunLibrary::SetObjFStringValue(BS, BuildData->GetID(), ResIDStr, ResID);
		}
	}
	
}

void UModelFileComponent::ChangeModel(const FString& ResID)
{
	UResourceMgr* ResMgr = UResourceMgr::GetResourceMgr();
	UResource* Res = ResMgr->FindRes(ResID, false);
	if (Res)
	{
		Res->ForceLoad();
		UModelFile* ModelFile = Cast<UModelFile>(Res);
		if (ModelFile)
		{
			UpdateModel(ModelFile);
		}
	}
}

void UModelFileComponent::ChangeModelMaterial(const FString& ResID, int32 MaterialIndex)
{
	 UMaterialInterface* UE4Mat = UDRFunLibrary::GetUE4MatByResID(ResID, 0);
	 if (UE4Mat)
	 {
		 TArray<USceneComponent*> Comps = GetAttachChildren();
		 for (USceneComponent* Item : Comps)
		 {
			 UModelPartComponent* PComp = Cast<UModelPartComponent>(Item);
			 if (PComp)
			 {
				if(MaterialIndex == -1)
				{ 
					for (int i = 0; i < PComp->GetMaterials().Num(); i++)
					{
						PComp->UpdateSurfaceMaterial(i, UE4Mat);
					}
				}
				else if (MaterialIndex < PComp->GetMaterials().Num())
				{
					PComp->UpdateSurfaceMaterial(MaterialIndex, UE4Mat);
				}
			 }
		 }
	 }
}

FBox UModelFileComponent::GetWorldBounds()
{
	FBox WorldBounds;
	FTransform WorldTransform = GetComponentTransform();
	WorldBounds = LocalBounds.TransformBy(WorldTransform);
	return WorldBounds;
}

UModelFile *UModelFileComponent::GetModel()
{
	UResourceMgr *ResMgr = UResourceMgr::Instance(this);
	UModelFile *ModelFile = Cast<UModelFile>(ResMgr ? ResMgr->FindRes(ModelResID, true) : NULL);
	if (ModelFile)
	{
		ModelFile->ForceLoad();
	}
	return ModelFile;
}

void UModelFileComponent::UpdateMaterial(int32 MaterialIndex)
{
	UModelFile *ModelFile = GetModel();
	if (ModelFile)
	{
		ModelFile->ForceLoad();
		FModelMaterial *material = ModelFile->GetMaterial(MaterialIndex);
		UResourceMgr *ResMgr = UResourceMgr::Instance(this);
		if (material->GetType() == EMaterialType::EMaterialRef) {
			FModelMaterialRef *refMtrl = (FModelMaterialRef*)material;
		int32 *pSlotIndex = ResMgr->ResidResMap.Find(refMtrl->BaseSkuid);
		if (pSlotIndex) {
			FResourceInfo &Info = ResMgr->PooledResource[*pSlotIndex];
			Info.Resource->ForceLoad();
		}
		}
	
		TArray<FMaterialSlot> materialSlots;
		ModelFile->GetMaterialInfo(MaterialIndex, materialSlots);

		for (int32 i = 0; i < materialSlots.Num(); ++i)
		{
			FMaterialSlot &slot = materialSlots[i];
			UModelPartComponent *Part = GetPart(slot.iModel);
			if (Part)
			{
				UMaterialInterface *Ue4Material = ModelFile->GetUE4Material(MaterialIndex);
				Part->UpdateSurfaceMaterial(slot.iSection, Ue4Material);
			}
		}
	}
}

void UModelFileComponent::SetMaterialByPart(int32 iPart, UMaterialInterface *Material)
{
	if (Parts.IsValidIndex(iPart))
	{
		UModelPartComponent *PartComp = Parts[iPart];
		if (PartComp)
		{
			PartComp->UpdateSurfaceMaterial(INDEX_NONE, Material);
		}
	}
}

void UModelFileComponent::SetMaterialByChannel(int32 iMtrlChannel, UMaterialInterface *Material)
{
	UModelFile *ModelFile = GetModel();
	if (ModelFile && Material)
	{
		TArray<FMaterialSlot> materialSlots;
		ModelFile->GetMaterialInfo(iMtrlChannel, materialSlots);

		for (int32 i = 0; i < materialSlots.Num(); ++i)
		{
			FMaterialSlot &slot = materialSlots[i];
			UModelPartComponent *Part = GetPart(slot.iModel);
			if (Part)
			{
				Part->UpdateSurfaceMaterial(slot.iSection, Material);
			}
		}
	}
}


UModelPartComponent *UModelFileComponent::GetPart(int32 Index)
{
	if (Parts.IsValidIndex(Index))
	{
		return Parts[Index];
	}
	return NULL;
}

void UModelFileComponent::UpdatePhysics()
{
	UModelFile *ModelFile = GetModel();
	if (ModelFile)
	{
		for (int32 i = 0; i < Parts.Num(); ++i)
		{
			UModelPartComponent *PartComp = Parts[i];
			if (ModelFile->m_Models.IsValidIndex(i))
			{
				FModel *Model = ModelFile->m_Models[i];
				PartComp->UpdatePhysics(Model->AggGeom);
			}
		}
	}
}

void UModelFileComponent::SetSimulatePhysic(bool bSimulate)
{
	for (int32 i = 0; i < Parts.Num(); ++i)
	{
		UModelPartComponent *PartComp = Parts[i];
		if (PartComp)
		{
			if (bSimulate)
			{
				PartComp->SetMobility(bSimulate ? EComponentMobility::Movable : EComponentMobility::Static);
			}
			PartComp->SetSimulatePhysics(bSimulate);
		}
	}
}

void UModelFileComponent::SetAffectByShadow(bool bAffectByShadow)
{
	for (int32 i = 0; i < Parts.Num(); ++i)
	{
		UModelPartComponent *PartComp = Parts[i];
		if (PartComp)
		{
			PartComp->SetCastShadow(bAffectByShadow);
			PartComp->bAffectDistanceFieldLighting = bAffectByShadow;
			PartComp->bAffectDynamicIndirectLighting = bAffectByShadow;
			PartComp->MarkRenderStateDirty();
		}
	}
}

void UModelFileComponent::SetCastShadow(bool bCastShadow)
{
	for (int32 i = 0; i < Parts.Num(); ++i)
	{
		UModelPartComponent *PartComp = Parts[i];
		if (PartComp)
		{
			PartComp->SetCastShadow(bCastShadow);
			PartComp->bCastFarShadow = bCastShadow;
			PartComp->bCastStaticShadow = bCastShadow;
			PartComp->bCastDynamicShadow = bCastShadow;
			PartComp->MarkRenderStateDirty();
		}
	}
}

void UModelFileComponent::DestroyComponent(bool bPromoteChildren /* = false */)
{
	for (int32 i = 0; i < Parts.Num(); ++i)
	{
		UModelPartComponent *PartComp = Parts[i];
		if (PartComp)
		{
			PartComp->DestroyComponent(bPromoteChildren);
		}
	}
	Super::DestroyComponent(bPromoteChildren);
}

