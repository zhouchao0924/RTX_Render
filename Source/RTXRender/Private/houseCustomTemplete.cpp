// Copyright? 2017 ihomefnt All Rights Reserved.

#include "houseCustomTemplete.h"
#include "HouseFunction/HouseFunctionLibrary.h"
#include "LuoLuoStruct.h"
#include "../Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"


UhouseCustomTemplete::UhouseCustomTemplete()
{
	MeshVisieble = true;
	IsChangeColor = false;
}

void UhouseCustomTemplete::SetColorData(FString Key, UVaRestJsonObject* ColorJson)
{
	FMaterialColorData dcolor;
	FString MD5;
	dcolor.propertyId = ColorJson->GetNumberField("propertyId");
	auto propertyValueList = ColorJson->GetObjectArrayField("propertyValueList");
	for (auto i : propertyValueList)
	{
		if (i->GetBoolField("selectFlag"))
		{
			dcolor.propertyValueCode = i->GetNumberField("propertyValueCode");
			dcolor.propertyValueId = i->GetNumberField("propertyValueId");
			MD5 = i->GetStringField("mxFileMD5");
			dcolor.MD5 = i->GetStringField("mxFileMD5");
				break;
		}
	}
	ColorMD5.Add(Key, MD5);
	CurrentColorData.Add(Key, dcolor);
	privateColorData.Add(Key, dcolor);
}

void UhouseCustomTemplete::InitDataByJson(UVaRestJsonObject* componentListObjectJson)
{
	FString CustomTramsformStr{};


	if (componentListObjectJson)
	{
		guid =componentListObjectJson->GetStringField("guid");
		PrmitivefileNum = fileNum=componentListObjectJson->GetStringField("mxFileMD5");
		modelId = componentListObjectJson->GetNumberField("modelId");
		CategoryName = componentListObjectJson->GetStringField("categoryName");
		MaterialName = componentListObjectJson->GetStringField("name");
		CategoryId = componentListObjectJson->GetNumberField("categoryId");
		UVaRestJsonObject * customJson = componentListObjectJson->GetObjectField("customJson");
		CustomTramsformStr= customJson->GetStringField("transform");
		associatedAnchor.parentGuid = customJson->GetStringField("parentGuid");
		bool b;
		// UKismetStringLibrary::Conv_StringToRotator(CustomTramsformStr, associatedAnchor.CustomTramsform,b);
		
		 //associatedAnchor.CustomTramsform.InitFromString(CustomTramsformStr);
		associatedAnchor.CustomTramsform = UHouseFunctionLibrary::StringToTransform(CustomTramsformStr);

		//TArray<FString> tempSelfPinsStr= customJson->GetStringArrayField("selfPins");
		TArray<UVaRestJsonObject*> tempSelfPins=customJson->GetObjectArrayField("selfPins");
		for (int i = 0; i < tempSelfPins.Num(); ++i)
		{
			FSelfPins selfPinOne{};
			FString pinLocation = tempSelfPins[i]->GetStringField("pinLocation");
			selfPinOne.bInnerCapacity = tempSelfPins[i]->GetBoolField("bInnerCapacity");
			UKismetStringLibrary::Conv_StringToVector(pinLocation, selfPinOne.selfPinsVec, b);
			associatedAnchor.selfPinsArr.Add(selfPinOne);

		}
		auto sizeRange = componentListObjectJson->GetObjectField("sizeRange");
		if (sizeRange)
		{
			MaxMeshSize.X = sizeRange->GetNumberField("maxLength");
			MinMeshSize.X = sizeRange->GetNumberField("minLength");

			MaxMeshSize.Y = sizeRange->GetNumberField("maxWidth");
			MinMeshSize.Y = sizeRange->GetNumberField("minWidth");

			MaxMeshSize.Z = sizeRange->GetNumberField("maxHeight");
			MinMeshSize.Z = sizeRange->GetNumberField("minHeight");
		}

		//for (int i = 0; i < tempSelfPinsStr.Num(); ++i)
		//{
		//	FVector selfPinsStr;
		//	UKismetStringLibrary::Conv_StringToVector(tempSelfPinsStr[i], selfPinsStr, b);
		//	associatedAnchor.selfPinsVecArr.Add(selfPinsStr);
		//}


		associatedAnchor.doorHandleOffset= customJson->GetNumberField("doorHandleOffset");
		associatedAnchor.uvAngle = customJson->GetNumberField("uvAngle");
		TArray<UVaRestJsonObject*> connectPinsArr = customJson->GetObjectArrayField("connectPins");

		auto MeshNameList = customJson->GetObjectArrayField("meshNameList");
		for (auto i : MeshNameList)
		{
			FString Name = i->GetStringField("meshName");
			FString size = i->GetStringField("meshSize");
			FVector exetent;
			bool bIs;
			UKismetStringLibrary::Conv_StringToVector(size, exetent, bIs);
			if (bIs)
				associatedAnchor.MeshSize.Add(Name, exetent);
		}

		auto comsize = customJson->GetStringField("componentSize");
		if (!comsize.IsEmpty())
		{
			bool bIs;
			UKismetStringLibrary::Conv_StringToVector(comsize, associatedAnchor.componentSize, bIs);
		}


		for (int i = 0; i < connectPinsArr.Num(); ++i)
		{
			FConnectPins pin;
			pin.parentIndex= connectPinsArr[i]->GetNumberField("parentIndex");
			pin.slefIndex = connectPinsArr[i]->GetNumberField("slefIndex");
			associatedAnchor.connectPins.Add(pin);
		}
		auto AttrJson = componentListObjectJson->GetObjectField("attrJson");

		if (!AttrJson)
			return;

		if (componentListObjectJson->GetStringField("categoryName").Find(TEXT("拉手")) > -1)
		{
			auto handleDirectionJson = AttrJson->GetObjectField("handleDirection");
			if (handleDirectionJson)
			{
				auto handlepropertyValueList = handleDirectionJson->GetObjectArrayField("propertyValueList");
				for (auto i : handlepropertyValueList)
				{
					if (i->GetBoolField("selectFlag"))
					{
						associatedAnchor.doorhandleDirection = i->GetNumberField("propertyValueCode");
						break;
					}
				}
			}
		}

		if (componentListObjectJson->GetStringField("categoryName").Find(TEXT("内构")) > -1)
		{
			auto StructureJson = AttrJson->GetObjectField("structure");
			if (StructureJson)
			{
				auto PropertyValueListJson = StructureJson->GetObjectArrayField("propertyValueList");
				if (PropertyValueListJson.Num() > 0)
				{
					associatedAnchor.InternalsType = PropertyValueListJson[0]->GetNumberField("propertyValueCode");
				}
			}
		}
		ChargeUnit = componentListObjectJson->GetNumberField("chargeUnit");
		ComponentID = componentListObjectJson->GetNumberField("componentId");
		price = componentListObjectJson->GetNumberField("price");
		num = componentListObjectJson->GetNumberField("num");
		materialId = componentListObjectJson->GetNumberField("materialId");

	
		auto ColorJson = AttrJson->GetObjectField("color");
		if (ColorJson)
			SetColorData("mesh_color1", ColorJson);
		else
		{
			auto ColorJson = AttrJson->GetObjectField("coreColor");
			if(ColorJson)
				SetColorData("mesh_door2", ColorJson);

			auto doorFrameColorJson = AttrJson->GetObjectField("doorFrameColor");
			if(doorFrameColorJson)
				SetColorData("mesh_door1", doorFrameColorJson);
		}
	}
}


void UhouseCustomTemplete::SetParentHouseCustomTempleteByGuid(TArray<UhouseCustomTemplete*> subModelComponentArr)
{
	for (int32 i = 0; i < subModelComponentArr.Num(); ++i)
	{
		if(this->guid== subModelComponentArr[i]->guid)
			continue;

		if ((this->associatedAnchor).parentGuid == subModelComponentArr[i]->guid)
		{
			parentHouseCustomTemplete = subModelComponentArr[i];
			return;
		}
	}
	parentHouseCustomTemplete = NULL;
}

void UhouseCustomTemplete::SetElementID(int32 id)
{
	ElementID = id;
}

UVaRestJsonObject * UhouseCustomTemplete::getquantitiesjson(FVector scale, float deflength, float defwidth, float defhight)
{
	UVaRestJsonObject* tempjson = UVaRestJsonObject::ConstructJsonObject(this);
	float quantity = 0.0f;
	float length = 10*Defaultsize.Y;
	float width=10*Defaultsize.X;
	float height= 10*Defaultsize.Z;
	int index = 0;
	if (CategoryName.Contains(TEXT("台面"))) {
		index = 1;
		length = deflength * scale.X;
		width = defwidth * scale.Y;
	}
	if (CategoryName.Contains(TEXT("踢脚板"))) {
		index = 2;
		length = deflength * scale.X;
		width = defwidth * scale.Y;
	}
	if (CategoryName.Contains(TEXT("顶角线"))) {
		index = 3;
		length = deflength * scale.X;
		width = defwidth * scale.Y;
	}

	switch (ChargeUnit)
	{
	case 0:
		quantity = length *0.001f;
		break;
	case 1:
		quantity = (float)num;
		break;
	case 2:
		quantity = length *0.001f;
		break;
	case 3:
		quantity = (length + width)*0.002f;
		break;
	case 4:
		quantity = length * height*0.001f*0.001f;
		break;
	case 5:
		quantity = width * length*0.001f*0.001f;
		break;
	default:
		quantity = length *0.001f;
		break;
	}
	FBigDecimal bigquantity = ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithFloat(quantity);
	FString quantitystr = ULuoLuoBlueprintFunctionLibrary::ConverToStringWithPrecisionControl(bigquantity, 2, true);
	UVaRestJsonObject* attrJson = UVaRestJsonObject::ConstructJsonObject(this);
	bool hascolor = false;
	if (CurrentColorData.Contains("mesh_color1")) {
		hascolor = true;
		FMaterialColorData *colorstruct = CurrentColorData.Find("mesh_color1");
		UVaRestJsonObject* colorjson = UVaRestJsonObject::ConstructJsonObject(this);
		colorjson->SetNumberField("propertyId", colorstruct->propertyId);
		TArray<UVaRestJsonObject*>colorInArray;
		UVaRestJsonObject* colorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
		//colorpropertyjson->SetNumberField("propertyValueCode", 34);
		colorpropertyjson->SetNumberField("propertyValueCode", (float)colorstruct->propertyValueCode);
		colorInArray.Add(colorpropertyjson);
		colorjson->SetObjectArrayField("propertyValueList", colorInArray);
		attrJson->SetObjectField("color", colorjson);
	}
	if (CurrentColorData.Contains("mesh_door2")) {
		hascolor = true;
		FMaterialColorData *coreColorstruct = CurrentColorData.Find("mesh_door2");
		UVaRestJsonObject* coreColorjson = UVaRestJsonObject::ConstructJsonObject(this);
		coreColorjson->SetNumberField("propertyId", coreColorstruct->propertyId);
		TArray<UVaRestJsonObject*>coreColorjsonInArray;
		UVaRestJsonObject* coreColorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
		//coreColorpropertyjson->SetNumberField("propertyValueCode", 34);
		coreColorpropertyjson->SetNumberField("propertyValueCode", (float)coreColorstruct->propertyValueCode);
		coreColorjsonInArray.Add(coreColorpropertyjson);
		coreColorjson->SetObjectArrayField("propertyValueList", coreColorjsonInArray);
		attrJson->SetObjectField("coreColor", coreColorjson);
	}
	if (CurrentColorData.Contains("mesh_door1")) {
		hascolor = true;
		FMaterialColorData *doorFrameColorstruct = CurrentColorData.Find("mesh_door1");
		UVaRestJsonObject* doorFrameColorjson = UVaRestJsonObject::ConstructJsonObject(this);
		doorFrameColorjson->SetNumberField("propertyId", doorFrameColorstruct->propertyId);
		TArray<UVaRestJsonObject*>doorFrameColorInArray;
		UVaRestJsonObject* doorFrameColorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
		//doorFrameColorpropertyjson->SetNumberField("propertyValueCode", 34);
		doorFrameColorpropertyjson->SetNumberField("propertyValueCode", (float)doorFrameColorstruct->propertyValueCode);
		doorFrameColorInArray.Add(doorFrameColorpropertyjson);
		doorFrameColorjson->SetObjectArrayField("propertyValueList", doorFrameColorInArray);
		attrJson->SetObjectField("doorFrameColor", doorFrameColorjson);
	}
	
	if (hascolor) {
	tempjson->SetObjectField("attrJson", attrJson);
	}
	tempjson->SetNumberField("categoryId", (float)CategoryId);
	tempjson->SetNumberField("componentId", (float)ComponentID);
	tempjson->SetNumberField("modelId",(float)modelId);
	tempjson->SetNumberField("materialId", (float)materialId);
	tempjson->SetNumberField("num", (float)num);
	tempjson->SetNumberField("price", (float)price);
	//tempjson->SetNumberField("quantities", quantity);
	tempjson->SetStringField("quantities", quantitystr);
	
	return tempjson;
}


UhouseCustomTemplete* UHouseCustomTempleteFunctionLibrary::CreatehouseCustomTemplete(UVaRestJsonObject* componentListObjectJson)
{
	UhouseCustomTemplete* CustomTemplete = NewObject<UhouseCustomTemplete>();
	if (CustomTemplete)
	{
		CustomTemplete->InitDataByJson(componentListObjectJson);


	}

	return CustomTemplete;
}

void UHouseCustomTempleteFunctionLibrary::RestoreTemplateDataan(bool IsTable, TArray<UhouseCustomTemplete*>subModelComponentArr, UCompundResourceMeshComponent* compoundElementInterface, int32 rootID, TMap<FString, UhouseCustomTemplete*>&Componentlist)
{


		for (int i = 0; i < subModelComponentArr.Num(); ++i)
		{
			if(subModelComponentArr[i]->CategoryName.Find(TEXT("踢脚板")) > -1 || subModelComponentArr[i]->CategoryName.Find(TEXT("顶角线")) > -1)
				continue;

			if (IsTable && subModelComponentArr[i]->CategoryName.Find(TEXT("台面")) > -1)
				continue;

			subModelComponentArr[i]->SetParentHouseCustomTempleteByGuid(subModelComponentArr);
			//通过guid得到父类物料
			auto Tempar = Componentlist.Find(subModelComponentArr[i]->associatedAnchor.parentGuid);
			if (!subModelComponentArr[i]->associatedAnchor.parentGuid.IsEmpty())
				subModelComponentArr[i]->parentHouseCustomTemplete = *Tempar;
			else
				subModelComponentArr[i]->parentHouseCustomTemplete = nullptr;

			//创建物料位置
			if (subModelComponentArr[i]->associatedAnchor.connectPins.Num() != 0)
			{
				int id = compoundElementInterface->AddElement(subModelComponentArr[i]->fileNum);
				compoundElementInterface->SetElementTransform(id, subModelComponentArr[i]->associatedAnchor.CustomTramsform.Rotator(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetLocation(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetScale3D());
				subModelComponentArr[i]->SetElementID(id);
			}
			else
			{
				subModelComponentArr[i]->SetElementID(rootID);
				compoundElementInterface->SetElementTransform(rootID, subModelComponentArr[i]->associatedAnchor.CustomTramsform.Rotator(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetLocation(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetScale3D());
			}

			//创建Pin
			const TArray<FConnectPins>& connectPins = subModelComponentArr[i]->associatedAnchor.connectPins;
			const TArray<FSelfPins>&selfPinsVecArr = subModelComponentArr[i]->associatedAnchor.selfPinsArr;
			const UhouseCustomTemplete* parentHouseCustomTemplete = subModelComponentArr[i]->parentHouseCustomTemplete;

			for (int j = 0; j < connectPins.Num(); ++j)
			{
			
				if (parentHouseCustomTemplete->associatedAnchor.selfPinsArr.Num() <= connectPins[j].parentIndex || parentHouseCustomTemplete->associatedAnchor.selfPinsArr.Num() <= connectPins[j].slefIndex)
				{
					UE_LOG(LogTemp, Warning, TEXT("connectPins Array Bounds"));
					return;
				}


				int slefID = compoundElementInterface->CreatePin(subModelComponentArr[i]->ElementID, selfPinsVecArr[connectPins[j].slefIndex].selfPinsVec, selfPinsVecArr[connectPins[j].slefIndex].bInnerCapacity);
				subModelComponentArr[i]->Pins.Add(slefID);

				int parenID = compoundElementInterface->CreatePin(parentHouseCustomTemplete->ElementID, parentHouseCustomTemplete->associatedAnchor.selfPinsArr[connectPins[j].parentIndex].selfPinsVec, parentHouseCustomTemplete->associatedAnchor.selfPinsArr[connectPins[j].parentIndex].bInnerCapacity);
				subModelComponentArr[i]->associatedAnchor.parentPins.Add(parenID);
			}

			if (subModelComponentArr[i]->associatedAnchor.connectPins.Num() != 0)
			{
				FCompoundElementInterface FixedInterface, SubInterface;
		
				FixedInterface.ElementID = subModelComponentArr[i]->parentHouseCustomTemplete->ElementID;
				FixedInterface.Pins = subModelComponentArr[i]->associatedAnchor.parentPins;
				SubInterface.ElementID = subModelComponentArr[i]->ElementID;
				SubInterface.Pins = subModelComponentArr[i]->Pins;
				if (!compoundElementInterface->Connect(FixedInterface, SubInterface))
				{
					UE_LOG(LogTemp, Warning, TEXT("connect return fail"));
				}
			}

		}



	//for (int i = 0; i < subModelComponentArr.Num(); ++i)
	//{
	//	if(subModelComponentArr[i]->CategoryName.Find(TEXT("踢脚板")) > -1 || subModelComponentArr[i]->CategoryName.Find(TEXT("顶角线")) > -1)
	//		continue;

	//	if (IsTable && subModelComponentArr[i]->CategoryName.Find(TEXT("台面")) > -1)
	//		continue;

	//	subModelComponentArr[i]->SetParentHouseCustomTempleteByGuid(subModelComponentArr);
	//}
	//for (int i = 0; i < subModelComponentArr.Num(); ++i)
	//{
	//	if (subModelComponentArr[i]->CategoryName.Find(TEXT("踢脚板")) > -1 || subModelComponentArr[i]->CategoryName.Find(TEXT("顶角线")) > -1)
	//		continue;

	//	if(IsTable && subModelComponentArr[i]->CategoryName.Find(TEXT("台面")) > -1)
	//		continue;

	//	if (subModelComponentArr[i]->associatedAnchor.connectPins.Num() != 0)
	//	{
	//		int id = compoundElementInterface->AddElement(subModelComponentArr[i]->fileNum);
	//		compoundElementInterface->SetElementTransform(id, subModelComponentArr[i]->associatedAnchor.CustomTramsform.Rotator(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetLocation(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetScale3D());
	//		subModelComponentArr[i]->SetElementID(id);
	//	}	
	//	else
	//	{
	//		subModelComponentArr[i]->SetElementID(rootID);
	//		compoundElementInterface->SetElementTransform(rootID, subModelComponentArr[i]->associatedAnchor.CustomTramsform.Rotator(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetLocation(), subModelComponentArr[i]->associatedAnchor.CustomTramsform.GetScale3D());
	//	}
	//}
	//for (int i = 0; i < subModelComponentArr.Num(); ++i)
	//{
	//	if (subModelComponentArr[i]->CategoryName.Find(TEXT("踢脚板")) > -1 || subModelComponentArr[i]->CategoryName.Find(TEXT("顶角线")) > -1)
	//		continue;

	//	if (IsTable && subModelComponentArr[i]->CategoryName.Find(TEXT("台面")) > -1)
	//		continue;

	//	const TArray<FConnectPins>& connectPins=subModelComponentArr[i]->associatedAnchor.connectPins;
	//
	//	const TArray<FSelfPins>&selfPinsVecArr= subModelComponentArr[i]->associatedAnchor.selfPinsArr;

	//	const UhouseCustomTemplete* parentHouseCustomTemplete = subModelComponentArr[i]->parentHouseCustomTemplete;

	//
	//	for (int j = 0; j < connectPins.Num(); ++j)
	//	{
	//	
	//		if (parentHouseCustomTemplete->associatedAnchor.selfPinsArr.Num() <= connectPins[j].parentIndex || parentHouseCustomTemplete->associatedAnchor.selfPinsArr.Num() <= connectPins[j].slefIndex)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("connectPins Array Bounds"));
	//			return;
	//		}


	//		int slefID= compoundElementInterface->CreatePin(subModelComponentArr[i]->ElementID, selfPinsVecArr[connectPins[j].slefIndex].selfPinsVec, selfPinsVecArr[connectPins[j].slefIndex].bInnerCapacity);
	//		subModelComponentArr[i]->Pins.Add(slefID);

	//		int parenID = compoundElementInterface->CreatePin(parentHouseCustomTemplete->ElementID, parentHouseCustomTemplete->associatedAnchor.selfPinsArr[connectPins[j].parentIndex].selfPinsVec, parentHouseCustomTemplete->associatedAnchor.selfPinsArr[connectPins[j].parentIndex].bInnerCapacity);
	//		subModelComponentArr[i]->associatedAnchor.parentPins.Add(parenID);

	//	}
	//}

	//for (int i = 0; i < subModelComponentArr.Num(); ++i)
	//{
	//	if (subModelComponentArr[i]->CategoryName.Find(TEXT("踢脚板")) > -1 || subModelComponentArr[i]->CategoryName.Find(TEXT("顶角线")) > -1)
	//		continue;

	//	if (IsTable && subModelComponentArr[i]->CategoryName.Find(TEXT("台面")) > -1)
	//		continue;

	//	if (subModelComponentArr[i]->associatedAnchor.connectPins.Num() != 0)
	//	{
	//		FCompoundElementInterface FixedInterface, SubInterface;
	//		FixedInterface.ElementID = subModelComponentArr[i]->parentHouseCustomTemplete->ElementID;
	//		FixedInterface.Pins = subModelComponentArr[i]->associatedAnchor.parentPins;
	//		SubInterface.ElementID = subModelComponentArr[i]->ElementID;
	//		SubInterface.Pins = subModelComponentArr[i]->Pins;
	//		if (!compoundElementInterface->Connect(FixedInterface, SubInterface))
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("connect return fail"));
	//		}
	//	}
	//	
	//}
}
