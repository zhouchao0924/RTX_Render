

#include "CurtainComponent.h"
#include "ModelFile.h"
#include "ResourceMgr.h"
#include "ModelCommons.h"
#include "ModelPartComponent.h"
#include "ModelFileComponent.h"

UCurtainComponent::UCurtainComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, Length(10.0f)
{
}

void UCurtainComponent::SetCurtainInfo(const FString &InResRomebar1, const FString &InResCloth1, const FString &InResRomebar2, const FString &InResCloth2, float InLength, float InHeight, float InDistanceBetweenBar, bool Blind, EClothPosition InClothPosition)
{
	Cloth1 = InResCloth1;
	Romebar1 = InResRomebar1;
	Cloth2 = InResCloth2;
	Romebar2 = InResRomebar2;
	DistanceBetweenBar = InDistanceBetweenBar;
	Length = InLength;
	Height = InHeight;
	IsBlind = Blind;
	ClothPosition = InClothPosition;
	CreateModels();
}

void UCurtainComponent::SetLength(float InLength)
{
	Length = InLength;
	UpdateLayout();
}

UModelFile *UCurtainComponent::GetRombarModel(int32 index)
{
	UModelFile *RombarModel = nullptr;
	UResourceMgr *ResMgr = UResourceMgr::Instance(this);
	if (ResMgr)
	{
		FString ResID = index == 0 ? Romebar1 : Romebar2;
		RombarModel = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (RombarModel)
		{
			RombarModel->ForceLoad();
		}
	}
	return RombarModel;
}

UModelFile *UCurtainComponent::GetClothModel(int32 index)
{
	UModelFile *ClothModel = nullptr;
	UResourceMgr *ResMgr = UResourceMgr::Instance(this);
	if (ResMgr)
	{
		FString &ResID = index == 0 ? Cloth1 : Cloth2;
		ClothModel = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (ClothModel)
		{
			ClothModel->ForceLoad();
		}
	}
	return ClothModel;
}

void UCurtainComponent::CreateModels()
{
	CreateCloths();
	CreateRomerbars();
	UpdateLayout();
}

void UCurtainComponent::CreateCloths()
{
	DestroyCloth();
	if (IsBlind)
	{
		AddBlindComponents(GetClothModel(0), LeftClothComponents1);
	}
	else
	{
		AddClothComponents(GetClothModel(0), LeftClothComponents1, RightClothComponents1);
		AddClothComponents(GetClothModel(1), LeftClothComponents2, RightClothComponents2);
	}
}

void UCurtainComponent::AddClothComponents(UModelFile *ClothModel, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents)
{
	if (ClothModel)
	{
		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();

		FVector Size = ClothModel->GetLocalBounds().GetSize();
		TArray<UModelPartComponent *> *ClothComponents[2] = { &LeftClothComponents, &RightClothComponents };
		for (int32 iCloth = 0; iCloth < 2; ++iCloth)
		{
			TArray<FModel*> &Models = ClothModel->GetSubModels();
			for (int i = 0; i < Models.Num(); ++i)
			{
				UModelPartComponent *Comp = NewObject<UModelPartComponent>(GetOwner());
				if (Comp)
				{
					Comp->RegisterComponentWithWorld(GetWorld());
					Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
					Comp->UpdateModel(TArray<FPlane>(), Forward, Size.Y, ClothModel, i);
					//Comp->SetLightChannels(false, true, false);
					ClothComponents[iCloth]->Add(Comp);
				}
			}
		}
	}
}

void UCurtainComponent::AddBlindComponents(UModelFile *BlindModel, TArray<UModelPartComponent *> &BlindComponent)
{
	if (BlindModel)
	{
		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();

		FVector Size = BlindModel->GetLocalBounds().GetSize();
		BlindLength = Size.X;
		TArray<UModelPartComponent *> *BlindComponents[1] = { &BlindComponent };
		TArray<FModel*> &Models = BlindModel->GetSubModels();
		for (int i = 0; i < Models.Num(); ++i)
		{
			UModelPartComponent *Comp = NewObject<UModelPartComponent>(GetOwner());
			if (Comp)
			{
				Comp->RegisterComponentWithWorld(GetWorld());
				Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
				Comp->UpdateModel(TArray<FPlane>(), Forward, Size.Y, BlindModel, i);
				//Comp->SetLightChannels(false, true, false);
				BlindComponents[0]->Add(Comp);
			}
		}
	}
}

void UCurtainComponent::CreateRomerbars()
{
	DestroyRomerbar();
	AddRomerbarComponents(GetRombarModel(0), RomerBarComponents1);
	AddRomerbarComponents(GetRombarModel(1), RomerBarComponents2);
}

void UCurtainComponent::AddRomerbarComponents(UModelFile *RomebarModel, TArray<UModelPartComponent *> &RomerBarComponents)
{
	RomerBarComponents.SetNum(uint8(ECurtainPart::ERomerbarMax));
	FTransform WorldTransform = GetComponentTransform();
	FVector Forward = WorldTransform.Rotator().Vector();

	if (RomebarModel)
	{
		int32		modelRod = INDEX_NONE;
		FVector		Size = RomebarModel->GetLocalBounds().GetSize();
		FTransform	RelativeTransform;
		RomerbarBodyLength = 0;
		RomerbarHeadLength = 0;

		TArray<FModel*> &Models = RomebarModel->GetSubModels();
		for (int32 i = 0; i < Models.Num(); ++i)
		{
			FModel *pModel = Models[i];
			if (pModel)
			{
				int iPart = 0;
				if (pModel->ModelName == TEXT("Curtain_Rod"))
				{
					modelRod = i;
					RomerbarBodyLength = pModel->Bounds.GetSize().X;
				}
				else if (pModel->ModelName == TEXT("Curtain_Rod_Head_L"))
				{
					RomerbarHeadLength += pModel->Bounds.GetSize().X;
					iPart = (int32)ECurtainPart::ERomebarL;
					RomerBarComponents[iPart] = AddBarPart(RomebarModel, i);
				}
				else if (pModel->ModelName == TEXT("Curtain_Rod_Head_R"))
				{
					RomerbarHeadLength += pModel->Bounds.GetSize().X;
					iPart = (int32)ECurtainPart::ERomebarR;
					RomerBarComponents[iPart] = AddBarPart(RomebarModel, i);
				}
			}
		}

		if (modelRod >= 0)
		{
			RomerBarComponents[int(ECurtainPart::ERomebarM)] = AddBarPart(RomebarModel, modelRod);
		}
	}

}

void UCurtainComponent::SetCloth(int32 index, const FString &InResCloth, bool UpdateModel)
{
	if ((index == 1 && !(Cloth1 == InResCloth)) || (index == 2 && !(Cloth2 == InResCloth)) || UpdateModel)
	{
		if (index == 1)
		{
			Cloth1 = InResCloth;
		}
		else
		{
			Cloth2 = InResCloth;
		}
		CreateCloths();
		UpdateLayout();
	}
}

void UCurtainComponent::SetRomerbar(int32 index, const FString &InResRomerbar, bool UpdateModel, EClothPosition InClothPostion)
{
	if (((index == 1) && !(Romebar1 == InResRomerbar)) || ((index == 2) && !(Romebar2 == InResRomerbar)) || UpdateModel)
	{
		if (index == 1)
		{
			Romebar1 = InResRomerbar;
		}
		else
		{
			Romebar2 = InResRomerbar;
		}
		ClothPosition = InClothPostion;
		CreateRomerbars();
		UpdateLayout();
	}
}

UModelPartComponent *UCurtainComponent::AddBarPart(UModelFile *ModelFile, int32 iModel)
{
	UModelPartComponent *Comp = nullptr;

	if (ModelFile)
	{
		FTransform WorldTransform = GetComponentTransform();
		FVector Forward = WorldTransform.Rotator().Vector();
		FVector Size = ModelFile->GetLocalBounds().GetSize();

		Comp = NewObject<UModelPartComponent>(GetOwner());
		if (Comp)
		{
			Comp->RegisterComponentWithWorld(GetWorld());
			Comp->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			Comp->UpdateModel(TArray<FPlane>(), Forward, Size.X, ModelFile, iModel);
			//Comp->SetLightChannels(false, true, false);
		}
	}

	return Comp;
}

void UCurtainComponent::DestroyCloth()
{
	DestroyComponents(LeftClothComponents1);
	DestroyComponents(RightClothComponents1);
	DestroyComponents(LeftClothComponents2);
	DestroyComponents(RightClothComponents2);
}

void UCurtainComponent::DestroyRomerbar()
{
	DestroyComponents(RomerBarComponents1);
	DestroyComponents(RomerBarComponents2);
}

void UCurtainComponent::DestroyComponents(TArray<UModelPartComponent *> &Components)
{
	for (int32 i = 0; i < Components.Num(); ++i)
	{
		UModelPartComponent *Comp = Components[i];
		if (Comp)
		{
			Comp->DestroyComponent(true);
		}
	}
	Components.Empty();
}

void UCurtainComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	DestroyCloth();
	DestroyRomerbar();
	Super::DestroyComponent(bPromoteChildren);
}

void UCurtainComponent::SetCurtainLength(float InLength)
{
	if (Length != InLength)
	{
		Length = InLength;
		UpdateLayout();
	}
}

void UCurtainComponent::SetCurtainHeight(float InHeight)
{
	if (Height != InHeight)
	{
		Height = InHeight;
		UpdateLayout();
	}
}

void UCurtainComponent::SetDistanceBetweenBar(float InDistance)
{
	if (DistanceBetweenBar != InDistance)
	{
		DistanceBetweenBar = InDistance;
		UpdateLayout();
	}
}
TArray<int32> UCurtainComponent::GetRomBarMXModelIndexArr(UModelFile *RomebarModel)
{
	TArray<int32>IntArray;
	IntArray.Init(0,3);
		TArray<FModel*> &Models = RomebarModel->GetSubModels();
		for (int32 i = 0; i < Models.Num(); ++i)
		{
			FModel *pModel = Models[i];
			if (pModel)
			{
				if (pModel->ModelName == TEXT("Curtain_Rod_Head_L")) {
					IntArray[0] = i;
				}
				if (pModel->ModelName == TEXT("Curtain_Rod_Head_R")) {
					IntArray[1] = i;
				}
				if (pModel->ModelName == TEXT("Curtain_Rod")) {
					IntArray[2] = i;
				}
			}
		}
	return IntArray;
}

bool UCurtainComponent::IsTwoBar()
{
	return Romebar2.Len() > 0;
}

void UCurtainComponent::UpdateLayout()
{
	if (GetClothModel(0) == NULL && GetClothModel(1) != NULL)
	{
		UpdateComponentsLayout(FVector::ZeroVector, GetRombarModel(0), GetClothModel(1), LeftClothComponents2, RightClothComponents2, RomerBarComponents1);
	}
	else
	{
		UpdateComponentsLayout(FVector::ZeroVector, GetRombarModel(0), GetClothModel(0), LeftClothComponents1, RightClothComponents1, RomerBarComponents1);
	}

	if (IsTwoBar())
	{
		FVector TwobarOffset = FVector(0, -DistanceBetweenBar, 0);
		UpdateComponentsLayout(TwobarOffset, GetRombarModel(1), GetClothModel(1), LeftClothComponents2, RightClothComponents2, RomerBarComponents2);
	}
}

void UCurtainComponent::UpdateComponentsLayout(const FVector &TwoBarOffset, UModelFile *RomebarModel, UModelFile *ClothModel, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents, TArray<UModelPartComponent *> &RomerBarComponents)
{
	FVector ClothOffset;
	TArray<float> RomerbarHeight;
	RomerbarHeight.Init(0, 4);
	float ClothOffset_X = 0;
	float RomerbarOffset_Z = 0;
	int32 RomerbarL = (int32)ECurtainPart::ERomebarL;
	int32 RomerbarR = (int32)ECurtainPart::ERomebarR;
	int32 RomerbarM = (int32)ECurtainPart::ERomebarM;
	int32 RomerbarMax = (int32)ECurtainPart::ERomerbarMax;

	if (RomebarModel)
	{
		FTransform RelativeTransform;

		if (RomerBarComponents.IsValidIndex(RomerbarL))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarL];
			if (Comp)
			{
				FModel *SubModel = Comp->GetModel();
				if (SubModel)
				{
					RomerbarHeight[RomerbarL] = RomerbarHeight[RomerbarMax] = SubModel->Bounds.GetSize().Z;
					ClothOffset_X = SubModel->Bounds.GetSize().X;
					RelativeTransform = UModelFileComponent::GetOffset(SubModel->Bounds, FVector(1.0f), ECenterAdjustType::BackCenter);
					FVector Offset = RelativeTransform.GetLocation();
					Offset = FVector(Offset.X + Length / 2.0f, 0, Height) + TwoBarOffset;
					RelativeTransform = FTransform(FRotator(0, 0, 0), Offset);
					Comp->SetRelativeTransform(RelativeTransform);
				}
			}
		}

		if (RomerBarComponents.IsValidIndex(RomerbarR))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarR];
			if (Comp)
			{
				FModel *SubModel = Comp->GetModel();
				if (SubModel)
				{
					ClothOffset_X = SubModel->Bounds.GetSize().X > ClothOffset_X ? SubModel->Bounds.GetSize().X : ClothOffset_X;
					RomerbarHeight[RomerbarMax] = SubModel->Bounds.GetSize().Z > RomerbarHeight[RomerbarMax] ? SubModel->Bounds.GetSize().Z : RomerbarHeight[RomerbarMax];
					RomerbarHeight[RomerbarR] = SubModel->Bounds.GetSize().Z;
					RelativeTransform = UModelFileComponent::GetOffset(SubModel->Bounds, FVector(1.0f), ECenterAdjustType::FrontCenter);
					FVector Offset = RelativeTransform.GetLocation();
					Offset = FVector(Offset.X - Length / 2.0f, 0, Height) + TwoBarOffset;
					RelativeTransform = FTransform(FRotator(0, 0, 0), Offset);
					Comp->SetRelativeTransform(RelativeTransform);
				}
			}
		}

		if (RomerBarComponents.IsValidIndex(RomerbarM))
		{
			UModelPartComponent *Comp = RomerBarComponents[RomerbarM];
			if (Comp)
			{
				FModel *SubModel = Comp->GetModel();
				if (SubModel)
				{
					RomerbarHeight[RomerbarMax] = SubModel->Bounds.GetSize().Z > RomerbarHeight[RomerbarMax] ? SubModel->Bounds.GetSize().Z : RomerbarHeight[RomerbarMax];
					RomerbarHeight[RomerbarM] = SubModel->Bounds.GetSize().Z;
					float DesireLength = Length;
					RelativeTransform = UModelFileComponent::GetOffset(RomebarModel, ECenterAdjustType::TopCenter);
					FVector Offset = RelativeTransform.GetLocation();
					Offset = FVector(Offset.X, 0, Height) + TwoBarOffset;
					FVector Scale3D = FVector(DesireLength / RomerbarBodyLength, 1.0f, 1.0f);
					RelativeTransform = FTransform(FRotator(0, 0, 0), Offset, Scale3D);
					Comp->SetRelativeTransform(RelativeTransform);
				}
			}
		}

		switch (ClothPosition)
		{
		case EClothPosition::EClothUp:
			RomerbarOffset_Z = -RomerbarHeight[RomerbarMax] / 2.0f;
			break;
		case EClothPosition::EClothMiddle:
			RomerbarOffset_Z = 0;
			break;
		case EClothPosition::EClothDown:
			RomerbarOffset_Z = RomerbarHeight[RomerbarM] / 2.0f;
			break;
		}

		for (int32 i = 0; i < RomerBarComponents.Num(); i++)
		{
			if (RomerBarComponents.IsValidIndex(i))
			{
				UModelPartComponent *Comp = RomerBarComponents[i];
				if (Comp)
				{
					Comp->AddRelativeLocation(FVector(0, 0, RomerbarOffset_Z));
				}
			}
		}
	}


	if (ClothModel)
	{
		float ScaleHeight = 1.0f;
		FVector Size = ClothModel->GetLocalBounds().GetSize();
		float ClothLength = Size.X;

		if (Height > 0 && Size.Z > 0)
		{
			ScaleHeight = Height / Size.Z;
		}
		else
		{
			Height = Size.Z;
		}

		FTransform ClothTransform[2];
		TArray<UModelPartComponent *> *ClothComponents[2] = { &LeftClothComponents, &RightClothComponents };

		//left
		ClothOffset = FVector(ClothOffset_X, 0, 0);
		FVector Offset = FVector(0, 0, 0);
		if (!IsBlind)
		{
			Offset = FVector(-Length / 2.0f + ClothLength / 2.0f, 0, 0) + TwoBarOffset + ClothOffset;
			ClothTransform[0].SetScale3D(FVector(1.0f, 1.0f, ScaleHeight));
		}
		else
		{
			ClothTransform[0].SetScale3D(FVector(Length / BlindLength, 1.0f, ScaleHeight));
		}
		ClothTransform[0].AddToTranslation(Offset);


		//right
		ClothOffset = FVector(-ClothOffset_X, 0, 0);
		Offset = FVector(Length / 2.0f - ClothLength / 2.0f, 0, 0) + TwoBarOffset + ClothOffset;
		ClothTransform[1].SetScale3D(FVector(-1.0f, 1.0f, ScaleHeight));
		ClothTransform[1].AddToTranslation(Offset);

		for (int32 iCloth = 0; iCloth < 2; ++iCloth)
		{
			TArray<UModelPartComponent *> &Components = *(ClothComponents[iCloth]);
			for (int32 iComp = 0; iComp < Components.Num(); ++iComp)
			{
				UModelPartComponent *Comp = Components[iComp];
				if (Comp)
				{
					Comp->SetRelativeTransform(ClothTransform[iCloth]);
				}
			}
		}
	}
}


