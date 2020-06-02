
#include "CompundResourceMeshComponent.h"
#include "IMeshEditor.h"
#include "DrawDebugHelpers.h"
#include "BuildingSDKComponent.h"
#include "BuildingGroupMeshComponent.h"

UCompundResourceMeshComponent::UCompundResourceMeshComponent(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
	,bDebug(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

ICompoundModel *UCompundResourceMeshComponent::GetCompoundEditor()
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK)
	{
		return SDK->GetCompoundEditor();
	}
	return nullptr;
}

kXform UCompundResourceMeshComponent::ToXform(const FTransform &Transform)
{
	kXform xform;
	FVector  Location = Transform.GetLocation();
	FVector  Scale = Transform.GetScale3D();
	FRotator Rotation = Transform.Rotator();

	xform.Location = FORCE_TYPE(kVector3D, Location);
	xform.Scale = FORCE_TYPE(kVector3D, Scale);
	xform.Rotation = FORCE_TYPE(kRotation, Rotation);
	
	return xform;
}

void UCompundResourceMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (bDebug && CompoundEditor && Resource.IsReady())
	{
		kArray<ElementID> Elements = CompoundEditor->GetElements(Resource.GetID());
		for (int i = 0; i < Elements.size(); ++i)
		{
			ElementID ID = Elements[i];
			kArray<PinID> Pins = CompoundEditor->GetPins(Resource.GetID(), ID);
			for (size_t iPin = 0; iPin < Pins.size(); ++iPin)
			{
				float Ratio = 0;
				kVector3D	Location;
				kVector3D	UniformLocation;
				if (CompoundEditor->GetPinInfo(Resource.GetID(), Pins[iPin], UniformLocation, Location))
				{
					DrawDebugBox(GetWorld(), FORCE_TYPE(FVector, Location), FVector(2.0f), FQuat::Identity, FColor::Red, false);
					FString PinDesc = FString::Printf(TEXT("[%.1f,%.1f,%.1f]"), UniformLocation.x, UniformLocation.y, UniformLocation.z);
					DrawDebugString(GetWorld(), FORCE_TYPE(FVector, Location), PinDesc, NULL, FColor::White, 0);
				}
			}
		}
	}
}

int32 UCompundResourceMeshComponent::AddElement(const FString &ResID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		return CompoundEditor->AddElement(Resource.GetID(), AnsiResID);
	}
	return INVALID_ELEMENT;
}

bool UCompundResourceMeshComponent::DeleteElement(int32 ElementID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		return CompoundEditor->DeleteElement(Resource.GetID(), ElementID);
	}
	return true;
}

void UCompundResourceMeshComponent::SetElementModel(int32 ElementID, const FString &ResID, bool bKeepPin)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		std::string AnsiResID = TCHAR_TO_ANSI(*ResID);
		EKeepRuleType Rule = bKeepPin ? EKeepPin : EKeepLocation;
		CompoundEditor->SetElement(Resource.GetID(), ElementID, AnsiResID.c_str(), Rule);
	}
}

void UCompundResourceMeshComponent::SetElementSurfaceByMeshName(int32 ElementID, const FString &MeshName, const FString &ResID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		std::string AnsiResID = TCHAR_TO_ANSI(*ResID);
		std::string AnsiMeshName = TCHAR_TO_ANSI(*MeshName);
		CompoundEditor->SetElementSurfaceByMeshName(Resource.GetID(), ElementID, AnsiMeshName.c_str(), AnsiResID.c_str());
	}
}

void UCompundResourceMeshComponent::SetElementVisibleByMeshName(int32 ElementID, const FString &MeshName, bool IsVisible)
{
	if (Resource.IsValid())
	{
		INode *pNode = Resource->GetNode();
		IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
		INode *pElemNode = pGroup->FindChild(ElementID);
		if (pElemNode)
		{
			pGroup = pElemNode->CastToGroup();
			if (MeshName.Len() > 0)
			{
				std::string mesh = TCHAR_TO_ANSI(*MeshName);
				kArray<INode *> nodes = pGroup->GetChildByMeshName(mesh.c_str());
				for (int i = 0; i < nodes.size(); ++i)
				{
					INode *pChildNode = nodes[i];
					pChildNode->SetVisible(IsVisible);
				}
			}
			else
			{
				pGroup->SetVisible(IsVisible);
			}
		}
	}
}

void UCompundResourceMeshComponent::SetElementSurfaceByMaterialName(int32 ElementID, const FString &MaterialName, const FString &ResID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		std::string AnsiResID = TCHAR_TO_ANSI(*ResID);
		std::string AnsiMaterialName = TCHAR_TO_ANSI(*MaterialName);
		CompoundEditor->SetElementSurfaceByMaterialName(Resource.GetID(), ElementID, AnsiMaterialName.c_str(), AnsiResID.c_str());
	}
}

FVector UCompundResourceMeshComponent::GetElementScale(int32 ElementID)
{
	FVector Scale3D(1.0f);
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D Scale;
		CompoundEditor->GetElementScale(Resource.GetID(), ElementID, Scale);
		Scale3D = FORCE_TYPE(FVector, Scale);
	}
	return Scale3D;
}

void UCompundResourceMeshComponent::GetElementLocationAndRotation(int32 ElementID, FVector &Location, FRotator &Rotation)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D Loc;
		kRotation Rot;
		CompoundEditor->GetElementLocationAndRotation(Resource.GetID(), ElementID, Loc, Rot);
		Location = FORCE_TYPE(FVector, Loc);
		Rotation = FORCE_TYPE(FRotator, Rot);
	}
}

void UCompundResourceMeshComponent::SetElementTransform(int32 ElementID, FRotator Rotation, FVector Location, FVector Scale)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D kLocation = FORCE_TYPE(kVector3D, Location);
		kRotation kRot = FORCE_TYPE(kRotation, Rotation);
		kVector3D kScale = FORCE_TYPE(kVector3D, Scale);

		CompoundEditor->SetElementTransform(Resource.GetID(), ElementID, kLocation, kRot, kScale);
	}
}

bool UCompundResourceMeshComponent::IsConnect(int32 ElementID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		return CompoundEditor->IsConnected(Resource.GetID(), ElementID);
	}
	return false;
}

void UCompundResourceMeshComponent::UnConnect(int32 ElementID, bool bKeepTransform)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		CompoundEditor->UnConnect(Resource.GetID(), ElementID, bKeepTransform);
	}
}

FVector UCompundResourceMeshComponent::ConvertToNormalizeLocation(int32 ElementID, FVector WorldLocation, bool bInnerCapacity)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D NormalizeLocation;		
		UBuildingGroupMeshComponent *Comp = FindComponentByElementID(ElementID);
		if (Comp != nullptr)
		{
			FTransform LocalToWorldTM = GetComponentTransform();
			FVector LocalLocation = LocalToWorldTM.Inverse().TransformPosition(WorldLocation);
			if (CompoundEditor->ConvertToNormalizeLocation(Resource.GetID(), ElementID, FORCE_TYPE(kVector3D, LocalLocation), NormalizeLocation, bInnerCapacity))
			{
				return FORCE_TYPE(FVector, NormalizeLocation);
			}
		}
	}

	return FVector::ZeroVector;
}

void UCompundResourceMeshComponent::GetMeshcCenter(int32 ElementID, FVector &MeshCenter, const FString &MeshName)
{
	INode *pNode = Resource.IsValid() ? Resource->GetNode() : nullptr;
	IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
	UBuildingGroupMeshComponent *GroupMeshComp = FindComponentByElementID(ElementID);
	if (pGroup && GroupMeshComp)
	{
		INode *pChildNode = pGroup->FindChild(ElementID);
		IGroup *pElemGroup = pChildNode ? pChildNode->CastToGroup() : nullptr;
		if (pElemGroup)
		{
			std::string AnsiName = TCHAR_TO_ANSI(*MeshName);
			kArray<INode *> nodes = pElemGroup->GetChildByMeshName(AnsiName.c_str());
			for (int32 i = 0; i < nodes.size(); ++i)
			{
				INode *pNextChildNode = nodes[i];
				if (pNextChildNode)
				{
					IMeshObject *meshObj = pNextChildNode->GetMesh();
					if (meshObj)
					{
						kBox3D MeshBound = meshObj->GetBounds();
						kVector3D Ext = MeshBound.GetCenter();

						MeshCenter.X = Ext.X;
						MeshCenter.Y = Ext.Y;
						MeshCenter.Z = Ext.Z;


					}
				}
			}
		}
	}
}

FVector UCompundResourceMeshComponent::ConvertToWorldLocation(int32 ElementID, FVector NormalLocation, bool bInnerCapacity)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D LocalLocation;
		UBuildingGroupMeshComponent *Comp = FindComponentByElementID(ElementID);
		if (Comp!=nullptr)
		{
			if (CompoundEditor->ConvertToLocalLocation(Resource.GetID(), ElementID, FORCE_TYPE(kVector3D, NormalLocation), LocalLocation, bInnerCapacity))
			{
				FTransform WorldTM = GetComponentTransform();
				FVector WorldLocation = WorldTM.TransformPosition(FORCE_TYPE(FVector, LocalLocation));
				return WorldLocation;
			}
		}
	}
	return FVector::ZeroVector;
}

int32 UCompundResourceMeshComponent::CreateInplacePinAtParent(int32 ParentElementID, bool bInnerCapacityParent, int32 SubElementID, int32 SubPinID)
{
	int32 ID = INVALID_PIN;
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kVector3D LocalLoction, UniLoc, ParentNormalizeLocation;
		if (CompoundEditor->GetPinInfo(Resource.GetID(), SubPinID, UniLoc, LocalLoction))
		{
			if (CompoundEditor->ConvertToNormalizeLocation(Resource.GetID(), ParentElementID, LocalLoction, ParentNormalizeLocation, bInnerCapacityParent))
			{
				ID = CompoundEditor->CreatePin(Resource.GetID(), ParentElementID, ParentNormalizeLocation, bInnerCapacityParent);
			}
		}
	}
	return ID;
}

void UCompundResourceMeshComponent::OnRegisterComponent(IObject *pObj)
{
	INode *pNode = pObj->GetNode();
	IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
	if (pGroup)
	{
		for (int i = 0; i < pGroup->GetChildCount(); ++i)
		{
			INode *pChildNode = pGroup->GetChild(i);
			IGroup *pElementGroup = pChildNode->CastToGroup();
			if (pElementGroup && pElementGroup->IsVisible())
			{
				AddComponentToWorld(this, pElementGroup);
			}
		}
	}
}

int32 UCompundResourceMeshComponent::GetElement(UPrimitiveComponent *Component)
{
	int32 ElemID = INVALID_ELEMENT;
	UBuildingGroupMeshComponent *MeshComp = Cast<UBuildingGroupMeshComponent>(Component);
	IGroup *pGroup = MeshComp? MeshComp->GetGroup() : nullptr;
	if (pGroup)
	{
		ElemID = pGroup->GetID();
	}
	return INVALID_ELEMENT;
}

bool UCompundResourceMeshComponent::GetElementInfo(int32 ElementID, int32 &ObjID, FString &ResID, FBox &InnerCapacityBox, FBox &OuterCapacityBox, FBox &MeshBounds)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kBox3D InnerBox, OutterBox;
		const char *AnsiResID = nullptr;
		if (CompoundEditor->GetElementInfo(Resource.GetID(), ElementID, ObjID, AnsiResID, InnerBox, OutterBox))
		{
			ResID = ANSI_TO_TCHAR(AnsiResID);

			FTransform WorldTM = GetComponentTransform();
			FVector MinEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, InnerBox.MinEdge));
			FVector MaxEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, InnerBox.MaxEdge));
			InnerCapacityBox += MinEdge;
			InnerCapacityBox += MaxEdge;
		
			MinEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, OutterBox.MinEdge));
			MaxEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, OutterBox.MaxEdge));
			OuterCapacityBox += MinEdge;
			OuterCapacityBox += MaxEdge;

			INode *pNode = Resource->GetNode();
			IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
			INode *pElemNode = pGroup->FindChild(ElementID);
			if (pElemNode)
			{
				kBox3D NodBounds = pElemNode->GetBoundingBox();
				if (!NodBounds.IsEmpty())
				{
					MinEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, NodBounds.MinEdge));
					MaxEdge = WorldTM.TransformPosition(FORCE_TYPE(FVector, NodBounds.MaxEdge));
					MeshBounds += MinEdge;
					MeshBounds += MaxEdge;
				}
			}

			return true;
		}
	}
	return false;
}

void UCompundResourceMeshComponent::SetPinNormalizeLocation(int32 PinID, FVector NormalizeLocation)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		CompoundEditor->SetPinLocation(Resource.GetID(), PinID, FORCE_TYPE(kVector3D, NormalizeLocation));
	}
}

void UCompundResourceMeshComponent::GetPinCount(int32 ElementID, TArray<int32> &ElmemtPins)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kArray<int> Pins = CompoundEditor->GetPins(Resource.GetID(), ElementID);
		ElmemtPins.SetNum(Pins.size());
		FMemory::Memcpy(ElmemtPins.GetData(), Pins.data(), Pins.bytes());
	}
}

bool UCompundResourceMeshComponent::GetPinInfo(int32 PinID, FVector &WorldLocation, FVector &NormalizeLocation, FVector& LocalLocation)
{
	kVector3D Loc, UniLoc;
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor && CompoundEditor->GetPinInfo(Resource.GetID(), PinID, UniLoc, Loc))
	{
		FTransform WorldTM = GetComponentTransform();
		FVector LocLocation = FORCE_TYPE(FVector, Loc);
		WorldLocation = WorldTM.TransformPosition(LocLocation);

		NormalizeLocation = FORCE_TYPE(FVector, UniLoc);
		LocalLocation = LocLocation;

		return true;
	}
	return false;
}

bool UCompundResourceMeshComponent::GetConnectPins(int32 ElementID0, int32 ElementID1, TArray<int32> &Pins0, TArray<int32> &Pins1)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		kArray<int> kPins0, kPins1;
		if (CompoundEditor->GetConnectPins(Resource.GetID(), ElementID0, ElementID1, kPins0, kPins1))
		{
			Pins0.SetNum(kPins0.size());
			Pins1.SetNum(kPins1.size());
			FMemory::Memcpy(Pins0.GetData(), kPins0.data(), kPins0.bytes());
			FMemory::Memcpy(Pins1.GetData(), kPins1.data(), kPins1.bytes());
			return true;
		}
	}
	return false;
}

int32 UCompundResourceMeshComponent::CreatePin(int32 ElementID, FVector UniformLocation, bool bInnerCapacity)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		return CompoundEditor->CreatePin(Resource.GetID(), ElementID, FORCE_TYPE(kVector3D, UniformLocation), bInnerCapacity);
	}
	return INVALID_PIN;
}

bool UCompundResourceMeshComponent::DeletePin(int32 PinID)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		return CompoundEditor->DeletePin(Resource.GetID(), PinID);
	}
	return false;
}

bool UCompundResourceMeshComponent::IsConnectValid(const TArray<FVector> &SubLoctions, const TArray<FVector> &FixedLoctions)
{
	if (SubLoctions.Num() ==0 || SubLoctions.Num() != FixedLoctions.Num())
	{
		return false;
	}

	if (FixedLoctions.Num() > 1)
	{
		for (size_t i = 1; i < FixedLoctions.Num(); ++i)
		{
			FVector Dir0 = (FixedLoctions[i] - FixedLoctions[i - 1]).GetUnsafeNormal();
			FVector Dir1 = (SubLoctions[i] - SubLoctions[i - 1]).GetUnsafeNormal();
			if (FVector::DotProduct(Dir0, Dir1) < 0.999f)
			{
				return false;
			}
		}
	}

	return true;
}

bool UCompundResourceMeshComponent::Connect(const FCompoundElementInterface &FixedInterface, const FCompoundElementInterface &SubInterface)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		FElementInterface Element0;
		Element0.ElemID = FixedInterface.ElementID;
		Element0.Pins = kArray<int>((int *)FixedInterface.Pins.GetData(), FixedInterface.Pins.Num());

		FElementInterface Element1;
		Element1.ElemID = SubInterface.ElementID;
		Element1.Pins = kArray<int>((int *)SubInterface.Pins.GetData(), SubInterface.Pins.Num());

		return CompoundEditor->Connect(Resource.GetID(), Element0, Element1);
	}
	return false;
}

void UCompundResourceMeshComponent::GetElementMaterials(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, int32 iSection /*= -1*/)
{
	UBuildingGroupMeshComponent *GroupMeshComp = FindComponentByElementID(ElementID);
	if (GroupMeshComp)
	{
		if (iSection >= 0)
		{
			INode *pNode = Resource.IsValid() ? Resource->GetNode() : nullptr;
			IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
			if (pGroup)
			{
				INode *pChildNode = pGroup->FindChild(ElementID);
				IGroup *pElemGroup = pChildNode ? pChildNode->CastToGroup() : nullptr;
				INode *pChild = pElemGroup? pElemGroup->GetChild(0) :nullptr;
				IMeshObject *meshObj = pChild ? pChild->GetMesh() : nullptr;
				if (meshObj)
				{
					int32 ComponentSectionIndex = GroupMeshComp->GetComponentSectionIndex(meshObj, iSection);
					UMaterialInstanceDynamic *DynMtr = Cast<UMaterialInstanceDynamic>(GroupMeshComp->GetMaterial(ComponentSectionIndex));
					if (DynMtr)
					{
						DynamicMaterials.Add(DynMtr);
					}
				}
			}
		}
		else
		{
			int32 NumMaterial = GroupMeshComp->GetNumOverrideMaterials();
			for (int32 i = 0; i < NumMaterial; ++i)
			{
				UMaterialInstanceDynamic *DynMtr = Cast<UMaterialInstanceDynamic>(GroupMeshComp->GetMaterial(i));
				if (DynMtr)
				{
					DynamicMaterials.AddUnique(DynMtr);
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::GetElementMaterialByMeshName(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, const FString &MeshName)
{
	INode *pNode = Resource.IsValid() ? Resource->GetNode() : nullptr;
	IGroup *pGroup = pNode? pNode->CastToGroup() :nullptr;
	UBuildingGroupMeshComponent *GroupMeshComp = FindComponentByElementID(ElementID);
	if (pGroup && GroupMeshComp)
	{
		INode *pChildNode = pGroup->FindChild(ElementID);
		IGroup *pElemGroup = pChildNode ? pChildNode->CastToGroup() : nullptr;
		if (pElemGroup)
		{
			std::string AnsiName = TCHAR_TO_ANSI(*MeshName);
			kArray<INode *> nodes = pElemGroup->GetChildByMeshName(AnsiName.c_str());
			for (int32 i = 0; i < nodes.size(); ++i)
			{
				INode *pNextChildNode = nodes[i];
				if (pNextChildNode)
				{
					IMeshObject *meshObj = pNextChildNode->GetMesh();
					if (meshObj)
					{
						TArray<int32> Sections;
						GroupMeshComp->GetComponentSections(meshObj, Sections);
						for (int32 iSection = 0; iSection < Sections.Num(); ++iSection)
						{
							UMaterialInstanceDynamic *DynaMtrl = Cast<UMaterialInstanceDynamic>(GroupMeshComp->GetMaterial(Sections[iSection]));
							if (DynaMtrl)
							{
								DynamicMaterials.AddUnique(DynaMtrl);
							}
						}
					}
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::GetElementMaterialBySourceMaterialName(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, const FString &MaterialName)
{
	INode *pNode = Resource.IsValid()? Resource->GetNode() : nullptr;
	IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;

	UBuildingGroupMeshComponent *GroupMeshComp = FindComponentByElementID(ElementID);
	if (pGroup && GroupMeshComp)
	{
		INode *pChildNode = pGroup->FindChild(ElementID);
		IGroup *pElemGroup = pChildNode ? pChildNode->CastToGroup() : nullptr;
		if (pElemGroup)
		{
			kArray<INode *> Nodes;
			kArray<int>		Sections;
			std::string AnsiName = TCHAR_TO_ANSI(*MaterialName);
			if (pElemGroup->GetChildByMaterialName(AnsiName.c_str(), Nodes, Sections))
			{
				if (!Nodes.empty())
				{
					IMeshObject *meshObj = Nodes[0]->GetMesh();
					if (meshObj!=nullptr)
					{
						for (int32 i = 0; i < Sections.size(); ++i)
						{
							int32 SectionIndex = Sections[i];
							int32 ComponentSectionIndex = GroupMeshComp->GetComponentSectionIndex(meshObj, SectionIndex);
							if (ComponentSectionIndex != INDEX_NONE)
							{
								UMaterialInstanceDynamic *DynaMtrl = Cast<UMaterialInstanceDynamic>(GroupMeshComp->GetMaterial(ComponentSectionIndex));
								if (DynaMtrl)
								{
									DynamicMaterials.AddUnique(DynaMtrl);
								}
							}
						}
					}
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::GetElementSize(int32 ElementID, int32 &Width, int32 &Height, int32 &Depth, bool bComponentInSpace)
{
	ICompoundModel *CompoundEditor = GetCompoundEditor();
	if (CompoundEditor)
	{
		if (bComponentInSpace)
		{
			kBox3D InnerBox, OutterBox;
			ObjectID ObjID = INVALID_OBJID;
			const char *AnsiResID = nullptr;
			if (CompoundEditor->GetElementInfo(Resource.GetID(), ElementID, ObjID, AnsiResID, InnerBox, OutterBox))
			{
				kVector3D Ext = OutterBox.GetExtent();
				Width = Ext.X;
				Depth = Ext.Y;
				Height = Ext.Z;
			}
		}
		else
		{
			CompoundEditor->GetElementSize(Resource.GetID(), ElementID, Width, Height, Depth);
		}
	}
}

void UCompundResourceMeshComponent::GetMeshSize(int32 ElementID, FVector &MeshSize, const FString &MeshName)
{
	INode *pNode = Resource.IsValid() ? Resource->GetNode() : nullptr;
	IGroup *pGroup = pNode ? pNode->CastToGroup() : nullptr;
	UBuildingGroupMeshComponent *GroupMeshComp = FindComponentByElementID(ElementID);
	if (pGroup && GroupMeshComp)
	{
		INode *pChildNode = pGroup->FindChild(ElementID);
		IGroup *pElemGroup = pChildNode ? pChildNode->CastToGroup() : nullptr;
		if (pElemGroup)
		{
			std::string AnsiName = TCHAR_TO_ANSI(*MeshName);
			kArray<INode *> nodes = pElemGroup->GetChildByMeshName(AnsiName.c_str());
			for (int32 i = 0; i < nodes.size(); ++i)
			{
				INode *pNextChildNode = nodes[i];
				if (pNextChildNode)
				{
					IMeshObject *meshObj = pNextChildNode->GetMesh();
					if (meshObj)
					{
						kBox3D MeshBound = meshObj->GetBounds();
						kVector3D Ext = MeshBound.GetExtent();
						MeshSize.X = Ext.X;
						MeshSize.Y = Ext.Y;
						MeshSize.Z = Ext.Z;

					}
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::GetElementMeshNames(int32 ElementID, TArray<FString> &MeshNames)
{
	if (Resource.IsValid())
	{
		INode *pNode = Resource->GetNode();
		IGroup *pResourceGroup = pNode? pNode->CastToGroup() : nullptr;
		if (pResourceGroup)
		{
			INode *pElemNode = pResourceGroup->FindChild(ElementID);
			IGroup *pElemGroup = pElemNode? pElemNode->CastToGroup() : nullptr;
			if (pElemGroup)
			{
				int nChild = pElemGroup->GetChildCount();
				for (int32 i = 0; i < nChild; ++i)
				{
					INode *pChildNode = pElemGroup->GetChild(i);
					if (pChildNode)
					{
						IMeshObject *meshObj = pChildNode->GetMesh();
						if (meshObj)
						{
							const char *meshName = meshObj->GetName();
							if (meshName)
							{
								FString Name = ANSI_TO_TCHAR(meshName);
								MeshNames.AddUnique(Name);
							}
						}
					}
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::GetSurfaces(int32 ElemID, TArray<ISurfaceObject *> &Surfaces, const FString &MeshName, int32 iSection)
{
	if (Resource.IsValid())
	{
		IObject *SurfaceOwner = nullptr;
		IGroup *pGroup = Resource->GetNode()->CastToGroup();
		IGroup *pElemGroup = pGroup->FindChild(ElemID)->CastToGroup();
		if (pElemGroup)
		{
			std::string AnsiName = TCHAR_TO_ANSI(*MeshName);
			kArray<INode *> Nodes = pElemGroup->GetChildByMeshName(AnsiName.c_str());
			for (int i = 0; i < Nodes.size(); ++i)
			{
				INode *pNode = Nodes[i];
				if (iSection >= 0)
				{
					ISurfaceObject *pSurf = pNode->GetSurface(iSection);
					if (pSurf)
					{
						Surfaces.AddUnique(pSurf);
					}
				}
				else
				{
					IMeshObject *meshObj = pNode->GetMesh();
					int32 nSection = meshObj ? meshObj->GetSectionCount() : 0;
					for (int32 index = 0; index < nSection; ++index)
					{
						ISurfaceObject *pSurf = pNode->GetSurface(index);
						if (pSurf)
						{
							Surfaces.AddUnique(pSurf);
						}
					}
				}
			}
		}
	}
}

void UCompundResourceMeshComponent::SetSurfaceScalarValue(int32 ElemID, const FString &MeshName, const FString &ParamName, float ScalarVaule, int32 iSection)
{
	TArray<ISurfaceObject *> Surfaces;
	GetSurfaces(ElemID, Surfaces, MeshName, iSection);
	for (int32 i = 0; i < Surfaces.Num(); ++i)
	{
		ISurfaceObject *pSurf = Surfaces[i];
		if (pSurf)
		{
			pSurf->SetScalar(CharPtr(*ParamName), ScalarVaule);
		}
	}
}

void UCompundResourceMeshComponent::SetSurfaceValue(int32 ElemID, const FString &MeshName, const FString &ParamName, const FLinearColor &Value, int32 iSection)
{
	TArray<ISurfaceObject *> Surfaces;
	GetSurfaces(ElemID, Surfaces, MeshName, iSection);
	for (int32 i = 0; i < Surfaces.Num(); ++i)
	{
		ISurfaceObject *pSurf = Surfaces[i];
		if (pSurf)
		{
			pSurf->SetVector(CharPtr(*ParamName), FORCE_TYPE(kVector4D, Value));
		}
	}
}

