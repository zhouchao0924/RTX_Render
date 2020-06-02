// Copyright? 2017 ihomefnt All Rights Reserved.


#include "ManageObjectC.h"

AManageObjectC* AManageObjectC::Myinstance = nullptr;
// Sets default values
AManageObjectC::AManageObjectC()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Myinstance = this;
	FurTreeData = nullptr;
}

// Called when the game starts or when spawned
void AManageObjectC::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AManageObjectC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AManageObjectC* AManageObjectC::GetInstance()
{
	return Myinstance;
}

void AManageObjectC::SetFunctionManage(AFunctionManage* ref)
{
	FunctionManageInstance = ref;
	//FunctionManageInstance->AddToRoot();
}

void AManageObjectC::CreateCutomFurTree(UVaRestJsonObject* Json)
{
	RemoveAll(FurTreeData);
	FurTreeData = new FCustomFurTree();

	GetJsonData(Json, FurTreeData);
}

void  AManageObjectC::GetJsonData(UVaRestJsonObject* Json, FCustomFurTree* child)
{

	UVaRestJsonObject* CurreentNode = Json->GetObjectField("categoryDto");
	if (CurreentNode)
	{
		child->Level = CurreentNode->GetNumberField("level");
		child->Categoryid = CurreentNode->GetNumberField("id");
		child->ParentCategoryid = CurreentNode->GetNumberField("parentId");
		child->name = CurreentNode->GetStringField("name");
		if (child->Level == 3)
		{
			TempCategoryid = CurreentNode->GetNumberField("id");
			TName = child->name;
		}
		else if (child->Level > 3)
		{
			child->TwoCategoryid = TempCategoryid;
			child->TwoName = TName;
		}
	}

	auto NextNode = Json->GetObjectArrayField("categoryTreeNodeDtos");
	if (NextNode.Num())
	{
		for (auto i : NextNode)
		{

			FCustomFurTree* next = new FCustomFurTree();
			GetJsonData(i, next);
			child->child.Add(next);
		}
	}

}
bool AManageObjectC::GetCurrentData(FCurreentLevelData& data, TArray<FCurreentLevelData>& ChildData, int32 Categoryid)
{
	ChildTemp.Empty();
	if (!FurTreeData)
		return false;
	bool bis = GetCrreent(Categoryid, FurTreeData);
	data = temp;
	ChildData = ChildTemp;
	return bis;

}



void AManageObjectC::SetJsonData(TArray<UVaRestJsonObject*> Sku, TArray<UVaRestJsonObject*> Template)
{
	Myinstance->Skulist = Sku;
	Myinstance->TempList = Template;
}

void  AManageObjectC::GetChildData(TArray<FCurreentLevelData>& Datalist, FCustomFurTree* Furdata)
{
	for (auto i : Furdata->child)
	{
		FCurreentLevelData temp;
		temp.Categoryid = i->Categoryid;
		temp.ParentCategoryid = i->ParentCategoryid;
		temp.TwoCategoryid = i->TwoCategoryid;
		temp.Level = i->Level;
		temp.name = i->name;
		temp.TwoName = i->TwoName;
		Datalist.Add(temp);

		if (i->child.Num() > 0)
		{
			GetChildData(Datalist, i);
		}
	}
}

bool  AManageObjectC::GetCrreent(int32 Categoryid, FCustomFurTree*Furdata)
{

	if (Furdata->Categoryid != Categoryid)
	{
		if (Furdata->child.Num())
		{
			for (auto i : Furdata->child)
			{
				if (GetCrreent(Categoryid, i))
					return true;
			}

			return false;
		}
		else
			return false;

	}
	temp.Categoryid = Furdata->Categoryid;
	temp.ParentCategoryid = Furdata->ParentCategoryid;
	temp.TwoCategoryid = Furdata->TwoCategoryid;
	temp.Level = Furdata->Level;
	temp.name = Furdata->name;
	temp.TwoName = Furdata->TwoName;
	GetChildData(ChildTemp, Furdata);

	return true;
}

void  AManageObjectC::ClearTree()
{
	RemoveAll(FurTreeData);
}

TArray<UVaRestJsonObject*> AManageObjectC::GetTemplateList()
{
	return TempList;
}

TArray<UVaRestJsonObject*> AManageObjectC::GetSkuList()
{
	return Skulist;
}

void AManageObjectC::RemoveAll(FCustomFurTree* TreeData)
{
	if (TreeData)
	{
		auto Arraydata = TreeData->child;
		delete TreeData;
		TreeData = nullptr;

		for (auto i : Arraydata)
			RemoveAll(i);
	}
}

AManageObjectC::~AManageObjectC()
{
	RemoveAll(FurTreeData);
}