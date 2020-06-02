// Copyright? 2017 ihomefnt All Rights Reserved.

#include "HouseCustomActor.h"
#include "BuildingSDKSystem.h"
#include "BuildingSDK.h"
#include "ResourceMeshComponent.h"
#include "BuildingGroupMeshComponent.h "
#include "VaRestRequestJSON.h"
#include "HouseFunction/SkuComponentActor.h"
#include "ResourceMgr.h"
#include "LuoLuoStruct.h"
#include "../Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"
#include "../Building/BuildingSystem.h"
#include "HouseFunction/ManageObjectC.h"
#include "HouseFunction/HouseFunctionLibrary.h"
#include "ModelFile.h"



AHouseCustomActor::AHouseCustomActor():CabinetBodyName0("sinkcut_0"),CabinetBodyName1("sinkcut_1")
{
	IsTable = false;
	Versions = -1;
	loft.bIsSkirting = true;
	loft.bIsTable = true;
	loft.bIsTopboard = true;
}

void AHouseCustomActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

bool AHouseCustomActor::InitCompoundElementRootByResID(const FString& ResID)
{
	UWorld *world = GetWorld();

	if (!compoundElementInterface && world)
	{
		ABuildingSDKSystem* buildingSDKSystem = ABuildingSDKSystem::GetSDKActor();
		UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();

		int32 id = buildingSDKSystem->ResourceMgr->CreateCompoundResource();
		SetResource(buildingSDKSystem->SolutionComponent, id);

		compoundElementInterface = Cast<UCompundResourceMeshComponent>(ResourceMeshComp);
		if (compoundElementInterface)
		{
			RootElementID=compoundElementInterface->AddElement(ResID);
		}

		return true;
	}

	return false;
}


void AHouseCustomActor::RestoreTemplateObject(UVaRestJsonObject* JsonObj)
{
	if (JsonObj)
	{
		PrimitiveTempleteJson = JsonObj;
		IsTable = JsonObj->GetBoolField("settingOutFlag");
		FString vec = JsonObj->GetStringField("templateSize");
		bool t = false;
		UKismetStringLibrary::Conv_StringToVector(vec, TemplateSize, t);
		
		FVector maxsize, minsize;
		maxsize.X = JsonObj->GetNumberField("maxLength");
		minsize.X = JsonObj->GetNumberField("minLength");

		maxsize.Y = JsonObj->GetNumberField("maxWidth");
		minsize.Y = JsonObj->GetNumberField("minWidth");

		maxsize.Z = JsonObj->GetNumberField("maxHeight");
		minsize.Z = JsonObj->GetNumberField("minHeight");
		TemplateMaxScale = maxsize / TemplateSize;
		TemplateMinScale = minsize / TemplateSize;



		Url = JsonObj->GetStringField("mainUrl");
		const TArray<UVaRestJsonObject*> SkuList =	JsonObj->GetObjectArrayField("hardSkuList");






		for (auto i : SkuList)
		{
			FSkuData Temp;
			Temp.Name = i->GetStringField("name");
			Temp.modelid = i->GetNumberField("modelId");
			Temp.MD5 = i->GetStringField("mxFileMD5");
			Temp.skuid = i->GetNumberField("skuId");
			Temp.Categoryid = i->GetNumberField("drCategoryId");
			Skuinfo.Add(i->GetNumberField("drCategoryId"), Temp);
		}


		SetTempleteJson(JsonObj);
		const TArray<UVaRestJsonObject*> componentListArr = TempleteJson->GetObjectArrayField("componentList");
		for (int i = 0; i < componentListArr.Num(); i++)
		{
			const UVaRestJsonObject* customJson = componentListArr[i]->GetObjectField("customJson");
			if (customJson != nullptr)
			{
				InitCompoundElementRootByResID(componentListArr[i]->GetStringField("mxFileMD5"));

				break;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("get customJson fail"));
				return;
			}


		}
		TArray<UVaRestJsonObject*> Meshjson;
		TArray<UhouseCustomTemplete*> Lashou;
		bool IsLashou = true;
		subModelComponentArr.Empty();
		for (int i = 0; i < componentListArr.Num(); i++)
		{
			UhouseCustomTemplete* oneSubModelComponent = UHouseCustomTempleteFunctionLibrary::CreatehouseCustomTemplete(componentListArr[i]);
			oneSubModelComponent->ComponentJson = componentListArr[i];
			if (oneSubModelComponent->CategoryName.Find(TEXT("门板")) > -1 && componentListArr[i]->GetObjectField("attrJson") != nullptr &&  componentListArr[i]->GetObjectField("attrJson")->GetObjectField("handlesNumber") != nullptr)
			{
				UVaRestJsonObject* Temp = componentListArr[i]->GetObjectField("attrJson")->GetObjectField("handlesNumber");
				if (Temp)
				{
					Temp = Temp->GetObjectArrayField("propertyValueList")[0];
					IsLashou = Temp->GetNumberField("propertyValueCode");
					Meshjson.Add(Temp);
				}
			}
			else if (oneSubModelComponent->CategoryName.Find(TEXT("拉手")) > -1)
				Lashou.Add(oneSubModelComponent);
			ComponentList.Add(oneSubModelComponent->guid, oneSubModelComponent);
			subModelComponentArr.Add(oneSubModelComponent);

			auto Tempvalue = ComponentType.Find(oneSubModelComponent->CategoryName);
			if (Tempvalue)
			{
				(*Tempvalue).Add(oneSubModelComponent);
				ComponentType.Add(oneSubModelComponent->CategoryName, (*Tempvalue));
			}
			else
			{
				TArray<UhouseCustomTemplete*> templist;
				templist.Add(oneSubModelComponent);
				ComponentType.Add(oneSubModelComponent->CategoryName, templist);
			}

		}

		UHouseCustomTempleteFunctionLibrary::RestoreTemplateDataan(IsTable,subModelComponentArr, compoundElementInterface, RootElementID,ComponentList);


		//根据门板数据是否隐藏拉手
		if (!IsLashou && Lashou.Num())
		{
			for (auto i : Lashou)
				SetMatching(i, false);
		}

	
	/*	for (int i = 0; i < subModelComponentArr.Num(); i++)
		{
			if (subModelComponentArr[i]->CategoryName.Find(TEXT("拉手")) > -1 && Meshjson.Num() > 0)
			{
				if (!IsLashou)
				{

					SetMatching(subModelComponentArr[i], false);
				
				}
			}

		}*/

		
	}
}

void AHouseCustomActor::RemoveSkudata(int32 id)
{
	SkuComponent.Remove(id);
	Skuinfo.Remove(id);
}

void AHouseCustomActor::SetCabinetBodyVisible(UhouseCustomTemplete* Compoennt, int32 CaType)
{
	switch (CaType)
	{
	case 0: {  compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName1, false);
		compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName0, false);
	}break;
	case 1: {	compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName0, false);
		compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName1, true);
	}break;
	case 2: {	compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName1, false);
		compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName0, true);
	}break;
	case 3: {   compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName1, true);
		compoundElementInterface->SetElementVisibleByMeshName(Compoennt->ElementID, CabinetBodyName0, true);
	}break;
	}

	Compoennt->associatedAnchor.CabinetBodyType = CaType;
}
void AHouseCustomActor::SetInitMeshMaterialData(int32 Ver, int32 index, FString guid, bool IsVisible,int32 InType, int32 CaType, int32 handlepos, int32 handlero)
{
	if (subModelComponentArr.Num() > index && IsVisible)
	{
		if (subModelComponentArr[index]->guid == guid)
		{
			//auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[index]->ElementID);
			//if (Mesh)
			//	Mesh->SetVisibility(IsVisible);
			SetMaterialVisible(subModelComponentArr[index], IsVisible);

			subModelComponentArr[index]->MeshVisieble = IsVisible;
			subModelComponentArr[index]->associatedAnchor.InternalsType = InType;
			if (Ver > 1 && subModelComponentArr[index]->CategoryName.Find(TEXT("柜身")) > -1)
				SetCabinetBodyVisible(subModelComponentArr[index], CaType);
			else if (Ver > 2 && subModelComponentArr[index]->CategoryName.Find(TEXT("拉手")) > -1)
			{
				subModelComponentArr[index]->associatedAnchor.doorhandleDirection = handlero;
				subModelComponentArr[index]->associatedAnchor.doorHandleOffset = handlepos;
			}

		}
		else
		{
			for (auto i : subModelComponentArr)
			{
				if (i->guid == guid)
				{
				/*	auto Mesh = compoundElementInterface->FindComponentByElementID(i->ElementID);
					if (Mesh)
						Mesh->SetVisibility(IsVisible);*/
					SetMaterialVisible(subModelComponentArr[index], IsVisible);
					i->MeshVisieble = IsVisible;
					i->associatedAnchor.InternalsType = InType;

					if (Ver > 1 && subModelComponentArr[index]->CategoryName.Find(TEXT("柜身")) > -1)
						SetCabinetBodyVisible(subModelComponentArr[index], CaType);
				}
			}
		}
	
	}
	else if (!IsVisible && subModelComponentArr.Num() > index && subModelComponentArr[index]->guid == guid && subModelComponentArr[index]->CategoryName.Find(TEXT("拉手")) > -1)
	{
		SetMaterialVisible(subModelComponentArr[index], false);
		subModelComponentArr[index]->MeshVisieble = false;
	}
}

void AHouseCustomActor::GetMaterialScale()
{
	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		int x, y, z;
		FVector Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
		auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
		if (Mesh)
		{
			//auto size1 = Mesh->Bounds.BoxExtent * 20.f;
			compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);

			FVector size = FVector(x, y, z) * Scale;
			//FVector size = size1; //* Scale;
			subModelComponentArr[i]->ScaleInfo.Add(subModelComponentArr[i]->guid, size);
			subModelComponentArr[i]->Currentsize = size;
		}
	}
}

void  AHouseCustomActor::SetCabinetBodyVisible(int32 Meshtype)
{
	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		if (subModelComponentArr[i]->CategoryName.Find(TEXT("柜身")) > -1)
		{
			switch(Meshtype)
			{
			   case 0 : {  compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName1, false);
						   compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName0, false);
					    }break;
			   case 1: {	compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName0, false);
							compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName1, true);
						}break;
			   case 2: {	compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName1, false);
							compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName0, true);
						}break;
			   case 3: {   compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName1, true);
				           compoundElementInterface->SetElementVisibleByMeshName(subModelComponentArr[i]->ElementID, CabinetBodyName0, true);
						}break;
			}

			subModelComponentArr[i]->associatedAnchor.CabinetBodyType = Meshtype;
			return;
		}
	}
}


void AHouseCustomActor::SetMaterialScale(TMap<FString, FVector> info, int32 Ver)
{

	if (Ver == 0)
	{
		for (int i = 0; i < subModelComponentArr.Num(); i++)
		{
			auto Temp = info.Find(subModelComponentArr[i]->guid);
			if (Temp && !(*Temp).Equals(FVector(1.f,1.f,1.f),0.001))
			{
				FVector Loation;
				FRotator rotator;
				compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[i]->ElementID, Loation, rotator);
				compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, (*Temp));
			}
		}
	}
	else
	{
		for (int i = 0; i < subModelComponentArr.Num(); i++)
		{
			auto Temp = info.Find(subModelComponentArr[i]->guid);
			if (Temp)
			{
				if (subModelComponentArr[i]->CategoryName.Find(TEXT("拉手")) > -1)
					continue;

				FVector Loation;
				FRotator rotator;
				int x, y, z;
				//FVector Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
				compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);
				FVector Tempsize;
				Tempsize.Set(x, y, z);
				//Tempsize *= Scale;
				FVector TempScale = (*Temp) / Tempsize;
				if (TempScale.X > 500.f || TempScale.Y > 500.f || TempScale.Z > 500.f)
					return;

				compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[i]->ElementID, Loation, rotator);
				compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, TempScale);
			}
		}
	}
}


void AHouseCustomActor::SpecialMaterialData(const FString& CategoryName, const FString& ResID, const FString& MaterialResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size, int32 modelid, int32 ColorID, int32 ColorCode, int32 Propertyid)
{
	FString MeshName("mesh_color1");
	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		if (UKismetStringLibrary::FindSubstring(subModelComponentArr[i]->CategoryName, CategoryName) > -1)
		{
			subModelComponentArr[i]->fileNum = ResID;
			subModelComponentArr[i]->IsChangeColor = true;
			subModelComponentArr[i]->materialId = MaterialID;
			subModelComponentArr[i]->ChargeUnit = chargeUnit;
			subModelComponentArr[i]->price = price;
			subModelComponentArr[i]->modelId = modelid;
			auto temoclor = subModelComponentArr[i]->CurrentColorData.Find(MeshName);
			FMaterialColorData dcolor;
			if (temoclor)
				dcolor = *temoclor;
			dcolor.propertyId = Propertyid;
			dcolor.propertyValueId = ColorID;
			dcolor.propertyValueCode = ColorCode;
			dcolor.MD5 = MaterialResID;
			subModelComponentArr[i]->CurrentColorData.Add(MeshName, dcolor);
			subModelComponentArr[i]->ColorMD5.Add(MeshName, MaterialResID);
		}
	}
}

void AHouseCustomActor::SetTempleteJson(UVaRestJsonObject* JsonObj)
{
	TempleteJson = JsonObj;
}

void AHouseCustomActor::SetType(int32 Aid, int32 TempId, ECustomFurType ClassTypeID, int32 Categoryid, int32 Ver)
{
	ActorId = Aid;
	TemplateId = TempId;
	TemplateCategoryId = Categoryid;
	ClassType = ClassTypeID;
	Versions = Ver;
}

void AHouseCustomActor::SetMaterialVisible(UhouseCustomTemplete* component, bool bIs)
{
	if (component)
	{
		compoundElementInterface->SetElementVisibleByMeshName(component->ElementID, "", bIs);

		//auto Mesh = compoundElementInterface->FindComponentByElementID(component->ElementID);
		//	if (Mesh)
		//		Mesh->SetVisibility(bIs);
	}
}

void  AHouseCustomActor::SetMatching(UhouseCustomTemplete* component, bool bIs)
{
	if (component)
	{
		/*auto Mesh = compoundElementInterface->FindComponentByElementID(component->ElementID);
		if (Mesh)
			Mesh->SetVisibility(bIs);*/
		SetMaterialVisible(component, bIs);
		component->MeshVisieble = bIs;
	}
}

void AHouseCustomActor::RevetAllMesh()
{
	auto ComponentListJson = PrimitiveTempleteJson->GetObjectArrayField("componentList");
	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		
		if(subModelComponentArr[i]->fileNum != subModelComponentArr[i]->PrmitivefileNum)
			ReplaceModelByCategoryId(subModelComponentArr[i]->CategoryId, subModelComponentArr[i]->PrmitivefileNum, ComponentListJson[i]->GetNumberField("materialId"), ComponentListJson[i]->GetNumberField("chargeUnit"), ComponentListJson[i]->GetNumberField("price"),FVector(), subModelComponentArr[i]->modelId);
	}

	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		TArray<FString> meshname;
		TArray<FMaterialColorData> datalist;
		subModelComponentArr[i]->privateColorData.GenerateKeyArray(meshname);
		subModelComponentArr[i]->privateColorData.GenerateValueArray(datalist);
		for(int j= 0; j < meshname.Num(); ++j)
			ReplaceSurfaceByCategoryId(subModelComponentArr[i]->CategoryId, meshname[j], datalist[j].MD5, datalist[j].propertyValueId, datalist[j].propertyValueId, datalist[j].propertyId);
	}
}

void AHouseCustomActor::SetIndex(int32 id)
{
	CounterclockwiseIndex = id;
}

void AHouseCustomActor::CreateSkuMesh(FString MD5, FVector Pos, int32 id,float z, FTransform transform, bool bIs)
{
	UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();
	UModelFile *modelFile = Cast<UModelFile>(ResourceMgr->FindRes(MD5, true));
	FVector temp(0,0,0);
	
	
	if (modelFile)
	{
		FSkuData *curdata = Skuinfo.Find(id);
		auto SkuTmpe =  GetWorld()->SpawnActor<ASkuComponentActor>();
		auto actorloc = GetActorLocation();

		if (bIs)
		{
			
			
			Pos = UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), Pos);
		}
		if (curdata)
		{
			SkuTmpe->InitSkuData(*curdata);
			curdata->UUID = SkuTmpe->myUUID;
		}
	
		SkuTmpe->ParentActor = this;
		//if(bIs && Versions < 7)
		SkuTmpe->SetActorTransform(FTransform((GetActorRotation() + FRotator(0,180.f,0)), (FVector(actorloc.X, actorloc.Y , 0) +  Pos / 10.f), FVector(1, 1, 1)));
	
		
		
		if (!bIs)
		    SkuTmpe->SetActorLocation(FVector(Pos.X, Pos.Y, Pos.Z /10.f));
		
		if (!bIs && Versions > 6)
			SkuTmpe->SetActorTransform(transform);
	
		SkuComponent.Add(id, SkuTmpe);
	}
}

bool AHouseCustomActor::ChangSku(float z, int32 Key, FSkuData data, ADynamicTableboard*& table)
{
	bool bIs = true;
	auto temp = SkuComponent.Find(Key);
	auto curdata = Skuinfo.Find(Key);
	data.UUID = curdata->UUID;
	if (!curdata)
	{
		TArray<FSkuData> Templist;
		Skuinfo.GenerateValueArray(Templist);
		for (auto i : Templist)
		{
			if (i.Categoryid == Key)
			{
				curdata = &i;
				break;
			}
		}
	}
	

	if ((Key == 722 || Key == 727) && temp)
	{
		if (curdata)
			data.IsFiltration = curdata->IsFiltration;
		table = (*temp)->ChangeSku(data);
		Skuinfo.Add(Key, data);
		return bIs;
	}

	if ((Versions == -1 || Versions > 2) && !temp)
	{
		temp = SkuComponent.Find(data.Categoryid);
		if (curdata)
			data.IsFiltration = curdata->IsFiltration;
		if (temp)
		{
			FSkuData Tm = data;
			//if (Tm.Categoryid == 726)
		//	{
				SkuComponent.Remove(data.Categoryid);
				FVector tmepvec = (*temp)->GetActorLocation();
				float Lasttempz = (*temp)->GetWashbowlZ() + (*temp)->GetActorLocation().Z;
				table = (*temp)->ChangeSku(Tm);
				float tempz = (*temp)->GetWashbowlZ(Tm.MD5, bIs) + (*temp)->GetActorLocation().Z;
				if (!bIs)
				{
					(*temp)->ChangeSku(Tm);
					Skuinfo.Add(Key, Tm);
					return bIs;
				}
				z = Lasttempz - tempz;
		//	}
		//	else
		//		(*temp)->ChangeSku(Tm.MD5);

			(*temp)->AddActorWorldOffset(FVector(0, 0, z));
			
			Skuinfo.Add(Key, Tm);
		}
	}
	else if (temp)
	{
		if (curdata)
			data.IsFiltration = curdata->IsFiltration;

		FSkuData Tm = data;	
		//if (Tm.Categoryid == 726)
		//{
			FVector tmepvec = (*temp)->GetActorLocation();
			float tempz = (*temp)->GetWashbowlZ(Tm.MD5, bIs);
			if (!bIs)
			{
				table = (*temp)->ChangeSku(Tm);
				Skuinfo.Add(Key, Tm);
				return bIs;
			}

			float Lasttempz = 0.f;
			if(curdata)
				Lasttempz  = (*temp)->GetWashbowlZ(curdata->MD5, bIs);
			table = (*temp)->ChangeSku(Tm);

			z = Lasttempz - tempz;
		//}
	//	else
	//		(*temp)->ChangeSku(Tm);

		(*temp)->AddActorWorldOffset(FVector(0, 0, z));

		Skuinfo.Add(Key, Tm);
	}
	return bIs;
}

FBoxSphereBounds AHouseCustomActor::GetMeshContent(const FString& CategoryName, const FString& MeshName)
{
	FBoxSphereBounds TempBox;
	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(CategoryName) > -1)
		{
			IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
			const char *AnsiResID = TCHAR_TO_ANSI(*i->fileNum);
			IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false);
			if (!pObj)
				return TempBox;
			IGeometry* pGeometry = SDK->GetGeometryLibrary();
			const char *meshname = TCHAR_TO_ANSI(*MeshName);
			IMeshObject* pMeshObj = pObj->GetMeshObject(meshname);
			if (!pMeshObj)
				return TempBox;
			kBox3D box = pMeshObj->GetBounds();
			float Z = box.MaxEdge.z * 10 + GetActorLocation().Z;
			auto center = box.GetCenter();
			TempBox.Origin.X = center.x;
			TempBox.Origin.Z = center.z;
			TempBox.Origin.Y = center.y;
			auto extent = box.GetExtent();
			TempBox.BoxExtent.X = extent.x;
			TempBox.BoxExtent.Y = extent.y;
			TempBox.BoxExtent.Z = extent.z;

			break;
		}
	}
	return TempBox;
}


void AHouseCustomActor::SetSkudata(TMap<int32, FSkuData> skudata)
{
	Skuinfo = skudata;
}


void AHouseCustomActor::SetPrimitiveJson(int32 Categoryid, FString Md5)
{
	auto ComponentListJson = PrimitiveTempleteJson->GetObjectArrayField("componentList");
	for (int i = 0; i < ComponentListJson.Num(); i++)
	{

		if (subModelComponentArr[i]->CategoryId == Categoryid)
			subModelComponentArr[i]->PrmitivefileNum = Md5;

	}

}

bool AHouseCustomActor::DisposeInternals(int type, FString Res, UhouseCustomTemplete*Internal, FVector maxsize, FVector minsize)
{
	auto Parentcomponent = Internal->parentHouseCustomTemplete;
	//int32 objid;
	//FString Resid;
	FBox InnerCapacityBox, OuterCapacityBox;
	GetBox(Parentcomponent, InnerCapacityBox, OuterCapacityBox);
	//compoundElementInterface->GetElementInfo(Parentcomponent->ElementID, objid, Resid, InnerCapacityBox, OuterCapacityBox, MeshBounds);
	FVector Loation;
	FRotator rotator;
	compoundElementInterface->GetElementLocationAndRotation(Internal->ElementID, Loation, rotator);
	if (Internal->Currentsize.IsZero())
	{
		FVector Scale = compoundElementInterface->GetElementScale(Internal->ElementID);
		int x, y, z;
		compoundElementInterface->GetElementSize(Internal->ElementID, y, z, x);
		Internal->Currentsize.Set(x, y, z);
		Internal->Currentsize = Internal->Currentsize * Scale;

	}
		//auto cutsize = Internal->Currentsize * GetActorScale3D();
	//if (cutsize.X > maxsize.X || cutsize.X < minsize.X)
	//{
	//	return false;
	//}

	compoundElementInterface->SetElementModel(Internal->ElementID, Res);
	
	//GetBox(Internal, InternalInnerCapacityBox, InternalOuterCapacityBox);
	FVector InternalSize;
	GetSize(Internal, InternalSize);

	//int x, y, z;
	//compoundElementInterface->GetElementSize(Internal->ElementID, y, z, x);
	//FVector ex(Internal->Currentsize.X, Internal->Currentsize.Y, z / 20.f);
	FVector ex = InternalSize / 20.f;
	FVector BosInnerExtent = InnerCapacityBox.GetExtent();
	FVector BosOuterExtent = OuterCapacityBox.GetExtent();
	//得到内包围盒最底部Z值
	//float tempt = InnerCapacityBox.GetCenter().Z - GetActorLocation().Z;
	float tempZ = InnerCapacityBox.GetCenter().Z - GetActorLocation().Z - BosInnerExtent.Z;

	//compoundElementInterface->GetElementInfo(Internal->ElementID, objid, Resid, InnerCapacityBox, OuterCapacityBox, MeshBounds);
	//FVector ex = InnerCapacityBox.GetExtent();
	
	//1下 2中 3上
	//偏移处理
	switch (type)
	{
	case 1: Loation.Z = tempZ; break;
	case 2: Loation.Z = tempZ + (tempZ + BosInnerExtent.Z) -(tempZ + ex.Z) ; break;
	case 3: Loation.Z = tempZ + (tempZ + BosInnerExtent.Z * 2.f) - (tempZ + ex.Z * 2.f);  break;
	}
	
	FVector Tsize = InternalSize;
	//Tsize.Set(x, y, z);
	//Tsize = Internal->Currentsize;
	FVector CurerntScale = Internal->Currentsize / Tsize;
	ex.Z *= GetActorScale().Z;
	if (BosInnerExtent.Z < ex.Z)
		CurerntScale.Z = BosInnerExtent.Z  / ex.Z;
		//CurerntScale.Z = (BosInnerExtent.Z - (ex.Z - BosInnerExtent.Z)) / BosInnerExtent.Z;
	else
		CurerntScale.Z = 1.f;

	Internal->associatedAnchor.InternalsType = type;

	CurerntScale.Y = 1.f;

	compoundElementInterface->SetElementTransform(Internal->ElementID, rotator, Loation, CurerntScale);
	return true;
}

void AHouseCustomActor::RevertLaShou()
{
	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(TEXT("拉手")) > -1)
		{
			
			UBuildingGroupMeshComponent * Temp = compoundElementInterface->FindComponentByElementID(i->ElementID);
			if (Temp)
			{
				//auto Sizestring = i->ComponentJson->GetObjectField("customJson")->GetStringField("componentSize");
				//if (!Sizestring.IsEmpty())
				//{
					FVector Defalutszie,CurrentSize;
					//bool bIs;
					//UKismetStringLibrary::Conv_StringToVector(Sizestring, Defalutszie, bIs);
					int x, y, z;
					compoundElementInterface->GetElementSize(i->ElementID,y,z,x);
					Defalutszie.Set(x, y, z);
					FVector sclae = compoundElementInterface->GetElementScale(i->ElementID);
					
					Defalutszie = Defalutszie / sclae;
					
					CurrentSize = Temp->Bounds.BoxExtent* 20.f;//GetBox().GetExtent();
					CurrentSize = UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), CurrentSize);


					auto tempscale = Defalutszie / CurrentSize;
					FVector Pos;
					FRotator Rot;
					compoundElementInterface->GetElementLocationAndRotation(i->ElementID, Pos, Rot);
					compoundElementInterface->SetElementTransform(i->ElementID,  Rot, Pos, tempscale);
					//Temp->SetWorldScale3D(tempscale);
				//}
			}
		}
	}
}

void AHouseCustomActor::UpdataInternals(UhouseCustomTemplete*Internal,bool bIs)
{
	if (Internal->associatedAnchor.InternalsType < 1 || Internal->associatedAnchor.InternalsType > 3)
		return;

	auto Parentcomponent = Internal->parentHouseCustomTemplete;

	FBox InnerCapacityBox, OuterCapacityBox;
	GetBox(Parentcomponent, InnerCapacityBox, OuterCapacityBox);

	
	FVector Loation;
	FRotator rotator;
	compoundElementInterface->GetElementLocationAndRotation(Internal->ElementID, Loation, rotator);
	
	FVector Scale = compoundElementInterface->GetElementScale(Internal->ElementID);

	FVector InternalSize;
	GetSize(Internal, InternalSize);

	FVector ex = InternalSize / 20.f;
	FVector BosInnerExtent = InnerCapacityBox.GetExtent();
	FVector BosOuterExtent = OuterCapacityBox.GetExtent();
	//得到内包围盒最底部Z值
	//float tempt = InnerCapacityBox.GetCenter().Z - GetActorLocation().Z;
	float tempZ = InnerCapacityBox.GetCenter().Z - GetActorLocation().Z - BosInnerExtent.Z;
	
	//1下 2中 3上
	//偏移处理
	switch (Internal->associatedAnchor.InternalsType)
	{
	case 1: Loation.Z = tempZ; break;
	case 2: Loation.Z = tempZ + (tempZ + BosInnerExtent.Z) - (tempZ + ex.Z); break;
	case 3: Loation.Z = tempZ + (tempZ + BosInnerExtent.Z * 2.f) - (tempZ + ex.Z * 2.f);  break;
	}

	FVector CurerntScale = Scale;

	if (bIs)
	{
		if (BosInnerExtent.Z < ex.Z)
			CurerntScale.Z = (BosInnerExtent.Z - (ex.Z - BosInnerExtent.Z)) / BosInnerExtent.Z;
		else
			CurerntScale.Z = 1.f;
	}

	compoundElementInterface->SetElementTransform(Internal->ElementID, rotator, Loation, CurerntScale);
}

void AHouseCustomActor::SetStyleTemplate(UVaRestJsonObject* StyleJson)
{
	auto Componetlistjson = StyleJson->GetObjectArrayField("componentList");

	for (auto i : Componetlistjson)
	{
		for (auto Comp : subModelComponentArr)
		{
			if (i->GetStringField("categoryName").Find(Comp->CategoryName) > -1)
			{
				//物料替换
				Comp->fileNum = i->GetStringField("mxFileMD5");
				Comp->materialId = i->GetNumberField("materialId");
				Comp->ChargeUnit = i->GetNumberField("chargeUnit");
				Comp->price = i->GetNumberField("price");
				Comp->modelId = i->GetNumberField("modelId");

				FVector Loation, Scale;
				FRotator rotator;
				compoundElementInterface->GetElementLocationAndRotation(Comp->ElementID, Loation, rotator);
				Scale = compoundElementInterface->GetElementScale(Comp->ElementID);
				if (Comp->Currentsize.IsZero())
				{
					int x, y, z;
					compoundElementInterface->GetElementSize(Comp->ElementID, y, z, x);
					Comp->Currentsize.Set(x, y, z);
					Comp->Currentsize = Comp->Currentsize * Scale;
				}

				compoundElementInterface->SetElementModel(Comp->ElementID, Comp->fileNum);

				if (Comp->CategoryName.Find(TEXT("拉手")) > -1)
				{
					SetHandleOffsetex(Comp);
					continue;
				}

				int x, y, z;
				compoundElementInterface->GetElementSize(Comp->ElementID, y, z, x);

				FVector Tsize;
				Tsize.Set(x, y, z);
				FVector CurerntScale = Comp->Currentsize / Tsize;
				compoundElementInterface->SetElementTransform(Comp->ElementID, rotator, Loation, CurerntScale);

				//颜色替换
				Comp->CurrentColorData.Empty();
				auto attrjson = i->GetObjectField("attrJson");

				TArray<FString> name;
				TArray<UVaRestJsonObject*> colorjsonlist;
				if (attrjson->GetObjectField("doorFrameColor"))
				{
					name.Add(FString("mesh_door1"));
					colorjsonlist.Add(attrjson->GetObjectField("doorFrameColor"));
				}
				else if (attrjson->GetObjectField("coreColor"))
				{
					name.Add(FString("mesh_door2"));
					colorjsonlist.Add(attrjson->GetObjectField("coreColor"));
				}
				else
				{
					name.Add(FString("mesh_color1"));
					colorjsonlist.Add(attrjson->GetObjectField("color"));
				}

				for (int j = 0; j < colorjsonlist.Num(); ++j)
				{

					FMaterialColorData dcolor;
					auto pripertyjson = colorjsonlist[j]->GetObjectField("propertyValue");
					dcolor.propertyId = colorjsonlist[j]->GetNumberField("propertyId");
					dcolor.propertyValueId = pripertyjson->GetNumberField("propertyValueId");
					dcolor.propertyValueCode = pripertyjson->GetNumberField("propertyValueCode");
					dcolor.MD5 = pripertyjson->GetStringField("mxFileMD5");
					Comp->CurrentColorData.Add(name[j], dcolor);
					Comp->ColorMD5.Add(name[j], dcolor.MD5);
					compoundElementInterface->SetElementSurfaceByMeshName(Comp->ElementID, name[j], dcolor.MD5);
				}
				Comp->IsChangeColor = true;
			}
		}
	}
}


bool AHouseCustomActor::GlobalReplacement(const FString& CategoryName, const FString& ResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size, int32 modelid, int32 HandleOffset, int32 handleDirection, bool bIsGlobal, int32 InterType, FVector maxsize, FVector minsize)
{
	if (!ResID.IsEmpty())
	{
		UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();

		UModelFile *modelFile = Cast<UModelFile>(ResourceMgr->FindRes(ResID, true));
		if (modelFile)
		{
			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (UKismetStringLibrary::FindSubstring(subModelComponentArr[i]->CategoryName, CategoryName) > -1)
				{
					//if (subModelComponentArr[i]->fileNum == ResID)
					///	return true;

				

					subModelComponentArr[i]->fileNum = ResID;
					subModelComponentArr[i]->materialId = MaterialID;
					subModelComponentArr[i]->ChargeUnit = chargeUnit;
					subModelComponentArr[i]->price = price;
					//subModelComponentArr[i]->Defaultsize = size;
					subModelComponentArr[i]->modelId = modelid;
					
				
					if (subModelComponentArr[i]->CategoryName.Find(TEXT("内构")) > -1)
					{
						bool bIsTemp = DisposeInternals(InterType, ResID, subModelComponentArr[i], maxsize, minsize);
						//if (bIsTemp)
						//{
						//	subModelComponentArr[i]->fileNum = ResID;
						//	subModelComponentArr[i]->materialId = MaterialID;
						//	subModelComponentArr[i]->ChargeUnit = chargeUnit;
						//	subModelComponentArr[i]->price = price;
						//	//subModelComponentArr[i]->Defaultsize = size;
						//	subModelComponentArr[i]->modelId = modelid;
						//}

						return true;
						//return;
					}

					//compoundElementInterface->SetElementModel(subModelComponentArr[i]->ElementID, ResID, true);

						//////////////////////////////////////////////////////////////////
					FVector Loation, Scale;
					FRotator rotator;
					compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[i]->ElementID, Loation, rotator);
					Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
					if (subModelComponentArr[i]->Currentsize.IsZero())
					{
						int x, y, z;
						compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);
						subModelComponentArr[i]->Currentsize.Set(x, y, z);
						subModelComponentArr[i]->Currentsize = subModelComponentArr[i]->Currentsize * Scale;
					}


					compoundElementInterface->SetElementModel(subModelComponentArr[i]->ElementID, ResID);

					if (subModelComponentArr[i]->CategoryName.Find(TEXT("拉手")) > -1 )
					{
						if (!bIsGlobal)
						{
							subModelComponentArr[i]->associatedAnchor.doorHandleOffset = HandleOffset;
							subModelComponentArr[i]->associatedAnchor.doorhandleDirection = handleDirection;
							
						}
						SetHandleOffset(i);
						continue;
					}

					int x, y, z;
					compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);
					

					FVector Tsize;
					Tsize.Set(x, y, z);

					FVector CurerntScale;
					if (TemplateCategoryId == 200036)
						CurerntScale = subModelComponentArr[i]->associatedAnchor.componentSize / Tsize;
					else
						CurerntScale = (subModelComponentArr[i]->Currentsize) / Tsize;

					CurerntScale = UHouseFunctionLibrary::GetRotationPos(rotator, CurerntScale);
					UHouseFunctionLibrary::ABSVector(CurerntScale);
					compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, CurerntScale);
				
				}
			}


			//重新覆盖材质
		/*	for (int i = 0; i < subModelComponentArr.Num(); i++)
			{

				if (UKismetStringLibrary::FindSubstring(subModelComponentArr[i]->CategoryName, CategoryName) > -1)
					continue;

				TArray<FString> MD5s, Keys;
				subModelComponentArr[i]->ColorMD5.GenerateValueArray(MD5s);
				subModelComponentArr[i]->ColorMD5.GenerateKeyArray(Keys);
				for (int j = 0; j < MD5s.Num(); ++j)
					compoundElementInterface->SetElementSurfaceByMeshName(subModelComponentArr[i]->ElementID, Keys[j], MD5s[j]);
			}*/
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(" Res does not exist:%s"), *ResID);
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReplaceModelByRes ResID Can not be empty"));
	}
	return  true;
}

void AHouseCustomActor::UpdataHandleOffset()
{
	for (int i = 0; i < subModelComponentArr.Num(); i++)
	{
		if (subModelComponentArr[i]->CategoryName.Find(TEXT("拉手")) > -1)
		{
			if (subModelComponentArr[i]->associatedAnchor.doorHandleOffset == 0)
				break;

			float dcretor = subModelComponentArr[i]->associatedAnchor.doorhandleDirection ? 90.f : 0.f;
			FVector Loation;
			FRotator rotator;
			FVector Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
			compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[i]->ElementID, Loation, rotator);
			//FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[i]);
			rotator.Pitch = dcretor;
			compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, Scale);
			FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[i]);
			Loation.X = offsetpos.X;
			Loation.Z = offsetpos.Y;
			compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, Scale);
		
		}
		else if (subModelComponentArr[i]->CategoryName.Find(TEXT("内构")) > -1)
			UpdataInternals(subModelComponentArr[i], false);
	}

}

void AHouseCustomActor::SetHandleOffset(int index)
{
	if (subModelComponentArr[index]->associatedAnchor.doorHandleOffset == 0)
		return;

	float dcretor = subModelComponentArr[index]->associatedAnchor.doorhandleDirection ? 90.f : 0.f;
	FVector Loation;
	FRotator rotator;
	FVector Scale = compoundElementInterface->GetElementScale(subModelComponentArr[index]->ElementID);
	compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[index]->ElementID, Loation, rotator);
	//FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[i]);
	rotator.Pitch = dcretor;
	compoundElementInterface->SetElementTransform(subModelComponentArr[index]->ElementID, rotator, Loation, Scale);
	FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[index]);
	Loation.X = offsetpos.X;
	Loation.Z = offsetpos.Y;
	compoundElementInterface->SetElementTransform(subModelComponentArr[index]->ElementID, rotator, Loation, Scale);
}

void AHouseCustomActor::SetHandleOffsetex(UhouseCustomTemplete* component)
{
	if (component->associatedAnchor.doorHandleOffset == 0)
		return;

	float dcretor = component->associatedAnchor.doorhandleDirection ? 90.f : 0.f;
	FVector Loation;
	FRotator rotator;
	FVector Scale = compoundElementInterface->GetElementScale(component->ElementID);
	compoundElementInterface->GetElementLocationAndRotation(component->ElementID, Loation, rotator);
	//FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[i]);
	rotator.Pitch = dcretor;
	compoundElementInterface->SetElementTransform(component->ElementID, rotator, Loation, Scale);
	FVector2D offsetpos = GetHandleOffsetPos(component);
	Loation.X = offsetpos.X;
	Loation.Z = offsetpos.Y;
	compoundElementInterface->SetElementTransform(component->ElementID, rotator, Loation, Scale);
}

void AHouseCustomActor::SetHandleOffsetIndex(UhouseCustomTemplete* component, int index)
{
	if (index < 1 && index > 9)
		return;

	component->associatedAnchor.doorHandleOffset = index;
	
	float dcretor = component->associatedAnchor.doorhandleDirection ? 90.f : 0.f;
	FVector Loation;
	FRotator rotator;
	FVector Scale = compoundElementInterface->GetElementScale(component->ElementID);
	compoundElementInterface->GetElementLocationAndRotation(component->ElementID, Loation, rotator);
	//FVector2D offsetpos = GetHandleOffsetPos(subModelComponentArr[i]);
	rotator.Pitch = dcretor;
	compoundElementInterface->SetElementTransform(component->ElementID, rotator, Loation, Scale);
	FVector2D offsetpos = GetHandleOffsetPos(component);
	Loation.X = offsetpos.X;
	Loation.Z = offsetpos.Y;
	compoundElementInterface->SetElementTransform(component->ElementID, rotator, Loation, Scale);
}

void AHouseCustomActor::SetDoorHandleVisible(bool bIs)
{
	for (auto i : subModelComponentArr)
	{
		if(i->CategoryName.Find(TEXT("拉手")) > -1 || i->CategoryName.Find(TEXT("门板")) > -1)
			compoundElementInterface->SetElementVisibleByMeshName(i->ElementID,"",bIs);
	}
}

FVector AHouseCustomActor::GetSize(int32 ID)
{
	//int32 Objid;
	//FString MD5;
	//FBox wbox, lbox, buox;
	//compoundElementInterface->GetElementInfo(ID, Objid, MD5, wbox, lbox, buox);
		//return lbox.GetSize();
	int32 Width;
	int32 Height;
	int32 Depth;
	FVector scale=this->GetActorScale3D();
	compoundElementInterface->GetElementSize(ID, Width,  Height, Depth, true);
	return FVector(Width*scale.X, Depth*scale.Y, Height*scale.Z);

}

bool AHouseCustomActor::ReplaceSurfaceByCategoryName(FString CategoryName, const TArray<FString>&MeshName, const TArray<FString>& ResID, const TArray<int32>& ColorID, const TArray<int32>&ColorCode, const TArray<int32>&Propertyid)
{
	bool IsChange = false;
	if (ResID.Num() > 0)
	{
		UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();

		UModelFile *modelFile = Cast<UModelFile>(ResourceMgr->FindRes(ResID[0], true));
		if (modelFile)
		{
			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (subModelComponentArr[i]->CategoryName.Find(CategoryName) > -1)
				{

					subModelComponentArr[i]->CurrentColorData.Empty();
					IsChange = true;
					for (int j = 0; j < MeshName.Num(); ++j)
					{
						FString name;
						if (CategoryName.Find(TEXT("门板")) > -1)
						{
							if (MeshName[j] == FString("doorFrameColor"))
								name = FString("mesh_door1");
							else if (MeshName[j] == FString("coreColor"))
								name = FString("mesh_door2");
							else
								name = FString("mesh_color1");
						}
						else
							name = "";
						FMaterialColorData dcolor;
						dcolor.propertyId = Propertyid[j];
						dcolor.propertyValueId = ColorID[j];
						dcolor.propertyValueCode = ColorCode[j];
						dcolor.MD5 = ResID[j];
						subModelComponentArr[i]->CurrentColorData.Add(name, dcolor);
						subModelComponentArr[i]->ColorMD5.Add(name, ResID[j]);
						compoundElementInterface->SetElementSurfaceByMeshName(subModelComponentArr[i]->ElementID, name, ResID[j]);
					}
					subModelComponentArr[i]->IsChangeColor = true;
				
				}
			}


			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (subModelComponentArr[i]->CategoryName.Find(TEXT("门板")) > -1)
				{
					auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					if (Mesh)
						Mesh->SetVisibility(subModelComponentArr[i]->MeshVisieble);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Surface Res does not exist:%s"), *ResID[0]);
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReplaceSurfaceByRes ResID Can not be empty"));
	}
	return IsChange;
	
}
void AHouseCustomActor::ReplaceModelByCategoryId(int32 CategoryId, const FString& ResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size, int32 modelid)
{
	if (!ResID.IsEmpty())
	{
		UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();

		UModelFile *modelFile = Cast<UModelFile>(ResourceMgr->FindRes(ResID, true));
		if (modelFile)
		{
			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if ((subModelComponentArr[i]->CategoryName.Find(TEXT("拉手")) == -1) && subModelComponentArr[i]->CategoryId == CategoryId)
				{


					subModelComponentArr[i]->fileNum = ResID;
					subModelComponentArr[i]->materialId = MaterialID;
					subModelComponentArr[i]->ChargeUnit = chargeUnit;
					subModelComponentArr[i]->price = price;
					//subModelComponentArr[i]->Defaultsize = size;
					subModelComponentArr[i]->modelId = modelid;

					auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					

				/*	compoundElementInterface->SetElementModel(subModelComponentArr[i]->ElementID, ResID, true);

					FVector Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
					int x, y, z;
					compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);
					subModelComponentArr[i]->Defaultsize = FVector(x, y, z) * Scale;*/


					////////////////////////////////////////////////////////
					FVector Loation, Scale;
					FRotator rotator;
					compoundElementInterface->GetElementLocationAndRotation(subModelComponentArr[i]->ElementID, Loation, rotator);
					Scale = compoundElementInterface->GetElementScale(subModelComponentArr[i]->ElementID);
					int x1, y1, z1;
					compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y1, z1, x1);					
					//auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					//auto size1 = Mesh->Bounds;


					//subModelComponentArr[i]->Defaultsize.Set(x1, y1, z1);
					///////////////////////////

					FVector LastSize = Scale * FVector(x1,y1,z1);//= (FVector(1.f,1.f,1.f) - Scale) * FVector(x1, y1, z1) + FVector(x1, y1, z1);
					compoundElementInterface->SetElementModel(subModelComponentArr[i]->ElementID, ResID);


					//Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					//size1 = Mesh->Bounds;
					//Mesh->UpdateBounds();
					//size1 = Mesh->Bounds;

					int x, y, z;
					compoundElementInterface->GetElementSize(subModelComponentArr[i]->ElementID, y, z, x);
					subModelComponentArr[i]->Defaultsize.Set(x, y, z);

					FVector CurerntScale =  LastSize / FVector(x, y, z);
					compoundElementInterface->SetElementTransform(subModelComponentArr[i]->ElementID, rotator, Loation, CurerntScale);
					subModelComponentArr[i]->ScaleInfo.Add(subModelComponentArr[i]->guid, CurerntScale);

					subModelComponentArr[i]->Defaultsize = FVector(x, y, z) * CurerntScale;

				}
			}

			//重新覆盖材质
			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{

				if(subModelComponentArr[i]->CategoryId == CategoryId)
					continue;

				TArray<FString> MD5s,Keys;
				subModelComponentArr[i]->ColorMD5.GenerateValueArray(MD5s);
				subModelComponentArr[i]->ColorMD5.GenerateKeyArray(Keys);
				for (int j = 0; j < MD5s.Num(); ++j)
					compoundElementInterface->SetElementSurfaceByMeshName(subModelComponentArr[i]->ElementID, Keys[j], MD5s[j]);
			}

			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (subModelComponentArr[i]->CategoryName.Find(TEXT("门板")) > -1)
				{
					auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					if (Mesh)		
						Mesh->SetVisibility(subModelComponentArr[i]->MeshVisieble);
				}
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(" Res does not exist:%s"), *ResID);
		}
	
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReplaceModelByRes ResID Can not be empty"));
	}
	
}

void AHouseCustomActor::ReplaceSurfaceByCategoryId(int32 CategoryId, const FString&MeshName, const FString& ResID, int32 ColorID, int32 ColorCode, int32 Propertyid)
{
	if (!ResID.IsEmpty())
	{
		UResourceMgr* ResourceMgr = UResourceMgr::GetResourceMgr();

		UModelFile *modelFile = Cast<UModelFile>(ResourceMgr->FindRes(ResID, true));
		if (modelFile)
		{
			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (subModelComponentArr[i]->CategoryId == CategoryId)
				{
					auto temoclor = subModelComponentArr[i]->CurrentColorData.Find(MeshName);
					FMaterialColorData dcolor;
					if (temoclor)
						dcolor = *temoclor;
					dcolor.propertyId = Propertyid;
					dcolor.propertyValueId = ColorID;
					dcolor.propertyValueCode = ColorCode;
					dcolor.MD5 = ResID;
					subModelComponentArr[i]->CurrentColorData.Add(MeshName, dcolor);
					subModelComponentArr[i]->ColorMD5.Add(MeshName, ResID);
					compoundElementInterface->SetElementSurfaceByMeshName(subModelComponentArr[i]->ElementID, MeshName, ResID);
				}
			}


			for (int i = 0; i < subModelComponentArr.Num(); i++)
			{
				if (subModelComponentArr[i]->CategoryName.Find(TEXT("门板")) > -1)
				{
					auto Mesh = compoundElementInterface->FindComponentByElementID(subModelComponentArr[i]->ElementID);
					if (Mesh)
						Mesh->SetVisibility(subModelComponentArr[i]->MeshVisieble);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Surface Res does not exist:%s"), *ResID);
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReplaceSurfaceByRes ResID Can not be empty"));
	}
}

UVaRestJsonObject * AHouseCustomActor::getCalculatejson()
{
	UVaRestJsonObject* calculatejsonjson = UVaRestJsonObject::ConstructJsonObject(this);
	TArray<UVaRestJsonObject*>componentList;
	FString templateSize = TempleteJson->GetStringField("categoryName");
	FVector size;
	bool OutIsValid;

	UKismetStringLibrary::Conv_StringToVector(TempleteJson->GetStringField("templateSize"), size, OutIsValid);
	//fenzu
	TMap<int, TArray<UhouseCustomTemplete*>> componentsgroup;
			for (UhouseCustomTemplete *component : subModelComponentArr)
			{
				if ((component->CategoryName.Find(TEXT("台面")) != -1) && !loft.bIsTable) 
				{
					continue;
				}
				else if ((component->CategoryName.Find(TEXT("踢脚板")) != -1) && !loft.bIsSkirting) 
				{
					continue;
				}
				else if ((component->CategoryName.Find(TEXT("顶角线")) != -1) && !loft.bIsTopboard)
				{
					continue;
				}
				
				if (component->MeshVisieble) {
					if (component->ElementID > 0) {
						component->Defaultsize = GetSize(component->ElementID);
					}
					if (componentsgroup.Contains(component->materialId))
					{
						TArray<UhouseCustomTemplete*> temp = componentsgroup.FindRef(component->materialId);
						temp.Add(component);
						componentsgroup.Add(component->materialId, temp);
					}
					else
					{
						TArray<UhouseCustomTemplete*> tempn = TArray<UhouseCustomTemplete*>();
						tempn.Add(component);
						componentsgroup.Add(component->materialId, tempn);
					}
				}
				//componentList.Add(component->getquantitiesjson(this->GetActorScale3D(), size.X, size.Y, size.Z));
			}
			//suanliang
			for (auto& Elem : componentsgroup)
			{
				componentList.Add(CalculateMaterialByArray(Elem.Value, this->GetActorScale3D(), size));
			}
			calculatejsonjson->SetObjectArrayField("componentList", componentList);
			float categoryId = TemplateCategoryId;
			calculatejsonjson->SetNumberField("categoryId", categoryId);
			calculatejsonjson->SetStringField("categoryName", TempleteJson->GetStringField("categoryName"));
			calculatejsonjson->SetStringField("mainUrl", Url);
			calculatejsonjson->SetStringField("templateName", TempleteJson->GetStringField("templateName"));
			calculatejsonjson->SetStringField("guid", FString::FromInt(ActorId));
			calculatejsonjson->SetNumberField("templateId", (float)TemplateId);
			return calculatejsonjson;
}
UVaRestJsonObject * AHouseCustomActor::CalculateMaterialByArray(TArray<UhouseCustomTemplete*> componts, FVector scale, FVector size)
{
			UVaRestJsonObject* tempjson = UVaRestJsonObject::ConstructJsonObject(this);
			float quantity = 0.0f;
			float length = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
			float averagelength = 0.0f;
			float averagewidth = 0.0f;
			float averageheight = 0.0f;
			float countnum = (float)componts.Num();
			//int32 type = (int32)PrimitiveTempleteJson->GetNumberField("type");
			////转角柜L型
			//bool iszjgL = (type == 2);

			int templetcatgoryId= (int32)PrimitiveTempleteJson->GetNumberField("categoryId");
			bool iszjgL = (templetcatgoryId == 200036|| templetcatgoryId == 200090 || templetcatgoryId == 200086 || templetcatgoryId == 200098 || templetcatgoryId == 200094 || templetcatgoryId == 200075);
			for (UhouseCustomTemplete* tempcomp : componts)
			{
				length = 10.0f * tempcomp->Defaultsize.X;
				width = 10.0f * tempcomp->Defaultsize.Y;
				height = 10.0f * tempcomp->Defaultsize.Z;

				if (iszjgL&&tempcomp->CategoryName.Contains(TEXT("柜身")))
				{
					FVector modelsize1;
					FVector modelsize2;
					bool t = false;
					TArray<UVaRestJsonObject *>meshNameList = tempcomp->ComponentJson->GetObjectField("customJson")->GetObjectArrayField("meshNameList");
					for(UVaRestJsonObject *obj : meshNameList)
					{
						
						if (obj->GetStringField("meshName").Equals("mesh_color1")) {
							
							UKismetStringLibrary::Conv_StringToVector(obj->GetStringField("meshSize"), modelsize1, t);
						}
						if (obj->GetStringField("meshName").Equals("mesh_color2")) {
							UKismetStringLibrary::Conv_StringToVector(obj->GetStringField("meshSize"), modelsize2, t);
						}
					}
					
					
						quantity = ((modelsize1.X - modelsize2.X)*modelsize1.Z + modelsize2.Z*modelsize2.Y)*0.001f*0.001f;

					

				}
				else
				{
					if (tempcomp->CategoryName.Contains(TEXT("台面"))) {
						length = size.X * scale.X;
						width = size.Y * scale.Y;
						height = 15;
					}
					if (tempcomp->CategoryName.Contains(TEXT("踢脚板"))) {
						length = size.X * scale.X;
						width = 10;
						height = 100;

					}
					if (tempcomp->CategoryName.Contains(TEXT("顶角线"))) {

						length = size.X * scale.X;
						width = 10;
						height = 100;
					}


					switch (tempcomp->ChargeUnit)
					{
					case 0:
						quantity += length * 0.001f;
						break;
					case 1:
						quantity += (float)tempcomp->num;

						break;
					case 2:
						quantity += length * 0.001f;
						break;
					case 3:
						quantity += (length + width)*0.002f;
						break;
					case 4:
						quantity += length * height*0.001f*0.001f;
						break;
					case 5:
						quantity += width * length*0.001f*0.001f;
						break;
					default:
						quantity += length * 0.001f;
						break;
					}
				}
				 averagelength	+= length;
				 averagewidth	+= width;
				 averageheight	+= height;
			}

			averagelength /= countnum;
			averagewidth /= countnum;
			averageheight /= countnum;
			quantity = quantity / countnum;

			//jingdu 2 wei
			FBigDecimal bigquantity = ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithFloat(quantity);
			FString quantitystr = ULuoLuoBlueprintFunctionLibrary::ConverToStringWithPrecisionControl(bigquantity, 2, true);
			UVaRestJsonObject* attrJson = UVaRestJsonObject::ConstructJsonObject(this);
			bool hascolor = false;
			if (componts[0]->CurrentColorData.Contains("mesh_color1")|| componts[0]->CurrentColorData.Contains("")) {
				hascolor = true;
				FMaterialColorData *colorstruct = componts[0]->CurrentColorData.Find("mesh_color1");
				if (!colorstruct) {
					colorstruct = componts[0]->CurrentColorData.Find("");
				}
				UVaRestJsonObject* colorjson = UVaRestJsonObject::ConstructJsonObject(this);
				colorjson->SetNumberField("propertyId", colorstruct->propertyId);
				TArray<UVaRestJsonObject*>colorInArray;
				UVaRestJsonObject* colorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
				colorpropertyjson->SetNumberField("propertyValueCode", (float)colorstruct->propertyValueCode);
				colorInArray.Add(colorpropertyjson);
				colorjson->SetObjectArrayField("propertyValueList", colorInArray);
				attrJson->SetObjectField("color", colorjson);
			}
			if (componts[0]->CurrentColorData.Contains("mesh_door2")) {
				hascolor = true;
				FMaterialColorData *coreColorstruct = componts[0]->CurrentColorData.Find("mesh_door2");
				UVaRestJsonObject* coreColorjson = UVaRestJsonObject::ConstructJsonObject(this);
				coreColorjson->SetNumberField("propertyId", coreColorstruct->propertyId);
				TArray<UVaRestJsonObject*>coreColorjsonInArray;
				UVaRestJsonObject* coreColorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
				coreColorpropertyjson->SetNumberField("propertyValueCode", (float)coreColorstruct->propertyValueCode);
				coreColorjsonInArray.Add(coreColorpropertyjson);
				coreColorjson->SetObjectArrayField("propertyValueList", coreColorjsonInArray);
				attrJson->SetObjectField("coreColor", coreColorjson);
			}
			if (componts[0]->CurrentColorData.Contains("mesh_door1")) {
				hascolor = true;
				FMaterialColorData *doorFrameColorstruct = componts[0]->CurrentColorData.Find("mesh_door1");
				UVaRestJsonObject* doorFrameColorjson = UVaRestJsonObject::ConstructJsonObject(this);
				doorFrameColorjson->SetNumberField("propertyId", doorFrameColorstruct->propertyId);
				TArray<UVaRestJsonObject*>doorFrameColorInArray;
				UVaRestJsonObject* doorFrameColorpropertyjson = UVaRestJsonObject::ConstructJsonObject(this);
				doorFrameColorpropertyjson->SetNumberField("propertyValueCode", (float)doorFrameColorstruct->propertyValueCode);
				doorFrameColorInArray.Add(doorFrameColorpropertyjson);
				doorFrameColorjson->SetObjectArrayField("propertyValueList", doorFrameColorInArray);
				attrJson->SetObjectField("doorFrameColor", doorFrameColorjson);
			}

			if (hascolor) {
				tempjson->SetObjectField("attrJson", attrJson);
			}

			tempjson->SetNumberField("length", (float)(int)averagelength);
			tempjson->SetNumberField("width", (float)(int)averagewidth);
			tempjson->SetNumberField("height", (float)(int)averageheight);

			tempjson->SetNumberField("categoryId", (float)componts[0]->CategoryId);
			tempjson->SetNumberField("componentId", (float)componts[0]->ComponentID);
			tempjson->SetNumberField("modelId", (float)componts[0]->modelId);
			tempjson->SetNumberField("materialId", (float)componts[0]->materialId);
			tempjson->SetNumberField("num", countnum);
			tempjson->SetNumberField("price", (float)componts[0]->price);
			tempjson->SetStringField("quantities", quantitystr);
			return tempjson;
}

FVector AHouseCustomActor::GetMeshSize(const FString& Name)
{
	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(Name) > -1)
		{
			int32 x,y,z;
			compoundElementInterface->GetElementSize(i->ElementID, y, z, x);
			return FVector(x, y, z);
		}
	}
	return FVector();
}

FVector AHouseCustomActor::GetMeshSize(const FString& meshname, const FString& compoentname,FVector& Pos)
{
	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(meshname) > -1)
		{
			FVector size;
			compoundElementInterface->GetMeshSize(i->ElementID, size,compoentname);
			return size;
		}
	}
	return FVector();
}
void AHouseCustomActor::ChekUpdateSpigottl(const FString& Meshname, bool bWt, bool bIs)
{
	if (SkuComponent.Num() == 2)
	{
		float offset = 0.f;
		auto comp1 = SkuComponent.Find(726);
		auto comp2 = SkuComponent.Find(727);
		if (!comp2)
			comp2 = SkuComponent.Find(721);

		auto Actorloc = GetActorLocation();
		//auto Actorloc =  UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), GetActorLocation());
		if (comp1 && comp2)
		{
			FVector loc;
			FVector tempori, tempext;
			GetActorBounds(true, tempori, tempext);
			auto temp = FindCutomTemplate(TEXT("柜身"));
			//FBox InnerCapacityBox, OuterCapacityBox;
		//	GetBox(temp, InnerCapacityBox, OuterCapacityBox);
			auto Box = GetMeshContent(TEXT("柜身"), "mesh_color1");
			float Posz = 0;
			if (bWt)
				Posz = Box.BoxExtent.Z + (Box.Origin.Z - Box.BoxExtent.Z / 2.f);
			else
			{
				auto tp = FindCutomTemplate(TEXT("支撑脚"));


				if (!tp)
					Posz = tempext.Z* 2.f + 10.f;
				else
					Posz = tempext.Z* 2.f;
			}
			if (bIs)
			{

				float cabz = (*comp1)->GetWashbowlZ();
				FVector cabPos;
				if (TemplateCategoryId == 200102 || TemplateCategoryId == 200105 || TemplateCategoryId == 200104)
				{
					cabPos.X = Box.Origin.X;
					cabPos.Y = Box.Origin.Y;
				}
				else
				{
					cabPos.X = Box.BoxExtent.X / 2.f;
					cabPos.Y = Box.BoxExtent.Y / 2.f;
				}
				cabPos.Z = Posz - cabz + TAMHIGHT + offset;

				if (temp)
					cabPos *= GetActorScale3D();
				cabPos = UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), cabPos);
				cabPos = UHouseFunctionLibrary::ABSVector(cabPos);


				if (200105 == TemplateCategoryId)
					Actorloc = Actorloc + (-GetActorForwardVector() * cabPos);
				else
					Actorloc = Actorloc + (GetActorForwardVector() * cabPos);
				(*comp1)->SetActorLocation(FVector(Actorloc.X, Actorloc.Y, cabPos.Z));
			}
			auto skudate = Skuinfo.Find(726);
			if (!skudate)
				return;
			if ((*comp1)->GetMeshLocationEx(skudate->MD5, Meshname, loc))
			{
				//loc.X += Actorloc.X;
				//loc.Y += Actorloc.Y;
				loc.Z += offset;
				(*comp2)->SetActorLocation(loc);
				(*comp2)->SetMove(false);
			}
			else
			{

				FVector size, Pos1;
				if (PrimitiveTempleteJson->GetStringField("templateName").Find(TEXT("洗衣柜")) > -1)
					size = GetMeshSize(TEXT("柜身"), "mesh_color1", Pos1);
				else
					size = GetMeshSize(TEXT("柜身")) / 10.f;
				FVector ori, ext, Pos;
				(*comp1)->GetActorBounds(true, ori, ext);

				auto rotation = GetActorRotation().Quaternion();
				FTransform const Trabsform(rotation);
				FVector BoxExtent = Trabsform.TransformPosition(ext);
				BoxExtent.Set(FMath::Abs(BoxExtent.X), FMath::Abs(BoxExtent.Y), FMath::Abs(BoxExtent.Z));
				auto tempnor = Actorloc + (GetActorRightVector()*((size.Y / 2.f - BoxExtent.Y) / 2.f + BoxExtent.Y));
				auto tposs = GetActorForwardVector();
				if (ERRORBAND(FMath::Abs(tposs.X),1.f))
					tempnor.X = (*comp1)->GetActorLocation().X;
				else
					tempnor.Y = (*comp1)->GetActorLocation().Y;
				
			
				Pos.Z = Posz + TAMHIGHT + offset;
				
				(*comp2)->SetActorLocation(FVector(tempnor.X, tempnor.Y, Pos.Z));
				(*comp2)->SetMove(true);
			}
		}
	}
}

void AHouseCustomActor::ChekUpdateSpigot(const FString& Meshname, bool bWt, bool bIs)
{
	if (SkuComponent.Num() == 2)
	{

		if (LCategoryid == 200101)
		{
			ChekUpdateSpigottl(Meshname, bWt, bIs);
			return; 
		}


		float offset = 0.f;
		auto comp1 = SkuComponent.Find(726);
		auto comp2 = SkuComponent.Find(727);
		if(!comp2)
			comp2 = SkuComponent.Find(721);
		
		auto Actorloc = GetActorLocation();
		//auto Actorloc =  UHouseFunctionLibrary::GetRotationPos(GetActorRotation(), GetActorLocation());
		if (comp1 && comp2)
		{
			FVector loc;
			FVector tempori, tempext;
			GetActorBounds(true, tempori, tempext);
			auto temp = FindCutomTemplate(TEXT("柜身"));
			//FBox InnerCapacityBox, OuterCapacityBox;
		//	GetBox(temp, InnerCapacityBox, OuterCapacityBox);
			auto Box = GetMeshContent(TEXT("柜身"), "mesh_color1");
			float Posz = 0;
			if(bWt)
			   Posz = Box.BoxExtent.Z + (Box.Origin.Z - Box.BoxExtent.Z / 2.f);
			else
			{
				auto tp = FindCutomTemplate(TEXT("支撑脚"));

				
				if(!tp)
					Posz = tempext.Z* 2.f + 10.f;
				else
					Posz = tempext.Z* 2.f;
			}
			if (bIs)
			{
				
				float cabz = (*comp1)->GetWashbowlZ();
				FVector cabPos;
				cabPos.X = Box.Origin.X;
				cabPos.Y = Box.Origin.Y;
				cabPos.Z = Posz - cabz + TAMHIGHT + offset;
				if (temp)
					cabPos *= temp->associatedAnchor.CustomTramsform.GetScale3D();
				

				(*comp1)->SetActorLocation(FVector(Actorloc.X + cabPos.X, Actorloc.Y + cabPos.Y, cabPos.Z));
			}
			auto skudate = Skuinfo.Find(726);
			if (!skudate)
				return;
			if ((*comp1)->GetMeshLocationEx(skudate->MD5,Meshname, loc))
			{
				loc.X += Actorloc.X;
				loc.Y += Actorloc.Y;
				loc.Z += offset;
				(*comp2)->SetActorLocation(loc);
				(*comp2)->SetMove(false);
			}
			else
			{
				
				FVector size,Pos1;
				if (PrimitiveTempleteJson->GetStringField("templateName").Find(TEXT("洗衣柜")) > -1)
					size = GetMeshSize(TEXT("柜身"), "mesh_color1", Pos1);
				else
				size = GetMeshSize(TEXT("柜身"))/10.f;
				FVector ori, ext, Pos;
				(*comp1)->GetActorBounds(true, ori, ext);

				auto rotation = GetActorRotation().Quaternion();
				FTransform const Trabsform(rotation);
				FVector BoxExtent = Trabsform.TransformPosition(ext);
				BoxExtent.Set(FMath::Abs(BoxExtent.X), FMath::Abs(BoxExtent.Y), FMath::Abs(BoxExtent.Z));
				auto tempnor = Actorloc + (GetActorRightVector()*((size.Y / 2.f - BoxExtent.Y) / 2.f + BoxExtent.Y));
			
				Pos.X = tempnor.X ;
				Pos.Y = tempnor.Y;//(*comp1)->GetRootComponent()->GetRelativeTransform().GetLocation().X;
				Pos.Z = Posz + TAMHIGHT + offset;
				if (temp)
					Pos *= temp->associatedAnchor.CustomTramsform.GetScale3D();
				//Pos.Y -= Actorloc.Y;
				//Pos.X = Actorloc.X;
				(*comp2)->SetActorLocation(Pos);
				(*comp2)->SetMove(true);
			}
		}
	}
}

UhouseCustomTemplete* AHouseCustomActor::FindCutomTemplate(const FString& meshName)
{
	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(meshName) > -1)
			return i;
	}
	return nullptr;
}

void AHouseCustomActor::CreateHouseCustomActorByTemplateId(UObject* WorldContextObject, int32 ID, const FString& URL)
{
	UVaRestJsonObject *jsonObj = UVaRestJsonObject::ConstructJsonObject(WorldContextObject);
	if (jsonObj)
	{
		//CallBack = FVaRestCallDelegate::CreateUObject(this, &AHouseCustomActor::CreateHouseCustomActorCallback);
		CallBack.BindDynamic(this,&AHouseCustomActor::CreateHouseCustomActorCallback);
		jsonObj->SetNumberField("id", ID);
		UVaRestLibrary::CallURL(WorldContextObject,URL, ERequestVerb::GET, ERequestContentType::json, ETimeOutHandleType::DefaultHandle, jsonObj, CallBack, "");
	}
}

void AHouseCustomActor::SetLoft(bool bIs, int type)
{
	switch (type)
	{
	case 1: loft.bIsTable = bIs; break;
	case 2: loft.bIsSkirting = bIs; break;
	case 3: loft.bIsTopboard = bIs; break;
	}
}

void AHouseCustomActor::SetLoftex(FIsLoft date)
{
	loft = date;
}

void AHouseCustomActor::CreateHouseCustomActorCallback(UVaRestRequestJSON* Request)
{
	TempleteJson= Request->GetResponseObject();
	if (TempleteJson->GetBoolField("success"))
	{
		RestoreTemplateObject(TempleteJson);
		/*const TArray<UVaRestJsonObject*> componentListArr = TempleteJson->GetObjectArrayField("componentList");
		for (int i = 0; i < componentListArr.Num(); i++)
		{
			const UVaRestJsonObject* customJson = componentListArr[i]->GetObjectField("customJson");
			if (customJson->GetStringField("parentGuid").IsEmpty())
			{
				if (InitCompoundElementRootByResID(componentListArr[i]->GetStringField("fileNum")))
				{
					RestoreTemplateObject(TempleteJson);
				}
				break;
			}
		}		*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateHouseCustomActor Callback failed!"));
	}
}

void AHouseCustomActor::FinishTemplate()
{
	auto CompoentJsonlist = PrimitiveTempleteJson->GetObjectArrayField("componentList");

	for (auto i : CompoentJsonlist)
	{
		//UhouseCustomTemplete* TempCompoent = nullptr;
		auto TempCompoent = *ComponentList.Find(i->GetStringField("guid"));
		if (TempCompoent)
		{
			i->SetNumberField("modelId", TempCompoent->modelId);
			i->SetNumberField("materialId", TempCompoent->materialId);
			i->SetNumberField("categoryId", TempCompoent->CategoryId);
			i->SetNumberField("price", TempCompoent->price);
			i->SetNumberField("chargeUnit", TempCompoent->ChargeUnit);
			i->SetStringField("mxFileMD5", TempCompoent->fileNum);
			auto colortemp = i->GetObjectField("attrJson");
			GetFinishColorJson(TempCompoent, colortemp);
		}
	}
}

void AHouseCustomActor::GetFinishColorJson(UhouseCustomTemplete* component, UVaRestJsonObject* json)
{
	TMap<FString, UVaRestJsonObject*> ColorData;
	TArray<FString> colorname;
	TArray<FMaterialColorData> colorproperty;
	component->CurrentColorData.GenerateKeyArray(colorname);
	component->CurrentColorData.GenerateValueArray(colorproperty);
	for (int32 i = 0; i < colorname.Num(); ++i)
	{
		UVaRestJsonObject* colorprorpertyjson = UVaRestJsonObject::ConstructJsonObject(this);
		colorprorpertyjson->SetNumberField("propertyId", colorproperty[i].propertyId);

		UVaRestJsonObject* Tempcolorjson = UVaRestJsonObject::ConstructJsonObject(this);
		Tempcolorjson->SetNumberField("propertyValueId", colorproperty[i].propertyValueId);
		Tempcolorjson->SetNumberField("propertyValueCode", colorproperty[i].propertyValueCode);
		Tempcolorjson->SetStringField("mxFileMD5", colorproperty[i].MD5);
		Tempcolorjson->SetBoolField("selectFlag", true);
		TArray<UVaRestJsonObject*>colorlist;
		colorlist.Add(Tempcolorjson);
		colorprorpertyjson->SetObjectArrayField("propertyValueList", colorlist);
		json->SetObjectField(colorname[i], colorprorpertyjson);
	}
}

void AHouseCustomActor::RestoreTemplate()
{
	auto CompoentJsonlist = PrimitiveTempleteJson->GetObjectArrayField("componentList");

	for (auto i : CompoentJsonlist)
	{
	
		auto TempCompoent = *ComponentList.Find(i->GetStringField("guid"));

		if (TempCompoent->IsChangeColor)
		{
			TArray<FString> colorname;
			TempCompoent->CurrentColorData.GenerateKeyArray(colorname);
			TempCompoent->CurrentColorData.Empty();
			auto colorjson = i->GetObjectField("attrJson");

			UVaRestJsonObject* temp = colorjson->GetObjectField("mesh_door1");
			FMaterialColorData tempcolordata;
			if (temp)
			{
				tempcolordata.propertyId = temp->GetNumberField("propertyId");
				TArray<UVaRestJsonObject*> list = temp->GetObjectArrayField("propertyValueList");
				if (list.Num() > NULL)
				{
					tempcolordata.propertyValueId = list[0]->GetNumberField("propertyValueId");
					tempcolordata.propertyValueCode = list[0]->GetNumberField("propertyValueCode");
					tempcolordata.MD5 = list[0]->GetStringField("mxFileMD5");
				}
				TempCompoent->CurrentColorData.Add("mesh_door1", tempcolordata);
			}
			temp = colorjson->GetObjectField("mesh_door2");
			if (temp)
			{
				tempcolordata.propertyId = temp->GetNumberField("propertyId");
				TArray<UVaRestJsonObject*> list = temp->GetObjectArrayField("propertyValueList");
				if (list.Num() > NULL)
				{
					tempcolordata.propertyValueId = list[0]->GetNumberField("propertyValueId");
					tempcolordata.propertyValueCode = list[0]->GetNumberField("propertyValueCode");
					tempcolordata.MD5 = list[0]->GetStringField("mxFileMD5");
				}
				TempCompoent->CurrentColorData.Add("mesh_door2", tempcolordata);
			}

			temp = colorjson->GetObjectField("mesh_color1");
			if (temp)
			{
				tempcolordata.propertyId = temp->GetNumberField("propertyId");
				TArray<UVaRestJsonObject*> list = temp->GetObjectArrayField("propertyValueList");
				if (list.Num() > NULL)
				{
					tempcolordata.propertyValueId = list[0]->GetNumberField("propertyValueId");
					tempcolordata.propertyValueCode = list[0]->GetNumberField("propertyValueCode");
					tempcolordata.MD5 = list[0]->GetStringField("mxFileMD5");
				}

				TempCompoent->CurrentColorData.Add("mesh_color1", tempcolordata);
			}
		}

		if (TempCompoent)
		{
			if (TempCompoent->fileNum != i->GetStringField("mxFileMD5"))
			{
				TempCompoent->modelId = i->GetNumberField("modelId");
				TempCompoent->materialId = i->GetNumberField("materialId");
				TempCompoent->CategoryId = i->GetNumberField("categoryId");
				TempCompoent->price = i->GetNumberField("price");
				TempCompoent->ChargeUnit = i->GetNumberField("chargeUnit");
				TempCompoent->fileNum = i->GetStringField("mxFileMD5");

				if (TempCompoent->CategoryName.Find(TEXT("踢脚板")) > -1 || TempCompoent->CategoryName.Find(TEXT("顶角线")) > -1)
					continue;

				if (IsTable && TempCompoent->CategoryName.Find(TEXT("台面")) > -1)
						continue;

				FVector Loation, Scale;
				FRotator rotator;
				compoundElementInterface->GetElementLocationAndRotation(TempCompoent->ElementID, Loation, rotator);
				Scale = compoundElementInterface->GetElementScale(TempCompoent->ElementID);

				compoundElementInterface->SetElementModel(TempCompoent->ElementID, TempCompoent->fileNum);

				if (TempCompoent->CategoryName.Find(TEXT("拉手")) > -1)
				{
					SetHandleOffsetex(TempCompoent);
					continue;
				}

				int x, y, z;
				compoundElementInterface->GetElementSize(TempCompoent->ElementID, y, z, x);
				FVector Tsize;
				Tsize.Set(x, y, z);
				FVector CurerntScale = TempCompoent->Currentsize / Tsize;
			
				compoundElementInterface->SetElementTransform(TempCompoent->ElementID, rotator, Loation, CurerntScale);
			}
		}
	}

	for (auto i : subModelComponentArr)
	{
		if (i->CategoryName.Find(TEXT("踢脚板")) > -1 || i->CategoryName.Find(TEXT("顶角线")) > -1)
			continue;

		if (IsTable && i->CategoryName.Find(TEXT("台面")) > -1)
			continue;
		if (i->IsChangeColor)
		{
			i->IsChangeColor = false;
			TArray<FString> name;
			TArray<FMaterialColorData> Colordata;
			i->CurrentColorData.GenerateKeyArray(name);
			i->CurrentColorData.GenerateValueArray(Colordata);
			for (int j = 0; j < name.Num(); ++j)
				compoundElementInterface->SetElementSurfaceByMeshName(i->ElementID, name[j], Colordata[j].MD5);
		}
	}
}

FVector2D AHouseCustomActor::GetHandleOffsetPos(UhouseCustomTemplete* Mesh)
{

	FVector2D offsetPos(0.f,0.f);
	auto DoorComponent = compoundElementInterface->FindComponentByElementID(Mesh->parentHouseCustomTemplete->ElementID);
	auto HandleComponent = compoundElementInterface->FindComponentByElementID(Mesh->ElementID);
	//if (DoorComponent && HandleComponent)
	
		int32 Index = Mesh->associatedAnchor.doorHandleOffset;
		FVector2D data(0, 0);
		//Component->Bounds.Origin;
		
		FVector location;
		FRotator rotator;
		compoundElementInterface->GetElementLocationAndRotation(Mesh->parentHouseCustomTemplete->ElementID,location, rotator);
		//float telength = Mesh->parentHouseCustomTemplete->Currentsize.X > Mesh->parentHouseCustomTemplete->Currentsize.Y ? Mesh->parentHouseCustomTemplete->Currentsize.X : Mesh->parentHouseCustomTemplete->Currentsize.Y;
		//auto Mesh222 = compoundElementInterface->FindComponentByElementID(Mesh->parentHouseCustomTemplete->ElementID);
		//rotator = Mesh222->K2_GetComponentRotation();
		//auto qurotor = rotator.Quaternion();
		//FTransform Transform(qurotor);
		//auto Mesh111 = Transform.TransformPosition(Mesh->parentHouseCustomTemplete->Currentsize);
		//Mesh111.Set(FMath::Abs(Mesh111.X), FMath::Abs(Mesh111.Y), FMath::Abs(Mesh111.Z));

	//	FVector2D DoorSize(Mesh111.X / 20.f, Mesh111.Z / 20.f);
		//auto Tempscaledoor = compoundElementInterface->GetElementScale(Mesh->parentHouseCustomTemplete->ElementID) - 1.f;
		//

		auto tempscale = GetActorScale();
		//auto Sizestring = Mesh->parentHouseCustomTemplete->ComponentJson->GetObjectField("customJson")->GetStringField("componentSize");
		//if (Sizestring.IsEmpty())
		//	return FVector2D();

		//FVector Defalutszie, CurrentSize;
		//bool bIs;
		//UKismetStringLibrary::Conv_StringToVector(Sizestring, Defalutszie, bIs);
		////auto tempscale = GetActorScale();

		//FVector2D DoorSize(Defalutszie.X * (tempscale.X + Tempscaledoor.X) / 20.f, Defalutszie.Z  * (tempscale.Z + Tempscaledoor.Z) / 20.f);
		FVector2D DoorSize(Mesh->parentHouseCustomTemplete->Currentsize.X / 20.f , Mesh->parentHouseCustomTemplete->Currentsize.Z / 20.f);

		/*auto Mesh111 = compoundElementInterface->FindComponentByElementID(Mesh->parentHouseCustomTemplete->ElementID);
		auto size1 = Mesh111->Bounds.Origin;
		FVector2D DoorOri(size1.X, size1.Z);*/
		FVector2D DoorOri(location.X, location.Z + DoorSize.Y);
	  
		compoundElementInterface->GetElementLocationAndRotation(Mesh->ElementID, location, rotator);

		//telength = HandleComponent->Bounds.BoxExtent.X > HandleComponent->Bounds.BoxExtent.Y ? HandleComponent->Bounds.BoxExtent.X : HandleComponent->Bounds.BoxExtent.Y;
		//Mesh111 = Transform.TransformPosition(HandleComponent->Bounds.BoxExtent);

		int32 x, y, z;
		compoundElementInterface->GetElementSize(Mesh->ElementID, y, z, x);

		FVector2D HandleSize(x / 20.f, z / 20.f);

		//Mesh111 = compoundElementInterface->FindComponentByElementID(Mesh->ElementID);
		//size1 = Mesh111->Bounds.Origin;
		//FVector2D HandleOri(size1.X, size1.Z);
		FVector2D HandleOri(location.X, location.Z + HandleSize.Y);
		switch (Index)
		{
		case 1:data.Set(1, 1); break;//左上
		case 2:data.Set(0, 1); break;//上
		case 3:data.Set(-1, 1); break;//右上
		case 4:data.Set(1, 0); break;//左中
		case 5:data.Set(0, 0); break;//中
		case 6:data.Set(-1, 0); break;//右中
		case 7:data.Set(1, -1); break;//左下
		case 8:data.Set(0, -1); break;//下
		case 9:data.Set(-1,-1); break;//右下
		}

		FVector2D Mindoorpos = DoorOri - DoorSize;
		FVector2D Maxdoorpos = DoorOri + DoorSize;
		FVector2D Minhandlepos = HandleOri - HandleSize;
		FVector2D Maxhandlepos = HandleOri + HandleSize;

		float offset = 0.f;
		if (LCategoryid == 200028)
			offset = 2.5f;
		else
			offset = 3.5f;

		if (data.X > 0.f)
			offsetPos.X = Maxdoorpos.X - offset - HandleSize.X + (offset * tempscale.X - offset) / 2.f;
		else if (data.X < 0.f)
			offsetPos.X = Mindoorpos.X + offset + HandleSize.X -(offset * tempscale.X - offset) / 2.f;
		else
			offsetPos.X = DoorOri.X;

		if (data.Y > 0.f)
			offsetPos.Y = Maxdoorpos.Y - offset - HandleSize.Y +(offset * tempscale.Z - offset) / 2.f;
		else if (data.Y < 0.f)
			offsetPos.Y = Mindoorpos.Y + offset + HandleSize.Y -(offset * tempscale.Z - offset) / 2.f;
		else
			offsetPos.Y = DoorOri.Y;


		
		

	

	return offsetPos;
}

bool AHouseCustomActor::FindComponent(const FString& MaterialName, TArray<UhouseCustomTemplete*>& Componts)
{
	auto temp = ComponentType.Find(MaterialName);
	bool bIs = false;
	if (temp)
	{
		Componts = (*temp);
		bIs = true;
	}
		
	return bIs;
}


void AHouseCustomActor::SetClassSaveData(int32 Index,int32 doorHandleOffset, int32 doorhandleDirection)
{
	subModelComponentArr[Index]->associatedAnchor.doorHandleOffset = doorHandleOffset;
	subModelComponentArr[Index]->associatedAnchor.doorhandleDirection = doorhandleDirection;
}