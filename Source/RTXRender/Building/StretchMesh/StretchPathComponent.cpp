

#include "StretchPathComponent.h"
#include "EditorGameInstance.h"
#include "ResourceMgr.h"

FStretchShapeSlot::FStretchShapeSlot()
	: bVisible(true)
	, TypeId(INDEX_NONE)
	, MeshObjId(INVALID_OBJID)
	, Component(nullptr)
{
}

UStretchPathComponent::UStretchPathComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, PathID(INVALID_OBJID)
{
}

UBuildingSystem *UStretchPathComponent::GetBuildingSystem()
{
	UBuildingSystem *BuildingSystem = nullptr;
	UCEditorGameInstance *MyGame = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (MyGame)
	{
		BuildingSystem = MyGame->GetBuildingSystemInstance();
	}
	return BuildingSystem;
}

ISuite *UStretchPathComponent::GetSuite()
{
	ISuite *pSuite = nullptr;
	UBuildingSystem *BuildingSystem = GetBuildingSystem();
	if (BuildingSystem)
	{
		pSuite = BuildingSystem->GetSuite();
	}
	return pSuite;
}

IStretchPath *UStretchPathComponent::GetStretchPathEditor()
{
	IStretchPath *PathEditor = nullptr;

	UCEditorGameInstance *MyGame = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (MyGame)
	{
		IBuildingSDK *SDK = MyGame->GetBuildingSystemInstance()->GetBuildingSDK();
		if (SDK)
		{
			PathEditor = SDK->GetStretchPathEditor();
		}
	}
	return PathEditor;
}

IObject *UStretchPathComponent::GetObject(int32 ObjID)
{
	if (ObjID != INVALID_OBJID)
	{
		ISuite *Suite = GetSuite();
		if (Suite)
		{
			return Suite->GetObject(ObjID);
		}
	}
	return nullptr;
}

void UStretchPathComponent::SetSize(int32 TypeID, const FVector2D &Size, int32 PointIndex/*= INDEX_NONE*/)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		PathEditor->SetSize(Suite->GetObjectFactory(), PathID, TypeID, kPoint(Size.X, Size.Y), PointIndex);
	}
}

void UStretchPathComponent::SetStretchMeshVisible(int32 TypeID, bool IsVisible, int32 PointIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		PathEditor->SetVisible(Suite->GetObjectFactory(), PathID, TypeID, IsVisible, PointIndex);
	}
}

void UStretchPathComponent::SetShapeMX(int32 TypeID, const FString &ShapeMXID, int32 PointIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		std::string MXID = TCHAR_TO_ANSI(*ShapeMXID);;
		PathEditor->SetShape(Suite->GetObjectFactory(), PathID, TypeID, MXID.c_str(), PointIndex);
	}
}

void UStretchPathComponent::SetSurface(int32 TypeID, const FString &SurfaceUri, int32 PointIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		std::string MXID = TCHAR_TO_ANSI(*SurfaceUri);;
		PathEditor->SetSurface(Suite->GetObjectFactory(), PathID, TypeID, MXID.c_str(), PointIndex);
	}
}

FVector2D UStretchPathComponent::GetSize(int32 TypeID, int32 PointIndex)
{
	FVector2D Size = FVector2D::ZeroVector;
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		kPoint kSize = PathEditor->GetSize(Suite->GetObjectFactory(), PathID, TypeID, PointIndex);
		Size = FORCE_TYPE(FVector2D, kSize);
	}
	return Size;
}

bool UStretchPathComponent::GetStretchMeshVisible(int32 TypeID, int32 PointIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		return PathEditor->GetVisible(Suite->GetObjectFactory(), PathID, TypeID, PointIndex);
	}
	return false;
}

FString UStretchPathComponent::GetShapeMX(int32 TypeID, int32 PointIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		std::string mx = PathEditor->GetShape(Suite->GetObjectFactory(), PathID, TypeID, PointIndex);
		FString ResID = ANSI_TO_TCHAR(mx.c_str());
		return ResID;
	}
	return TEXT("");
}

int32 UStretchPathComponent::GetPathPointIndex(int32 StretchMeshID, int32 SubSectionIndex)
{
	int32 FoundIndex = INDEX_NONE;
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		FoundIndex = PathEditor->GetPointIndex(Suite->GetObjectFactory(), PathID, StretchMeshID, SubSectionIndex);
	}
	return FoundIndex;
}

bool UStretchPathComponent::GetSubMesh(int32 PointIndex, int32 TypeID, int32 &OutStretchMeshID, int32 &SubSectionIndex)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		return PathEditor->GetMeshSectionByPointIndex(Suite->GetObjectFactory(), PathID, PointIndex, TypeID, OutStretchMeshID, SubSectionIndex);
	}
	return false;
}

bool UStretchPathComponent::GetPath(int32 TypeID, TArray<FVector> &TypePaths)
{
	TypePaths.Empty();
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		kArray<kVector3D> kPath;
		if (PathEditor->GetPaths(Suite->GetObjectFactory(), PathID, TypeID, kPath))
		{
			FTransform WorldTransform = GetComponentTransform();
			TypePaths.SetNum(kPath.size());
			for (int32 i = 0; i < kPath.size(); ++i)
			{
				TypePaths[i] = WorldTransform.TransformPosition(FORCE_TYPE(FVector, kPath[i]));
			}
			return true;
		}
	}
	return false;
}

float UStretchPathComponent::GetCeilThickness()
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		return PathEditor->GetCeilThickness(Suite->GetObjectFactory(), PathID);
	}
	return 0;
}

void  UStretchPathComponent::SetCeilThickness(float Thickness)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		return PathEditor->SetCeilThickness(Suite->GetObjectFactory(), PathID, Thickness);
	}
}

void  UStretchPathComponent::SetCeilShape(const FString &ResID)
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		std::string mx = TCHAR_TO_ANSI(*ResID);
		return PathEditor->SetCeilShape(Suite->GetObjectFactory(), PathID, mx.c_str());
	}
}

FString UStretchPathComponent::GetCeilShape()
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (Suite && PathEditor)
	{
		FString  ResID = ANSI_TO_TCHAR(PathEditor->GetCeilShape(Suite->GetObjectFactory(), PathID));
		return ResID;
	}
	return TEXT("");
}

int32 UStretchPathComponent::GetTypeID(int32 StretchMeshID)
{
	int32 TypeID = -1;
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		IObject *pObj = Suite->GetObject(StretchMeshID);
		if (pObj)
		{
			IValue &Value = pObj->GetPropertyValue("TypeID");
			if (!Value.IsNil())
			{
				TypeID = Value.IntValue();
			}
		}
	}
	return TypeID;
}

void UStretchPathComponent::DeletePath()
{
	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();
	if (PathID != INVALID_OBJID && PathEditor && Suite)
	{
		PathEditor->DeletePath(Suite->GetObjectFactory(), PathID);
		PathID = INVALID_OBJID;
	}
}

void UStretchPathComponent::Update(const TArray<FStretchShapeSlot> &Slots, const TArray<FVector> &Path)
{
	if (PathID != INVALID_OBJID)
	{
		DeletePath();
	}

	ISuite *Suite = GetSuite();
	IStretchPath *PathEditor = GetStretchPathEditor();

	kArray<kVector3D> Paths((kVector3D*)Path.GetData(), Path.Num());
	std::vector<IStretchPath::FSlot> kSlots(Slots.Num());
	std::vector<std::string> MXFiles;

	for (int i = 0; i < Slots.Num(); ++i)
	{
		IStretchPath::FSlot &kSlot = kSlots[i];
		MXFiles.push_back(TCHAR_TO_ANSI(*Slots[i].MX));
		kSlot.MX = MXFiles.back().c_str();
		kSlot.TypeID = Slots[i].TypeId;
	}

	if (PathID == INVALID_OBJID)
	{
		PathID = PathEditor->AddPath(Suite->GetObjectFactory(), 0, kSlots, Paths);
		IObject *pPathObj = Suite->GetObject(PathID);
		if (pPathObj)
		{
			pPathObj->SetProxy(this);
		}
	}
}

int32 UStretchPathComponent::GetSelectPathPointIndex(FVector Start, FVector Dir, UBuildingComponent *StretchMesh)
{
	if (StretchMesh)
	{
		UBuildingData *Data = StretchMesh->Data;
		int StretchMeshID = Data->GetID();
		IObject *pObj = GetSuite()->GetObject(StretchMeshID);
		if (pObj)
		{
			IMeshObject *pMeshObj = pObj->GetMeshObject(StretchMeshID);
			if (pMeshObj)
			{
				int SectionIndex = -1;
				FTransform WorldTM = GetComponentTransform();
				FTransform WorldToLocal = WorldTM.Inverse();
				FVector LocalStart = WorldToLocal.TransformPosition(Start);
				FVector LocalDir = WorldToLocal.TransformVector(Dir);
				pMeshObj->HitTest(FORCE_TYPE(kVector3D, LocalStart), FORCE_TYPE(kVector3D, LocalDir), &SectionIndex);
				return GetPathPointIndex(StretchMeshID, SectionIndex);
			}
		}
	}

	return -1;
}

bool UStretchPathComponent::GetLightPath(TArray<int32> TypeList, int PointIndex, TArray<FVector>& LightPath, float& LightPointWidth,float& LightStartWidth)
{
	TArray<FVector> TypePath;
	FVector2D TypeMainSize, TypeChildenSize;
	TypeChildenSize = FVector2D::ZeroVector;
	float CeilingThickness = 0.0f;
	if (TypeList.Contains(3))
	{
		GetPath(3, TypePath);
		if (TypePath.Num() >= (PointIndex + 2))
		{
			int32 PathStartIndex, PathEndIndex;
			PathStartIndex = PointIndex;
			if (TypePath.Num() > (PointIndex + 2))
				PathEndIndex = PointIndex + 1;
			else
				PathEndIndex = 0;

			TypeMainSize = GetSize(3, 0);

			if (TypeList.Contains(6))
			{
				TypeChildenSize = GetSize(6, 0);
			}
			if (TypeList.Contains(7))
			{
				TypeChildenSize += GetSize(7, 0);
			}

			FVector Seg = TypePath[PathEndIndex] - TypePath[PathStartIndex];
			FVector Dir = Seg.RotateAngleAxis(90, FVector(0, 0, 1));
			Dir.Normalize();

			//if (GetCeiling())
			//{
			//	CeilingThickness = GetCeilThickness();
			//}
			//if (TypeList.Contains(1))
			//{
			//	CeilingThickness += GetSize(1, 0).Y;
			//}

			FVector StartPoint = TypePath[PathStartIndex] + (Dir * TypeChildenSize.X);
			FVector EndPoint = TypePath[PathStartIndex] + (Dir * TypeMainSize.X);
			FVector MidPoint = (StartPoint + EndPoint) / 2.0f;
			LightPath.Add(MidPoint - FVector(0, 0, (TypeMainSize.Y + CeilingThickness)));

			StartPoint = TypePath[PathEndIndex] + (Dir * TypeChildenSize.X);
			EndPoint = TypePath[PathEndIndex] + (Dir * TypeMainSize.X);
			MidPoint = (StartPoint + EndPoint) / 2.0f;
			LightPath.Add(MidPoint - FVector(0, 0, (TypeMainSize.Y + CeilingThickness)));

			LightPointWidth = FVector::Distance(StartPoint, EndPoint);
			LightStartWidth = TypeChildenSize.X;
			return true;
		}
		return false;
	}
	return false;
}

UBuildingComponent *UStretchPathComponent::GetCeiling()
{
	UBuildingComponent *Comp = nullptr;
	for (TMap<int32, UBuildingComponent *>::TIterator It(Components); It; ++It)
	{
		int32 ID = It.Key();
		IObject *pObj = GetObject(ID);
		if (pObj && pObj->IsA(EStretchCeiling))
		{
			Comp = It.Value();
			break;
		}
	}
	return Comp;
}

void UStretchPathComponent::OnAddObject(IObject *RawObj)
{
	int32 iSegment = INDEX_NONE, iSlot = INDEX_NONE;
	EObjectType Type = RawObj->GetType();

	int32 ID = RawObj->GetID();
	UBuildingSystem *BuildingSys = GetBuildingSystem();
	if (Type == EStretchMesh || Type== EStretchCeiling && BuildingSys)
	{
		UBuildingData *Data = BuildingSys->GetData(ID);
		if (Data)
		{
			UBuildingComponent *Comp = NewObject<UBuildingComponent>(GetOwner());
			Comp->RegisterComponentWithWorld(GetWorld());
			Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			Comp->SetData(Data);
			Components.FindOrAdd(ID) = Comp;
			Comp->SetHiddenInGame(!Data->GetBool(TEXT("bVisible")));

			OnStretchPathUpdate.Broadcast();
		}
	}
}

void UStretchPathComponent::OnDeleteObject(IObject *RawObj)
{
	UBuildingComponent **ppComp = Components.Find(RawObj->GetID());
	if (ppComp)
	{
		UBuildingComponent *Comp = *ppComp;
		Comp->DestroyComponent();
		Components.Remove(RawObj->GetID());
	}
}

void UStretchPathComponent::OnUpdateObject(IObject *RawObj, unsigned int ChannelMask)
{
	UBuildingComponent **ppComp = Components.Find(RawObj->GetID());
	if (ppComp)
	{
		UBuildingComponent *Comp = *ppComp;
		UBuildingSystem *BuildingSys = GetBuildingSystem();
		UBuildingData *Data = BuildingSys->GetData(RawObj->GetID());
		if (Data && Comp)
		{
			Comp->SetData(Data);
			Comp->SetHiddenInGame(!Data->GetBool(TEXT("bVisible")));
			OnStretchPathUpdate.Broadcast();
		}
	}
}

void UStretchPathComponent::OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID Surface)
{
	UBuildingComponent **ppComp = Components.Find(RawObj->GetID());
	if (ppComp)
	{
		UBuildingComponent *Comp = *ppComp;
		if (Comp)
		{
			Comp->UpdateSurface(SectionIndex, Surface);
		}
	}
}

void UStretchPathComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	for (TMap<int32, UBuildingComponent *>::TIterator It(Components); It; ++It)
	{
		UBuildingComponent *Comp = It.Value();
		Comp->DestroyComponent(true);
	}

	PathID = INVALID_OBJID;
	Components.Empty();

	Super::DestroyComponent(bPromoteChildren);
}

void UStretchPathComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		IObject *pPathObj = Suite->GetObject(PathID);
		if (pPathObj)
		{
			pPathObj->SetProxy(nullptr);
		}
	}
	Super::EndPlay(EndPlayReason);
}


