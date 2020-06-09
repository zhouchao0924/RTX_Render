// Copyright© 2017 ihomefnt All Rights Reserved.


#include "ProjectPaper.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EditorGameInstance.h"

UProjectPaper::UProjectPaper()
{
}

void UProjectPaper::InitGameInst(class UCEditorGameInstance *InGameInst)
{
	GameInst = InGameInst;
}

void UProjectPaper::LoadFromFile()
{
	FString ProjectConfigPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("test.json"));
	FString FileContent;
	FFileHelper::LoadFileToString(FileContent, *ProjectConfigPath);
	TSharedRef<TJsonReader<>> ProjectContentReader = TJsonReaderFactory<>::Create(FileContent);
	TSharedPtr<FJsonObject> HouseJsonObject;
	if (!FJsonSerializer::Deserialize(ProjectContentReader, HouseJsonObject))
	{
		return;
	}

	LoadPaperFromHouseJson(HouseJsonObject);
}

void UProjectPaper::LoadFromHouseJson(UVaRestJsonObject *HouseJson)
{
	if (HouseJson)
	{
		LoadPaperFromHouseJson(HouseJson->GetRootObject());
	}
}

void UProjectPaper::PaperDoOffset(const FVector2D &OldOrigin, const FVector2D &NewOrigin)
{
	FVector2D OffsetDir = NewOrigin - OldOrigin;

	for (FCornerData &CornerNode : CornerNodes.PooledDatas)
	{
		CornerNode.Position += OffsetDir;
	}

	for (FOpeningData &OpeningNode : OpeningNodes.PooledDatas)
	{
		OpeningNode.Position += OffsetDir;
	}
}

void UProjectPaper::InitHousePictureInfo()
{

}

void UProjectPaper::LoadPaperFromHouseJson(const TSharedPtr<FJsonObject> &HouseJsonObject)
{
	if (!HouseJsonObject.IsValid())
	{
		return;
	}
	LoadCornersFromJson(HouseJsonObject);

	LoadWallsFromJson(HouseJsonObject);

	LoadOpeningsFromJson(HouseJsonObject);

	LoadTextLabelsFromJson(HouseJsonObject);

	MakeRelation();
}

const TArray<FCornerData>& UProjectPaper::GetCorners()
{
	return CornerNodes.PooledDatas;
}

const TArray<FWallData>& UProjectPaper::GetWalls()
{
	return WallNodes.PooledDatas;
}

const TArray<FOpeningData>& UProjectPaper::GetOpenings()
{
	return OpeningNodes.PooledDatas;
}

FString UProjectPaper::CreateNewGuid()
{
	return FGuid::NewGuid().ToString().ToLower();
}

void UProjectPaper::ConvertWallData2SaveGame(FSavedDataNode &OutWallData, UCEditorGameInstance *InGameInst, const FWallData &InData)
{
	OutWallData = InGameInst->WallBuildSystem->GetDefaultWallSetting();
	OutWallData.bIsDelete = false;
	OutWallData.Type = EWallTypeCPP::Wall;
	OutWallData.Transform = FTransform::Identity;
	OutWallData.TagName = InData.ID;
	FWallNode& WallNode = OutWallData.WallNode;
	int32 CornerCnt = CornerNodes.PooledDatas.Num();
	if (InData.StartCornerIndex >= 0 && InData.StartCornerIndex < CornerCnt)
	{
		WallNode.StartPos = CornerNodes.PooledDatas[InData.StartCornerIndex].Position;
		WallNode.StartPnt = FVector(WallNode.StartPos, 0);
	}
	if (InData.EndCornerIndex >= 0 && InData.EndCornerIndex < CornerCnt)
	{
		WallNode.EndPos = CornerNodes.PooledDatas[InData.EndCornerIndex].Position;
		WallNode.EndPnt = FVector(WallNode.EndPos, 0);
	}
	
	WallNode.LeftThick = InData.LeftThick;
	WallNode.RightThick = InData.RightThick;
	WallNode.bLeftRuler = InData.bShowLeftRuler;
	WallNode.bRightRuler = InData.bShowRightRuler;
	WallNode.Name = InData.ID;
	WallNode.Height = InData.Height;
	WallNode.StartCorner = InData.StartCornerID;
	WallNode.EndCorner = InData.EndCornerID;
	WallNode.ZPos = 0.0f;
	WallNode.FrontUVAngle = WallNode.BackUVAngle = WallNode.SideUVAngle = 0.0f;
	WallNode.FrontUVPos = WallNode.BackUVPos = WallNode.SideUVPos = FVector::ZeroVector;
	WallNode.FrontUVScale = WallNode.BackUVScale = WallNode.SideUVScale = FVector2D(100.0f, 100.0f);
	WallNode.Color = FLinearColor(0.155f, 0.155f, 0.155f, 1.0f);
}

void UProjectPaper::LoadCornersFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject)
{
	CornerNodes.PooledDatas.Empty();
	const auto& CornerJsonList = HouseJsonObject->GetArrayField(TEXT("Corners"));
	for (const auto &CornerJson : CornerJsonList)
	{
		auto &CornerObject = CornerJson->AsObject();
		FCornerData CornerData;
		CornerData.ID = CornerObject->GetStringField(TEXT("ID"));
		CornerData.Position.X = CornerObject->GetNumberField(TEXT("X"));
		CornerData.Position.Y = CornerObject->GetNumberField(TEXT("Y"));
		CornerNodes.PooledDatas.Add(CornerData);
	}
}

void UProjectPaper::LoadWallsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject)
{
	UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;

	WallNodes.PooledDatas.Empty();
	const auto& WallJsonList = HouseJsonObject->GetArrayField(TEXT("Walls"));
	for (const auto &WallJson : WallJsonList)
	{
		auto &WallObject = WallJson->AsObject();
		FWallData WallData;
		WallData.ID = WallObject->GetStringField(TEXT("ID"));
		WallData.StartCornerID = WallObject->GetStringField(TEXT("StartCorner"));
		WallData.EndCornerID = WallObject->GetStringField(TEXT("EndCorner"));
		WallData.LeftThick = WallObject->GetNumberField(TEXT("LeftThick"));
		WallData.RightThick = WallObject->GetNumberField(TEXT("RightThick"));
		bool bShowLeftRuler = true;
		if (WallObject->TryGetBoolField(TEXT("ShowLeftRuler"), bShowLeftRuler))
		{
			WallData.bShowLeftRuler = bShowLeftRuler;
		}
		else
		{
			WallData.bShowLeftRuler = true;
		}
		bool bShowRightRuler = true;
		if (WallObject->TryGetBoolField(TEXT("ShowLeftRuler"), bShowRightRuler))
		{
			WallData.bShowRightRuler = bShowRightRuler;
		}
		else
		{
			WallData.bShowRightRuler = true;
		}
		
		WallData.WallType = (EWallUsage)(WallObject->GetIntegerField(TEXT("Usage")));
		WallData.LeftRoomID = WallObject->GetStringField(TEXT("LeftRoom"));
		WallData.RightRoomID = WallObject->GetStringField(TEXT("RightRoom"));
		double WallHeight = 0.0f;
		if (WallObject->TryGetNumberField(TEXT("Height"), WallHeight))
		{
			WallData.Height = (float)WallHeight;
		}
		else
		{
			WallData.Height = WallSystem->GetDefaultWallSetting().WallNode.Height;
		}
		
		WallNodes.PooledDatas.Add(WallData);
	}
}

void UProjectPaper::LoadOpeningsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject)
{
	OpeningNodes.PooledDatas.Empty();
	const auto& WallOpeningJsonList = HouseJsonObject->GetArrayField(TEXT("WallOpenings"));
	for (const auto &WallOpeningJson : WallOpeningJsonList)
	{
		auto &OpeningJsonObject = WallOpeningJson->AsObject();
		FOpeningData OpeningData;
		OpeningData.ID = OpeningJsonObject->GetStringField(TEXT("ID"));
		auto &LocationJsonObject = OpeningJsonObject->GetObjectField(TEXT("Location"));
		OpeningData.LeftThick = OpeningJsonObject->GetNumberField(TEXT("LeftThick"));
		OpeningData.RightThick = OpeningJsonObject->GetNumberField(TEXT("RightThick"));
		OpeningData.Width = OpeningJsonObject->GetNumberField(TEXT("Width"));
		if (OpeningJsonObject->HasField(TEXT("OwnerWall")))
		{
			OpeningData.OwnerWallID = OpeningJsonObject->GetStringField(TEXT("OwnerWall"));
		}
		else if (OpeningJsonObject->HasField(TEXT("OwnerWallID")))
		{
			OpeningData.OwnerWallID = OpeningJsonObject->GetStringField(TEXT("OwnerWallID"));
		}
		
		OpeningData.bXFlip = OpeningJsonObject->GetBoolField(TEXT("XFlip"));
		OpeningData.bYFlip = OpeningJsonObject->GetBoolField(TEXT("YFlip"));
		OpeningData.OpeningType = (EOpeningType)OpeningJsonObject->GetIntegerField(TEXT("Type"));
		OpeningData.Position.X = LocationJsonObject->GetNumberField(TEXT("X"));
		OpeningData.Position.Y = LocationJsonObject->GetNumberField(TEXT("Y"));
		double OpeningZPos = 0.0f;
		if (LocationJsonObject->TryGetNumberField(TEXT("Z"), OpeningZPos))
		{
			OpeningData.ZPos = (float)OpeningZPos;
		}
		else
		{
			OpeningData.ZPos = GetDefOpeningZPosByType(OpeningData.OpeningType);
		}
		double OpeningHeight = 0.0f;
		if (OpeningJsonObject->TryGetNumberField(TEXT("Height"), OpeningHeight))
		{
			OpeningData.Height = (float)OpeningHeight;
		}
		else
		{
			OpeningData.Height = GetDefOpeningHeightByType(OpeningData.OpeningType);
		}

		OpeningNodes.PooledDatas.Add(OpeningData);
	}
}

void UProjectPaper::LoadTextLabelsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject)
{
	const TArray< TSharedPtr<FJsonValue> >& TextLabels = HouseJsonObject->GetArrayField(TEXT("TextLabels"));
	if (TextLabels.Num() < 1)
	{
		return;
	}
	const TSharedPtr<FJsonValue>& RulerLabel = TextLabels[0];
	const TSharedPtr<FJsonObject> &RulerLabelJsonObj = RulerLabel->AsObject();
	RulerData.RulerLength = RulerLabelJsonObj->GetNumberField(TEXT("RulerLength"));
	RulerData.RulerPixelInPic = RulerLabelJsonObj->GetNumberField(TEXT("RulerPixelInPic"));
}

void UProjectPaper::MakeRelation()
{
	// do adapter
	// reverse x and y
	for (auto &CornerData : CornerNodes.PooledDatas)
	{
		Swap(CornerData.Position.X, CornerData.Position.Y);
		CornerData.Position.X *= 0.1f;
		CornerData.Position.Y *= 0.1f;
		CornerData.Position.X *= -1.0f;
	}

	for (auto &OpeningData : OpeningNodes.PooledDatas)
	{
		Swap(OpeningData.Position.X, OpeningData.Position.Y);
		OpeningData.Position.X *= 0.1f;
		OpeningData.Position.Y *= 0.1f;
		OpeningData.Position.X *= -1.0f;
		OpeningData.Width *= 0.1f;
		OpeningData.LeftThick *= 0.1f;
		OpeningData.RightThick *= 0.1f;
		Swap(OpeningData.LeftThick, OpeningData.RightThick);
		if (OpeningData.OpeningType == EOpeningType::BayWindow)
		{
			// correct ML's result
			Swap(OpeningData.bXFlip, OpeningData.bYFlip);
		}
	}

	for (auto &WallData : WallNodes.PooledDatas)
	{
		WallData.LeftThick *= 0.1f;
		WallData.RightThick *= 0.1f;
		Swap(WallData.LeftThick, WallData.RightThick);
	}

	// wall corner relation
	for (auto &WallData : WallNodes.PooledDatas)
	{
		bool bStartCornerFound = false;
		bool bEndCornerFound = false;
		int32 CornerCnt = CornerNodes.PooledDatas.Num();
		for (int32 CornerIdx = 0; CornerIdx < CornerCnt; ++CornerIdx)
		{
			auto &CornerData = CornerNodes.PooledDatas[CornerIdx];
			if (CornerData.ID == WallData.StartCornerID)
			{
				bStartCornerFound = true;
				WallData.StartCornerIndex = CornerIdx;
			}
			else if (CornerData.ID == WallData.EndCornerID)
			{
				bEndCornerFound = true;
				WallData.EndCornerIndex = CornerIdx;
			}

			if (bStartCornerFound && bEndCornerFound)
			{
				break;
			}
		}
	}

	// opening wall relation
	for (auto &OpeningData : OpeningNodes.PooledDatas)
	{
		int32 WallCnt = WallNodes.PooledDatas.Num();
		OpeningData.OwnerWallIndex = 0;
		for (int32 WallIdx = 0; WallIdx < WallCnt; ++WallIdx)
		{
			auto &WallData = WallNodes.PooledDatas[WallIdx];
			if (OpeningData.OwnerWallID == WallData.ID)
			{
				OpeningData.OwnerWallIndex = WallIdx;
			}
		}
	}

	// do some correction
	for (FOpeningData &OpeningData : OpeningNodes.PooledDatas)
	{
		FWallData& OwnerWallData = WallNodes.PooledDatas[OpeningData.OwnerWallIndex];
		FCornerData &StartCornerData = CornerNodes.PooledDatas[OwnerWallData.StartCornerIndex];
		FCornerData &EndCornerData = CornerNodes.PooledDatas[OwnerWallData.EndCornerIndex];
		FVector OpeningProjectPos = FMath::ClosestPointOnInfiniteLine(FVector(StartCornerData.Position, 0),
			FVector(EndCornerData.Position, 0), 
			FVector(OpeningData.Position, 0));
		OpeningData.Position = FVector2D(OpeningProjectPos);
	}
}

double UProjectPaper::GetDefOpeningZPosByType(EOpeningType InType)
{
	double OpeningZ = 0.0f;
	switch (InType)
	{
	case EOpeningType::Window:
	{
		OpeningZ = 90.0f;
	}
	break;
	case EOpeningType::BayWindow:
	{
		OpeningZ = 50.0f;
	}
	break;
	case EOpeningType::FrenchWindow:
	{
		OpeningZ = 15.0f;
	}
	break;
	}

	return OpeningZ;
}

double UProjectPaper::GetDefOpeningHeightByType(EOpeningType InType)
{
	double OpeningHeight = 0.0f;
	switch (InType)
	{
	case EOpeningType::DoorWay:
	case EOpeningType::SingleDoor:
	case EOpeningType::DoubleDoor:
	case EOpeningType::SlidingDoor:
	case EOpeningType::BayWindow:
	{
		OpeningHeight = 210.0f;
	}
	break;
	case EOpeningType::Window:
	{
		OpeningHeight = 150.0f;
	}
	break;
	case EOpeningType::FrenchWindow:
	{
		OpeningHeight = 230.0f;
	}
	break;
	}

	return OpeningHeight;
}

FString UProjectPaper::GetOpeningNameByType(EOpeningType InType)
{
	FString OpeningName;
	switch (InType)
	{
		case EOpeningType::DoorWay:
		{
			OpeningName = TEXT("门洞");
		}
		break;
		case EOpeningType::SingleDoor:
		{
			OpeningName = TEXT("单开门");
		}
		break;
		case EOpeningType::DoubleDoor:
		{
			OpeningName = TEXT("双开门");
		}
		break; 
		case EOpeningType::SlidingDoor:
		{
			OpeningName = TEXT("推拉门");
		}
		break;
		case EOpeningType::Window:
		{
			OpeningName = TEXT("窗");
		}
		break;
		case EOpeningType::BayWindow:
		{
			OpeningName = TEXT("飘窗");
		}
		break;
		case EOpeningType::FrenchWindow:
		{
			OpeningName = TEXT("落地窗");
		}
		break;
	}

	return OpeningName;
}

FTextLabelData UProjectPaper::GetRulerData()
{
	return RulerData;
}
