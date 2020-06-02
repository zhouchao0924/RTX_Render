// Copyright? 2017 ihomefnt All Rights Reserved.


#include "CustomLayoutFunctionLibrary.h"
#include "Math/LayoutMath.h"
#include "ExpansionDrawingActor.h"
//#include "HouseFunction/HouseFunctionLibrary.h"
#include "DynamicTableboard.h"
#include "../Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "houseCustomTemplete.h"

class AFunctionManage;

TArray<FExpansionTest> UCustomLayoutFunctionLibrary::CreateExpansionDrawingMesh(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionLine>& WallList, FString mx, TArray<AExpansionDrawingActor*>&TopBoard, AFunctionManage* manage)
{
	if (FurActor.Num() == 0)
		return TArray<FExpansionTest>();

	TArray<ExpansionBasic>TempList;
	TArray<TArray<int32>> Actorids;
	FVector orige;
	TMap<int32, AHouseCustomActor*>FurActors;
	for (int i = 0; i < FurActor.Num(); ++i)
	{
		FurActors.Add(FurActor[i]->ActorId, FurActor[i]);

		int  y, z, x;
		FVector Size, Ori = FurActor[i]->GetActorLocation();
		TArray<UhouseCustomTemplete*> Outconpoment;
		FString name(TEXT("柜身"));
		FurActor[i]->FindComponent(name, Outconpoment);
		FurActor[i]->compoundElementInterface->GetElementSize(Outconpoment[0]->ElementID, y, z, x);
		Size.Set(x, y, z);
		Size *= FurActor[i]->GetActorScale3D();
		Size /= 20.f;
		//Ori = FurActor[i]->GetActorRightVector()* 0.5f + Ori;

		FVector tempnor(FMath::Abs(FurActor[i]->GetActorRightVector().X), FMath::Abs(FurActor[i]->GetActorRightVector().Y), 0.f);
		

		FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
		
		Ori.Set(Ori.X, Ori.Y, 0.f);
		LayoutMath::GetInstance()->GetABSvector(Extent);
		Extent = Extent + tempnor * 1.5f;
		bool bIs = true;
		//FVector Size, Ori;
		//FurActor[i]->GetActorBounds(bIs, Ori, Size);
		//Ori = FurActor[i]->GetActorRightVector()* 0.7f + Ori;
		if (i == 0)
		{
			FVector Size1, Ori;
			FurActor[i]->GetActorBounds(bIs, orige, Size1);
			
		}
	//	FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
		//Ori.Set(Ori.X, Ori.Y, 0.f);
		//LayoutMath::GetInstance()->GetABSvector(Extent);
		//if (FurActor[i]->TemplateSize.Y > 0.f)
		//	Size = (FurActor[i]->TemplateSize.Y / 20.f) - 0.5f;
		//else
		

		ExpansionBasic Temp(Ori, Extent, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);
		TempList.Add(Temp);
	}

	TArray<FExpansionTest> Testlist;
	for (auto i : TempList)
	{
		FExpansionTest temp;
		temp.size = i.Size;
		temp.Pos = i.Pos;
		Testlist.Add(temp);
		//UKismetSystemLibrary::DrawDebugBox(WorldContextObject, FVector(i.Pos.X, i.Pos.Y, 100.f), i.Size, FLinearColor::Green, FRotator(), 200, 1.f);
	}
	float TopZ = orige.Z + TempList[0].Size.Z;
	
	auto list = LayoutMath::GetInstance()->TopLineBoardCount(TempList, WallList, Actorids);

	for (int i = 0; i < list.Num(); ++i)
	{
		for (int j = 0; j < list[i].Num() - 1; ++j)
		{
			if (list[i][j].X != list[i][j+1].X && ERRORBANDEX(list[i][j].X, list[i][j+1].X, 2.f))
			{
				list[i][j].X = list[i][j+1].X;
			}
			else if (list[i][j].Y != list[i][j+1].Y && ERRORBANDEX(list[i][j].Y, list[i][j+1].Y, 2.f))
			{
				list[i][j].Y = list[i][j+1].Y;
			}
		}


		auto temp = FurActors.Find(Actorids[i][0]);
		if (!(*temp)->loft.bIsTopboard)
			continue;

		TArray<FVector> tplist;
		for (int j = list[i].Num() - 1 ;j >= 0; --j)
		{
			tplist.Add(list[i][j]);
			
		}

		auto Actor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		TopBoard.Add(Actor);
		Actor->SetFunctionManage(manage);
		Actor->CreateTopMesh(tplist, mx, TopZ, Actorids[i], EArticlelofting::VertexAngle, (*temp)->LCategoryid);
	}

	return Testlist;
}

TArray<FExpansionTest> UCustomLayoutFunctionLibrary::CreateExpansionDrawingMesh1(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionCabnetPillars>& Fluelist, const TArray <FExpansionLine>& WallList,
	const TArray<FString>& mx, TArray<ADynamicTableboard*>& Tableboardlist, TArray<AExpansionDrawingActor*>& BreakWater, TArray<AExpansionDrawingActor*>& SkirtBoard, AFunctionManage* manage)
{
	if (FurActor.Num() == 0)
		return TArray<FExpansionTest>();


	TArray<ExpansionBasic>TempList;
	TMap<FVector2D, TArray<FVector2D>> skuHoule;
	TMap<FVector2D, ASkuComponentActor*> skuPre;
	TArray<int32> specialActorid;
	TArray<float> specialPosZ;
	TArray<AHouseCustomActor*> TempFurActor;
	TMap<int32, AHouseCustomActor*>FurActors;
	float temp_z;
	for (int i = 0; i < FurActor.Num(); ++i)
	{
		FurActors.Add(FurActor[i]->ActorId, FurActor[i]);

		if (FurActor[i]->LCategoryid == 200028 && FurActor[i]->GetActorLocation().Z == 0.f)
			TempFurActor.Add(FurActor[i]);

		if (200036 == FurActor[i]->TemplateCategoryId || FurActor[i]->TemplateId == 37)
		{
			TArray<UhouseCustomTemplete*> Outconpoment;
			FString name(TEXT("柜身"));
			FurActor[i]->FindComponent(name, Outconpoment);
			if (Outconpoment.Num() > 0)
			{
				FVector orige, extent;
				FurActor[i]->GetActorBounds(true, orige, extent);

				FVector center, size;
				FurActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color1");
				center = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), center);
				auto rotator = -FurActor[i]->GetActorRightVector();
				center = (orige + center); //+ (-FurActor[i]->GetActorRightVector() * 0.25f);
				center.Set(center.X, center.Y, 0.f);

				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1"))
					size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1")) / 20.f;
				else
				{
					FurActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color1");
					size /= 2.f;
				}

				size = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), size);
				LayoutMath::GetInstance()->GetABSvector(size);

				//size = size + (-FurActor[i]->GetActorRightVector() * 0.5f);
				size.Z = 40.f;
				//FurActor[i]->ActorId
				ExpansionBasic Temp(center, size, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);
				TempList.Add(Temp);

				FurActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color2");
				center = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), center);
				center = (orige + center);//+ (-FurActor[i]->GetActorForwardVector() * 0.25f);

				center.Set(center.X, center.Y, 0.f);
				rotator = -FurActor[i]->GetActorForwardVector();
				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2"))
					size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2")) / 20.f;
				else
				{
					FurActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color2");
					size /= 2.f;
				}

				//size = size + (-FurActor[i]->GetActorForwardVector() * 0.5f);
				//size = size + (-FurActor[i]->GetActorRightVector() * 0.5f);
				size = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation() + FRotator(0.f, 90.f, 0.f), size);
				LayoutMath::GetInstance()->GetABSvector(size);


				size.Set(size.Y, size.X, 40.f);
				ExpansionBasic Temp1(center, size, -FurActor[i]->GetActorForwardVector(), FurActor[i]->ActorId);
				TempList.Add(Temp1);
				continue;
			}
		}




		auto sku1 = FurActor[i]->SkuComponent.Find(721);
		if (sku1)
		{
			TArray<FVector2D> TempHole;
			if ((*sku1)->GetHole(TempHole))
			{
				auto skupos = (*sku1)->GetActorLocation();
				skuHoule.Add(FVector2D(skupos.X, skupos.Y), TempHole);
				skuPre.Add(FVector2D(skupos.X, skupos.Y), (*sku1));
			}
		}
		auto sku2 = FurActor[i]->SkuComponent.Find(726);
		if (sku2)
		{
			TArray<FVector2D> TempHole;
			if ((*sku2)->GetHole(TempHole))
			{
				auto skupos = (*sku2)->GetActorLocation();
				skuHoule.Add(FVector2D(skupos.X, skupos.Y), TempHole);
				skuPre.Add(FVector2D(skupos.X, skupos.Y), (*sku2));
			}
		}

		if (FurActor[i]->TemplateCategoryId == 200105 || FurActor[i]->TemplateId == 107)
		{
			TArray<UhouseCustomTemplete*> components;
			if (!FurActor[i]->FindComponent(TEXT("台面"), components))
				continue;
			auto size = components[0]->associatedAnchor.componentSize * FurActor[i]->GetActorScale() / 20.f;
			auto orige = components[0]->associatedAnchor.CustomTramsform.GetLocation() * FurActor[i]->GetActorScale();
			orige = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), orige);
			orige = orige + FurActor[i]->GetActorLocation();// +-FurActor[i]->GetActorRightVector() * 1.f;
			orige.Z = 0;
			size = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), size);
			LayoutMath::GetInstance()->GetABSvector(size);

			ExpansionBasic Temp(orige, size, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);

			TempList.Add(Temp);
			specialActorid.Add(FurActor[i]->ActorId);
			specialPosZ.Add(components[0]->associatedAnchor.CustomTramsform.GetLocation().Z + 2.f);
			continue;
		}



		bool bIs = true;
		int  y, z, x;
		FVector Size, Ori = FurActor[i]->GetActorLocation();
		TArray<UhouseCustomTemplete*> Outconpoment;
		FString name(TEXT("柜身"));
		FurActor[i]->FindComponent(name, Outconpoment);
		FurActor[i]->compoundElementInterface->GetElementSize(Outconpoment[0]->ElementID, y, z, x);
		Size.Set(x,y,z);
		Size *= FurActor[i]->GetActorScale3D();
		Size /= 20.f;


		/*if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1"))
			Size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1")) * FurActor[i]->GetActorScale3D() / 20.f;
		else
		{
			
			FurActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, Size, "mesh_color1");
			Size = Size * FurActor[i]->GetActorScale3D() / 2.f;
		}*/



		FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
		Ori.Set(Ori.X, Ori.Y, 0.f);
		LayoutMath::GetInstance()->GetABSvector(Extent);
		/*if (FurActor[i]->LCategoryid != 200028)
		{
			auto tempdir = -FurActor[i]->GetActorRightVector();
			Ori = Ori + (-FurActor[i]->GetActorRightVector() * 0.25f);
			Extent = Extent + (FVector(FMath::Abs(tempdir.X), FMath::Abs(tempdir.Y),0.f) * 0.5f);
		}*/

		ExpansionBasic Temp(Ori, Extent, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);

		TempList.Add(Temp);
	}


	TArray<FExpansionTest> Testlist;
	for (auto i : TempList)
	{
		FExpansionTest temp;
		temp.size = i.Size;
		temp.Pos = i.Pos;
		Testlist.Add(temp);
		//UKismetSystemLibrary::DrawDebugBox(WorldContextObject, FVector(i.Pos.X, i.Pos.Y, 100.f), i.Size, FLinearColor::Green, FRotator(), 200, 1.f);
	}
	auto Temp = Fluelist;
	TArray<TArray<FVector>>ForwardPoints, LastPoints, ForDir;
	TArray<TArray<int32>> Actorids;
	LayoutMath::GetInstance()->TableboardCount(TempList, Temp, ForwardPoints, LastPoints, ForDir);
	//float TopZ = FurActor[0]->GetActorLocation().Z + temp_z * 2.f;

	TMap<int32, float> loftZ;



	////////台面
	TArray<TArray<FVector>> tablelist;
	for (int i = 0; i < ForwardPoints.Num(); ++i)
	{
		auto tableTemp1 = ForwardPoints[i];
		auto temp = LastPoints[i];
		temp.RemoveAt(0);
		temp.RemoveAt(temp.Num() - 1);
		for (int j = temp.Num() - 1; j >= 0; --j)
			tableTemp1.Add(temp[j]);
		tablelist.Add(tableTemp1);
	}

	for (int i = 0; i < tablelist.Num(); ++i)
	{
		auto Templist = LayoutMath::GetInstance()->FVector3DtoFvector2D(tablelist[i]);

		TArray<int32> tempt;
		for (auto te : FurActor)
		{
			FVector2D pot(te->GetActorLocation().X, te->GetActorLocation().Y);
			if (LayoutMath::GetInstance()->ChekColseInterval(pot, Templist))
				tempt.Add(te->ActorId);
		}
		Actorids.Add(tempt);

		if (Actorids[i].Num() <= 0)
			continue;

		auto temp = FurActors.Find(Actorids[i][0]);
		if (!(*temp)->loft.bIsTable)
			continue;

		TArray<TArray<FVector2D>>Holelist;
		TArray<ASkuComponentActor*>skucomponents;
		for (auto j : skuHoule)
		{
			if (LayoutMath::GetInstance()->ChekColseInterval(j.Key, Templist))
			{
				Holelist.Add(j.Value);
				skucomponents.Add((*skuPre.Find(j.Key)));
			}
		}

		auto tp = Templist;
		Templist.Empty();
		for (int g = tp.Num() - 1; g >= 0; --g)
		{
			Templist.Add(tp[g]);
		}

		auto Actor = WorldContextObject->GetWorld()->SpawnActor<ADynamicTableboard>();
		Tableboardlist.Add(Actor);
		auto tablemx = mx[0];
		int tempindex;
		for (auto g : skucomponents)
			g->SetDynamicTable(Actor);
		Actor->SetFunctionManage(manage);
		if (Actorids[i].Num() == 1 && (tempindex = specialActorid.Find(Actorids[i][0])) > -1)
		{
			Actor->CreateMesh(Templist, Holelist, 1.5f, tablemx, specialPosZ[tempindex], Actorids[i], (*temp)->LCategoryid);
		}
		else
		{
			auto tempactor = *FurActors.Find(Actorids[i][0]);

			bool bIs = true;
			FVector Size, Ori;
			tempactor->GetActorBounds(bIs, Ori, Size);
			temp_z = Size.Z;

			if (tempactor->LCategoryid == 200051)//FurActor[i]->TemplateCategoryId == 200056 || FurActor[i]->TemplateId == 113)
			{
				if (tempactor->SkuComponent.Num() == 2 && !tempactor->FindCutomTemplate(TEXT("支撑脚")))
					temp_z = tempactor->TemplateSize.Z / 20.f - 1.5f;
			}
			else if (tempactor->LCategoryid == 200028)
			{
				temp_z = 41.25f;
			}

			float TopZ = tempactor->GetActorLocation().Z + temp_z * 2.f;
			loftZ.Add(i, TopZ);

			Actor->CreateMesh(Templist, Holelist, 1.5f, tablemx, TopZ, Actorids[i], (*temp)->LCategoryid);
		}
	}
	////////////////////////////////////////////



	/////后挡水
	auto Breaklastwaterlist = LayoutMath::GetInstance()->RemovedLastBreakwaterPoint(ForwardPoints, LastPoints, WallList);
	for (int i = 0; i < Breaklastwaterlist.Num(); ++i)
	{

		if (Actorids[i].Num() <= 0)
			continue;

		auto temp = FurActors.Find(Actorids[i][0]);
		if (!(*temp)->loft.bIsTable)
			continue;
		//TArray<FVector> tplist;
		//for (int j = i.Num() - 1; j >= 0; --j)
		//	tplist.Add(i[j]);

		auto tcros = FVector::CrossProduct((Breaklastwaterlist[i][0] - Breaklastwaterlist[i][1]).GetSafeNormal(), FVector(0, 0, 1.f));
		FLine templine(Breaklastwaterlist[i][1], Breaklastwaterlist[i][0]);
		auto Tmiddlepos = templine.GetMiddlePoint() + FVector2D(tcros.X, tcros.Y) * 5.f;
		auto Templist = LayoutMath::GetInstance()->FVector3DtoFvector2D(tablelist[i]);
		TArray<FVector> Templistbreak;
		if (!LayoutMath::GetInstance()->ChekColseInterval(Tmiddlepos, Templist))
		{

			for (int len = Breaklastwaterlist[i].Num() - 1; len > -1; --len)
				Templistbreak.Add(Breaklastwaterlist[i][len]);

		}
		else
			Templistbreak = Breaklastwaterlist[i];

		auto Actor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		BreakWater.Add(Actor);
		Tableboardlist[i]->Addbreakwaters(Actor);
		Actor->SetTable(Tableboardlist[i]);
		auto Breakwatermx = mx[1];
		int tempindex;
		Actor->SetFunctionManage(manage);
		if (Actorids[i].Num() == 1 && (tempindex = specialActorid.Find(Actorids[i][0])) > -1)
		{
			Actor->CreateTopMesh(Templistbreak, Breakwatermx, specialPosZ[tempindex] + 2.f, Actorids[i], EArticlelofting::LastBreakwater, (*temp)->LCategoryid);
		}
		else
		{
			float TopZ = *loftZ.Find(i);
			Actor->CreateTopMesh(Templistbreak, Breakwatermx, TopZ + 1.f, Actorids[i], EArticlelofting::LastBreakwater, (*temp)->LCategoryid);
		}
	}
	//////////////////////////

	/////前挡水
	TArray<TArray<FVector>> Breakforwardwaterlist = ForwardPoints;

	LayoutMath::GetInstance()->RemovedPointfloor(Breakforwardwaterlist, WallList);
	//LayoutMath::GetInstance()->MoveForwardPoints(Breakforwardwaterlist, LastPoints, 1.8f);

	for (int i = 0; i < Breakforwardwaterlist.Num(); ++i)
	{

		if (Actorids[i].Num() <= 0)
			continue;

		auto temp = FurActors.Find(Actorids[i][0]);
		if (!(*temp)->loft.bIsTable)
			continue;

		if ((*temp)->TemplateCategoryId == 200105)
		{
			Breakforwardwaterlist[i].RemoveAt(Breakforwardwaterlist[i].Num() - 1);
			Breakforwardwaterlist[i].RemoveAt(0);
		}

		TArray<FVector> tplist;
		for (int j = Breakforwardwaterlist[i].Num() - 1; j >= 0; --j)
			tplist.Add(Breakforwardwaterlist[i][j]);

		auto Actor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		BreakWater.Add(Actor);
		auto Breakforwardwatermx = mx[2];
		Tableboardlist[i]->Addbreakwaters(Actor);
		Actor->SetTable(Tableboardlist[i]);
		int tempindex;
		Actor->SetFunctionManage(manage);
		if (Actorids[i].Num() == 1 && (tempindex = specialActorid.Find(Actorids[i][0])) > -1)
		{
			Actor->CreateTopMesh(tplist, Breakforwardwatermx, specialPosZ[tempindex] - 2.46f, Actorids[i], EArticlelofting::ForwardBreakwater, (*temp)->LCategoryid);
		}
		else
		{
			float TopZ = *loftZ.Find(i);
			Actor->CreateTopMesh(tplist, Breakforwardwatermx, TopZ - 2.46f, Actorids[i], EArticlelofting::ForwardBreakwater, (*temp)->LCategoryid);
		}
	}
	////////////////////////

	///////踢脚线
	if (mx.Num() < 4)
		return Testlist;

	TArray<TArray<FVector>> SkirtingBoard = ForwardPoints;
	for (int i = 0; i < SkirtingBoard.Num(); ++i)
	{
		if (Actorids[i].Num() <= 0)
			continue;

		auto temp = FurActors.Find(Actorids[i][0]);
		if (!(*temp)->loft.bIsSkirting)
			continue;

		//TArray<FVector> tplist;
		//for (int j = i.Num() - 1; j >= 0; --j)
		//	tplist.Add(i[j]);
		SkirtingBoard[i].RemoveAt(0);
		SkirtingBoard[i].RemoveAt(SkirtingBoard[i].Num() - 1);
		auto Actor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		SkirtBoard.Add(Actor);
		auto SkirtingBoardmx = mx[3];
		Actor->SetFunctionManage(manage);
		Actor->CreateTopMesh(SkirtingBoard[i], SkirtingBoardmx, 0, Actorids[i], EArticlelofting::Skirtingboard, (*temp)->LCategoryid);
	}
	///////////////////////////


	return Testlist;
}

TArray<TArray<FVector>> UCustomLayoutFunctionLibrary::GetForwardPoints(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionLine>& WallList, FString mx, AFunctionManage* manage)
{
	if (FurActor.Num() == 0)
		return TArray<TArray<FVector>>();

	TArray<ExpansionBasic>TempList;
	TArray<TArray<int32>> Actorids;
	FVector orige;
	TMap<int32, AHouseCustomActor*>FurActors;
	for (int i = 0; i < FurActor.Num(); ++i)
	{
		FurActors.Add(FurActor[i]->ActorId, FurActor[i]);

		if (200036 == FurActor[i]->TemplateCategoryId || FurActor[i]->TemplateId == 37)
		{
			TArray<UhouseCustomTemplete*> Outconpoment;
			FString name(TEXT("柜身"));
			FurActor[i]->FindComponent(name, Outconpoment);
			if (Outconpoment.Num() > 0)
			{
				FVector orige, extent;
				FurActor[i]->GetActorBounds(true, orige, extent);

				FVector center, size;
				FurActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color1");
				center = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), center);
				auto rotator = -FurActor[i]->GetActorRightVector();
				center = (orige + center)+(-FurActor[i]->GetActorRightVector() * 0.9f);

				center = center + (-FurActor[i]->GetActorForwardVector() * 0.5f);

				center.Set(center.X, center.Y, 0.f);

				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1"))
					size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1")) / 20.f;
				else
				{
					FurActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color1");
					size /= 2.f;
				}
				size.X -= 1.f;
				size.Y = size.Y + 1.8f;
				size.Z = 40.f;
				//FurActor[i]->ActorId
				ExpansionBasic Temp(center, size, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);
				TempList.Add(Temp);

				FurActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color2");
				center = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), center);
				center = (orige + center) +(-FurActor[i]->GetActorForwardVector() * 0.9f);

				center.Set(center.X, center.Y, 0.f);
				rotator = -FurActor[i]->GetActorForwardVector();
				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2"))
					size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2")) / 20.f;
				else
				{
					FurActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color2");
					size /= 2.f;
				}
				size.Set(size.Y, size.X, 40.f);
				size.Y = size.Y + 1.8f;

				ExpansionBasic Temp1(center, size, -FurActor[i]->GetActorForwardVector(), FurActor[i]->ActorId);
				TempList.Add(Temp1);
				continue;
			}
		}


		bool bIs = true;
		FVector Size, Ori;
		FurActor[i]->GetActorBounds(bIs, Ori, Size);
		Ori = FurActor[i]->GetActorRightVector()* 0.35f + Ori;
		if (i == 0)
			orige = Ori;
		FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
		Ori.Set(Ori.X, Ori.Y, 0.f);
		LayoutMath::GetInstance()->GetABSvector(Extent);


		if (FurActor[i]->TemplateSize.Y > 0.f)
			Extent.Y = (FurActor[i]->TemplateSize.Y / 20.f) - 0.7f;
		else
			Extent.Y -= 0.7f;

		ExpansionBasic Temp(Ori, Extent, -FurActor[i]->GetActorRightVector(), FurActor[i]->ActorId);
		TempList.Add(Temp);
	}
	float TopZ = orige.Z + TempList[0].Size.Z;

	auto list = LayoutMath::GetInstance()->TopLineBoardCountEx(TempList, WallList, Actorids);
	TArray<TArray<FVector>> Templist;
	////for (int i = 0; i < list.Num(); ++i)
	////{
	////	auto temp = FurActors.Find(Actorids[i][0]);
	////	if (!(*temp)->loft.bIsTopboard)
	////		continue;

	////	TArray<FVector> tplist;
	////	for (int j = list[i].Num() - 1; j >= 0; --j)
	////	{
	////		tplist.Add(list[i][j]);

	////	}
	////	Templist.Add(tplist);

	////	//auto Actor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
	////	//TopBoard.Add(Actor);
	////	//Actor->SetFunctionManage(manage);
	////	//Actor->CreateTopMesh(tplist, mx, TopZ, Actorids[i], EArticlelofting::VertexAngle, (*temp)->LCategoryid);
	////}

	return list;

}

void UCustomLayoutFunctionLibrary::LoadloftData(UObject * WorldContextObject, const TMap<int32, FString>& mx, const FCustomLoftdata & data, AFunctionManage* manage,TArray<ADynamicTableboard*>& Tableboardlist, TArray<AExpansionDrawingActor*>& BreakWater, TArray<AExpansionDrawingActor*>& SkirtBoard, TArray<AExpansionDrawingActor*>&TopBoard, TMap<int32, AHouseCustomActor*> FurActor)
{
	for (int i = 0; i < data.Tablelist.Num(); ++i)
	{
		auto Actor = WorldContextObject->GetWorld()->SpawnActor<ADynamicTableboard>();

		Actor->SetFunctionManage(manage);
		auto points = data.Tablelist[i].ToPointList;
		auto holes = data.Tablelist[i].Holes;
		auto hight = data.Tablelist[i].Hight;
		auto ids = data.Tablelist[i].Ids;
		TArray<TArray<FVector2D>> tempholes;
		tempholes.Add(holes);
		if (ids.Num() == 0)
			return;
		FString Tempmx;
		if (data.Tablelist[i].mx.IsEmpty())
		{
			auto temp0 = mx.Find(0);
			if (temp0 == nullptr)
				return;

			Tempmx = *temp0;
		}
		else
			Tempmx = data.Tablelist[i].mx;

			auto tempactor = FurActor.Find(ids[0]);
			if (!tempactor)
				return;

			Actor->CreateMesh(points, tempholes, 1.5f, Tempmx, hight, ids, (*tempactor)->LCategoryid);
			Tableboardlist.Add(Actor);
			Actor->bIsFinish = true;
		
		/////////////////////////
		if (data.ForwardBreakwater.Num() > i)
		{
			auto ForwardbreakActor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
			auto temppoints = data.ForwardBreakwater[i].ToPointList;
			hight = data.ForwardBreakwater[i].Hight;
			ids = data.ForwardBreakwater[i].Ids;
			//auto Breakforwardwatermx = mx[2];
			ForwardbreakActor->SetTable(Actor);
			ForwardbreakActor->SetFunctionManage(manage);
			Tableboardlist[i]->Addbreakwaters(ForwardbreakActor);
			FString Tempmx1;
			if (data.ForwardBreakwater[i].mx.IsEmpty())
			{
				auto temp0 = mx.Find(2);
				if (temp0 == nullptr)
					return;

				Tempmx1 = *temp0;
			}
			else
				Tempmx1 = data.ForwardBreakwater[i].mx;

				auto tempactor = FurActor.Find(ids[0]);
				if (!tempactor)
					return;
				ForwardbreakActor->CreateTopMesh(temppoints, Tempmx1, hight, ids, EArticlelofting::ForwardBreakwater, (*tempactor)->LCategoryid);
				BreakWater.Add(ForwardbreakActor);
				ForwardbreakActor->bIsFinish = true;
			
		}
		/////////////////////////////////////

		if (data.LastBreakwater.Num() > i)
		{
			auto LastBreakActor = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
			auto temppoints = data.LastBreakwater[i].ToPointList;
			hight = data.LastBreakwater[i].Hight;
			ids = data.LastBreakwater[i].Ids;
			
			LastBreakActor->SetTable(Actor);
			LastBreakActor->SetFunctionManage(manage);
			Tableboardlist[i]->Addbreakwaters(LastBreakActor);
			FString Tempmx1;
			if (data.LastBreakwater[i].mx.IsEmpty())
			{
				auto temp0 = mx.Find(1);
				if (temp0 == nullptr)
					return;

				Tempmx1 = *temp0;
			}
			else
				Tempmx1 = data.LastBreakwater[i].mx;

				auto tempactor = FurActor.Find(ids[0]);
				if (!tempactor)
					return;
				LastBreakActor->CreateTopMesh(temppoints, Tempmx1, hight, ids, EArticlelofting::LastBreakwater, (*tempactor)->LCategoryid);
				BreakWater.Add(LastBreakActor);
				LastBreakActor->bIsFinish = true;
			
		}
		/////////////////////////////////////
		
	}


	for (int i = 0; i < data.Skirtboard.Num(); ++i)
	{
		auto SkirtingBoard = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		auto temppoints = data.Skirtboard[i].ToPointList;
		auto hight = data.Skirtboard[i].Hight;
		auto ids = data.Skirtboard[i].Ids;
		//auto Breakforwardwatermx = mx[3];
		SkirtingBoard->SetFunctionManage(manage);
		FString Tempmx1;
		if (data.Skirtboard[i].mx.IsEmpty())
		{
			auto temp0 = mx.Find(3);
			if (temp0 == nullptr)
				return;

			Tempmx1 = *temp0;
		}
		else
			Tempmx1 = data.Skirtboard[i].mx;

			auto tempactor = FurActor.Find(ids[0]);
			if (!tempactor)
				return;
			SkirtingBoard->CreateTopMesh(temppoints, Tempmx1, hight, ids, EArticlelofting::Skirtingboard, (*tempactor)->LCategoryid);
			SkirtBoard.Add(SkirtingBoard);
			SkirtingBoard->bIsFinish = true;
		
	}

	for (int i = 0; i < data.TopBoard.Num(); ++i)
	{
		auto TempTopBoard = WorldContextObject->GetWorld()->SpawnActor<AExpansionDrawingActor>();
		auto temppoints = data.TopBoard[i].ToPointList;
		auto hight = data.TopBoard[i].Hight;
		auto ids = data.TopBoard[i].Ids;
		//auto Breakforwardwatermx = mx[4];

		TempTopBoard->SetFunctionManage(manage);
		FString Tempmx1;
		if (data.TopBoard[i].mx.IsEmpty())
		{
			auto temp0 = mx.Find(4);
			if (temp0 == nullptr)
				return;

			Tempmx1 = *temp0;
		}
		else
			Tempmx1 = data.TopBoard[i].mx;
		
			auto tempactor = FurActor.Find(ids[0]);
			if (!tempactor)
				return;
			TempTopBoard->CreateTopMesh(temppoints, Tempmx1, hight, ids, EArticlelofting::VertexAngle, (*tempactor)->LCategoryid);
			TopBoard.Add(TempTopBoard);
			TempTopBoard->bIsFinish = true;
		
	}

}
