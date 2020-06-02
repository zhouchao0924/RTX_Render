
#include "ResourceMeshComponent.h"
#include "BuildingGroupMeshComponent.h"

#define  USE_COMPONENT_POOL	0

UResourceMeshComponent::UResourceMeshComponent(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, InstanceID(INVALID_OBJID)
{
}

IBuildingResourceMgr *UResourceMeshComponent::GetResourceMgr()
{
	FBuildingSDKModule &SDKModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
	IBuildingSDK *SDK = SDKModule.GetSDK();
	if (SDK)
	{
		return SDK->GetResourceMgr();
	}
	return nullptr;
}

void UResourceMeshComponent::SetResource(int32 ResourceID)
{
	IBuildingResourceMgr *Mgr = GetResourceMgr();

	if (!Resource.Equals(ResourceID))
	{
		if (SolutionComponent && InstanceID != INVALID_OBJID)
		{
			SolutionComponent->Delete(InstanceID);
			InstanceID = INVALID_OBJID;
		}

		DestoryComponents();

		Resource = Mgr->GetResource(ResourceID);
		if (Resource.SetWatcher(CALLBACK_3(UResourceMeshComponent::OnUpdateResource, this)))
		{
			if (Resource.IsReady())
			{
				OnUpdateResource(Resource.Get(), ELoaded, nullptr);
			}
		}
	}
}

void UResourceMeshComponent::OnUpdateResource(IObject *RawObj, EFuncType FuncType, IValue *pValue)
{
	if (RawObj && Resource.IsReady())
	{
		if (SolutionComponent)
		{
			if (FuncType == ELoaded)
			{
				InstanceID = SolutionComponent->AddModelByObjectID(Resource.GetID(), FTransform::Identity);
				Instance = SolutionComponent->GetSDKObject(InstanceID);
				Instance.SetWatcher(CALLBACK_3(UResourceMeshComponent::OnUpdateObject, this));
			}
		}
		else
		{
			UpdateComponents(Resource.Get());
		}
	}
}

void UResourceMeshComponent::OnUpdateObject(IObject *RawObj, EFuncType FuncType, IValue *pValue)
{
	if (RawObj!=nullptr)
	{
		if (FuncType == EAddToScene)
		{
			OnRegisterComponent(RawObj);
		}
		else if(FuncType== EUpdate||FuncType==EUpdateResource)
		{
			if (MeshComponents.Num() > 0)
			{
				UpdateComponents(RawObj);
			}
		}
	}
}

void UResourceMeshComponent::OnUnregister()
{
	Super::OnUnregister();

	DestoryComponents();
}

void UResourceMeshComponent::DestoryComponents()
{
	for (int32 i = MeshComponents.Num() - 1; i >= 0; --i)
	{
		UBuildingGroupMeshComponent *Comp = MeshComponents[i];
		if (Comp != nullptr)
		{
#if USE_COMPONENT_POOL
			UBGCPoolManager::FreeComponent(Comp);
#else
			Comp->DestroyComponent(true);
#endif
		}
	}

	MeshComponents.Empty();
}

void UResourceMeshComponent::UpdateComponents(IObject *RawObj)
{
	DestoryComponents();
	OnRegisterComponent(RawObj);
}

void UResourceMeshComponent::OnRegisterComponent(IObject *pObj)
{
	INode *pNode = pObj ? pObj->GetNode() : nullptr;
	if (pNode)
	{
		AActor *pOwner = GetOwner();
#if USE_COMPONENT_POOL
		UBuildingGroupMeshComponent *MeshComponent = UBGCPoolManager::Instance()->AllocComponent(pOwner);
#else
		FString Name = FString::Printf(TEXT("UBuildingGroupMeshComponent_%d"), pNode->GetID());
		UBuildingGroupMeshComponent *MeshComponent = NewObject<UBuildingGroupMeshComponent>(pOwner, *Name);
#endif
		MeshComponent->RegisterComponentWithWorld(GetWorld());
		MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		MeshComponent->UpdateNode(pNode);
		MeshComponents.Add(MeshComponent);
	}
}

UBuildingGroupMeshComponent *UResourceMeshComponent::AddComponentToWorld(USceneComponent *ParentMeshComponent, IGroup *Group)
{
	if (Group && Group->GetOwner())
	{
		kXform xForm = Group->GetLocalTranform();
		FTransform LocalTransform = FTransform(FORCE_TYPE(FRotator, xForm.Rotation), FORCE_TYPE(FVector, xForm.Location), FORCE_TYPE(FVector, xForm.Scale));

		AActor *pOwner = GetOwner();
#if USE_COMPONENT_POOL
		UBuildingGroupMeshComponent *MeshComponent = UBGCPoolManager::Instance()->AllocComponent(pOwner);
#else
		FString Name = FString::Printf(TEXT("UBuildingGroupMeshComponent_%d"), Group->GetID());
		UBuildingGroupMeshComponent *MeshComponent = NewObject<UBuildingGroupMeshComponent>(pOwner, *Name);
#endif
		MeshComponent->RegisterComponentWithWorld(GetWorld());
		MeshComponent->SetRelativeTransform(LocalTransform);
		MeshComponent->AttachToComponent(ParentMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
		MeshComponent->UpdateNode(Group);
		MeshComponents.Add(MeshComponent);

		return MeshComponent;
	}
	return nullptr;
}

UBuildingGroupMeshComponent *UResourceMeshComponent::FindComponentByElementID(int32 InElemID)
{
	for (int32 i = 0; i < MeshComponents.Num(); ++i)
	{
		UBuildingGroupMeshComponent *Comp = MeshComponents[i];
		if (Comp && Comp->GetID()== InElemID)
		{
			return Comp;
		}
	}
	return nullptr;
}


