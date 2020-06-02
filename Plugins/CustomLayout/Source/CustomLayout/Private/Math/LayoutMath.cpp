// Copyright? 2017 ihomefnt All Rights Reserved.

#include "Math/LayoutMath.h"
#include "../Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include"UnrealMathUtility.h"

LayoutMath* LayoutMath::Instance = nullptr;
LayoutMath::LayoutMath()
{
}

LayoutMath::~LayoutMath()
{
	if (Instance)
		delete Instance;
}

LayoutMath* LayoutMath::GetInstance()
{
	if (!Instance)
		Instance = new LayoutMath();

	return Instance;
}

FVector LayoutMath::GetPosToRotator(FRotator rot, FVector Pos)
{
	auto rotation = rot.Quaternion();
	FTransform const Trabsform(rotation);
	FVector Temp = Trabsform.TransformPosition(Pos);
	return Temp;
}

TMap<FVector, TArray<ExpansionBasic>> LayoutMath::GetClassifyData(const TArray<ExpansionBasic>& TempList)
{
	TMap<FVector, TArray<ExpansionBasic>> ClassifyData;
	for (int i = 0; i < TempList.Num(); ++i)
	{		
		TArray<FVector> keylist;
		ClassifyData.GenerateKeyArray(keylist);
		TArray<ExpansionBasic>* Temp = nullptr;
		FVector Dirtemp = TempList[i].Dir;
		for (int j = 0; j < keylist.Num();++j)
		{
			if (keylist[j].Equals(TempList[i].Dir, 0.05f))
			{
				Temp = ClassifyData.Find(keylist[j]);
				Dirtemp = keylist[j];
				break;
			}
		}
		
		if (Temp)
		{
			(*Temp).Add(TempList[i]);
			ClassifyData.Add(Dirtemp, (*Temp));
		}
		else
		{
			TArray<ExpansionBasic> data;
			data.Add(TempList[i]);
			ClassifyData.Add(Dirtemp, data);
		}
	}
	return ClassifyData;
}

ExpansionAuadrangle LayoutMath::GetAuadranglePoint(FVector size, FVector dir, FVector Pos, bool bIs)
{
	ExpansionAuadrangle Temp;
	FVector Cor(0, 0, 1.f);
	auto DirRight = FVector::CrossProduct(dir, Cor);
	if (bIs)
	{
		auto Forwordpos = dir * size.Y + Pos;
		Temp.ForwordPos1 = Forwordpos + DirRight * size.X;
		Temp.ForwordPos2 = Forwordpos + -DirRight * size.X;

		auto Lastpos = -dir * size.Y + Pos;
		Temp.LastPos1 = Lastpos + DirRight * size.X;
		Temp.LastPos2 = Lastpos + -DirRight * size.X;
	}
	else
	{
		auto Forwordpos = dir * size + Pos;
		Temp.ForwordPos1 = Forwordpos + DirRight * size;
		Temp.ForwordPos2 = Forwordpos + -DirRight * size;

		auto Lastpos = -dir * size + Pos;
		Temp.LastPos1 = Lastpos + DirRight * size;
		Temp.LastPos2 = Lastpos + -DirRight * size;
	}

	return Temp;
}

struct TPointMap
{
	TPointMap(int ind, float len)
	{
		length = len;
		index = ind;
	}
	float length;
	int index;
};

TArray<ExpansionAuadrangle> LayoutMath::GetAuadrangle(TMap<FVector, TArray<ExpansionBasic>>& Data)
{
	TArray<ExpansionAuadrangle> TempList;
	TArray<TArray<ExpansionBasic>> TeList;
	for (auto i : Data)
	{
		FVector Cor(0, 0, 1.f);
		auto PiPos = i.Value[0].Pos;
		auto ridir = FVector::CrossProduct(i.Value[0].Dir, Cor);
		int max = 0, min = 0;
		float maxlen = 0, minlen = 0;
		TArray<TPointMap> maxtemplist, mintemplist;
	
		for (int j = 1; j < i.Value.Num(); ++j)
		{
			auto tempdir = (i.Value[j].Pos - PiPos).GetSafeNormal();

			if (tempdir.Equals(ridir, 0.1f))
			{
				auto templen = (PiPos - i.Value[j].Pos).Size();
				if (templen > maxlen)
				{
					TPointMap temp(j, templen);
					maxtemplist.Add(temp);
					max = j;
					maxlen = templen;

				}
				else
				{
					bool bIs = false;
					for (int g = 0; g < maxtemplist.Num(); ++g)
					{
						if (maxtemplist[g].length > templen)
						{
							TPointMap temp(j, templen);
							maxtemplist.Insert(temp, g);
							bIs = true;
							break;
						}
					}
					if (!bIs)
					{
						TPointMap temp(j, templen);
						maxtemplist.Add(temp);
					}

				}

			}
			else
			{
				auto templen = (PiPos - i.Value[j].Pos).Size();
				if (templen > minlen)
				{
					TPointMap temp(j, templen);
					mintemplist.Add(temp);
					min = j;
					minlen = templen;
				}
				else
				{
					bool bIs = false;
					for (int g = 0; g < mintemplist.Num(); ++g)
					{
						if (mintemplist[g].length > templen)
						{
							TPointMap temp(j, templen);
							mintemplist.Insert(temp, g);
							bIs = true;
							break;
						}
					}
					if (!bIs)
					{
						TPointMap temp(j, templen);
						mintemplist.Add(temp);
					}
				}
			}
		}

		TArray<int> ordrList;
		for (int h = mintemplist.Num() - 1; h >= 0; --h)
			ordrList.Add(mintemplist[h].index);

		ordrList.Add(0);

		for (int h = 0;h < maxtemplist.Num(); ++h)
			ordrList.Add(maxtemplist[h].index);
		TArray<ExpansionBasic> odlist;
		for (auto t = 0; t < ordrList.Num() - 1 ; ++t)
		{ 
			if (t == 0)
				odlist.Add(i.Value[ordrList[t]]);
			
			auto lenm = (i.Value[ordrList[t]].Pos - i.Value[ordrList[t + 1]].Pos).Size();
			auto tempnor = (i.Value[ordrList[t]].Pos - i.Value[ordrList[t + 1]].Pos).GetSafeNormal();
			tempnor.Set(FMath::Abs(tempnor.X), FMath::Abs(tempnor.Y), 0.f);
			auto lenm1 = (i.Value[ordrList[t]].Size * tempnor + i.Value[ordrList[t + 1]].Size * tempnor).Size();
			if (LayoutMath::GetInstance()->GetAbs(lenm - lenm1)<1.2f)
				odlist.Add(i.Value[ordrList[t + 1]]);
			else
			{
				TeList.Add(odlist);
				odlist.Empty();
				odlist.Add(i.Value[ordrList[t + 1]]);
			}
		}
		if(odlist.Num() > 0)
			TeList.Add(odlist);


		if (i.Value.Num() == 1)
		{
			odlist.Add(i.Value[0]);
			TeList.Add(odlist);
		}
	}

	for (auto i : TeList)
	{
		ExpansionAuadrangle Temp;
		auto alllen = i.Num() - 1;
		TArray<int32>ids;
		for (auto j : i)
			ids.Add(j.ID);
		

		if (alllen == 0)
		{
			Temp = GetAuadranglePoint(i[0].Size, i[0].Dir, i[0].Pos, false);
			TempList.Add(Temp);
			continue;
		}
	
		float pointlen = (i[0].Pos - i[alllen].Pos).Size();
		auto tempdir = (i[0].Pos - i[alllen].Pos).GetSafeNormal();
		tempdir.Set(FMath::Abs(tempdir.X), FMath::Abs(tempdir.Y),0.f);
		FVector tempdiry(tempdir.Y, tempdir.X, 0.f);

		FVector msize((pointlen + (i[0].Size * tempdir + i[alllen].Size * tempdir).Size()) / 2.f, (i[0].Size * tempdiry).Size(), i[alllen].Size.Z);
		auto dir = (i[0].Pos - i[alllen].Pos).GetSafeNormal();
		auto pos1 = i[0].Pos + dir * i[0].Size.X;
		auto pos2 = i[alllen].Pos + -dir * i[alllen].Size.X;
		auto startpos = i[0].Pos + i[0].Size * dir;
		auto endpos = i[alllen].Pos + i[alllen].Size * -dir;
		//msize.Set((startpos - endpos).Size(), (i[0].Size * tempdiry).Size(), i[alllen].Size.Z);


		FVector Pos = GetMiddPoint(startpos, endpos);
		Temp = GetAuadranglePoint(msize, i[0].Dir, Pos,true);
		TempList.Add(Temp);
	}

	return TempList;
}


void LayoutMath::GetABSvector(FVector& data)
{
	data.Set(FMath::Abs(data.X), FMath::Abs(data.Y), FMath::Abs(data.Z));
}
float LayoutMath::GetAbs(float b)
{
	return FMath::Abs(b);
}

bool LayoutMath::IsPointAsLine(FVector2D& point, FLine& line)
{
	FVector2D outPos;
	if (point.X == line.Start.X || point.Y == line.Start.Y)
		outPos = point;
	else
		GetSubPoint(point, line, outPos);

	float Length = line.GetLineLength();
	float Slen = (outPos - line.Start).Size();
	float Elen = (outPos - line.End).Size();

	return ERRORBAND((Slen + Elen), Length);
}

bool LayoutMath::IsPointToLine(FVector2D & point, FLine & line)
{
	float Length = line.GetLineLength();
	float Slen = (point - line.Start).Size();
	float Elen = (point - line.End).Size();

	return ERRORBAND((Slen + Elen), Length);
}

bool LayoutMath::IsPointToLine(FVector2D & point, FLine & line, float offset, FVector & outpos)
{
	FVector2D outPos;
	GetSubPoint(point, line, outPos);



	float Length = line.GetLineLength();
	float Slen = (outPos - line.Start).Size();
	float Elen = (outPos - line.End).Size();
	if (Slen < offset || Elen < offset)
		return false;
	outpos.Set(outPos.X, outPos.Y, 0.f);
	return (GetAbs((Slen + Elen) - Length) < offset && point.Equals(outPos, offset));
}

TArray<TArray<FVector>> LayoutMath::TopLineBoardCountEx(const TArray<ExpansionBasic>& TempList, const TArray <FExpansionLine>& WallList, TArray<TArray<int32>>& Actorids)
{
	auto ClassifyData = GetClassifyData(TempList);
	auto AuadrangleList = GetAuadrangle(ClassifyData);
	auto Splitpoint = SplitAuadrangle(AuadrangleList);
	TArray<TArray<int>>AuadrangleIndex;
	TArray< TArray<FVector>>ForDir;
	auto ForwardPoints = GetForwardPoint(Splitpoint, AuadrangleIndex, ForDir);

	TArray< TArray<FVector>> LastPoints;
	TArray <FExpansionCabnetPillars> Fluelist;
	GetLastPoint(AuadrangleList, AuadrangleIndex, Splitpoint, ForwardPoints, Fluelist, LastPoints);
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

		auto templist = FVector3DtoFvector2D(tableTemp1);
		TArray<int32>tempids;
		for (int j = 0; j < TempList.Num(); ++j)
		{
			FVector2D tpos(TempList[j].Pos.X, TempList[j].Pos.Y);
			if (LayoutMath::GetInstance()->ChekColseInterval(tpos, templist))
			{
				tempids.Add(TempList[j].ID);
			}
		}
		Actorids.Add(tempids);
	}

	TArray<TArray<FVector>> Breakforwardwaterlist = ForwardPoints;

	RemovedPoint(Breakforwardwaterlist, WallList);

	float Length = 2.5f;
	for (int i = 0; i < Breakforwardwaterlist.Num(); ++i)
	{
		auto Templist = LayoutMath::GetInstance()->FVector3DtoFvector2D(tablelist[i]);
		int index = 0;
		int end = Breakforwardwaterlist[i].Num();
		for (int j = 0; j < end; ++j)
		{

			float len = Length;
			if (j > 0 && j < end - 1)
			{
				len = FMath::Sqrt((Length * Length + 1.0f) * 2);
				auto dir = LayoutMath::GetInstance()->GetDir(Breakforwardwaterlist[i][j], Breakforwardwaterlist[i][j - 1], Breakforwardwaterlist[i][j + 1], Templist);
				Breakforwardwaterlist[i][j] = -dir * len + Breakforwardwaterlist[i][j];
			}
			else
			{
				//Breakforwardwaterlist[i][j] = -ForDir[i][index] * len + Breakforwardwaterlist[i][j];
				index += 1;
			}
		}
	}

	return Breakforwardwaterlist;

}

//bool PanIs;
TArray<TArray<FVector>> LayoutMath::TopLineBoardCount(const TArray<ExpansionBasic>& TempList, const TArray <FExpansionLine>& WallList, TArray<TArray<int32>>& Actorids)
{
	//PanIs = true;
	//TArray<TArray<int32>> Actorids;
	auto ClassifyData = GetClassifyData(TempList);
	auto AuadrangleList = GetAuadrangle(ClassifyData);
	auto Splitpoint = SplitAuadrangle(AuadrangleList);
	TArray<TArray<int>>AuadrangleIndex;
	TArray< TArray<FVector>>ForDir;
	auto ForwardPoints = GetForwardPoint(Splitpoint, AuadrangleIndex, ForDir);

	TArray< TArray<FVector>> LastPoints;
	TArray <FExpansionCabnetPillars> Fluelist;
	GetLastPoint(AuadrangleList, AuadrangleIndex, Splitpoint, ForwardPoints, Fluelist, LastPoints);
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

		auto templist = FVector3DtoFvector2D(tablelist[i]);
		TArray<int32>tempids;
		for (int j = 0; j < TempList.Num(); ++j)
		{
			FVector2D tpos(TempList[j].Pos.X, TempList[j].Pos.Y);
			if (LayoutMath::GetInstance()->ChekColseInterval(tpos, templist))
			{
				tempids.Add(TempList[j].ID);
			}
		}
		Actorids.Add(tempids);
	}

	TArray<TArray<FVector>> Breakforwardwaterlist = ForwardPoints;


	RemovedPoint(Breakforwardwaterlist, WallList);
	TArray<TArray<FVector>> Outtemp;
	for (int i = 0; i < Breakforwardwaterlist.Num(); ++i)
	{
		auto tcros = FVector::CrossProduct((Breakforwardwaterlist[i][0] - Breakforwardwaterlist[i][1]).GetSafeNormal(), FVector(0, 0, 1.f));
		FLine templine(Breakforwardwaterlist[i][1], Breakforwardwaterlist[i][0]);
		auto Tmiddlepos = templine.GetMiddlePoint() + FVector2D(tcros.X, tcros.Y) * 5.f;
		auto Templist = LayoutMath::GetInstance()->FVector3DtoFvector2D(tablelist[i]);
		TArray<FVector> Templistbreak;
		if (LayoutMath::GetInstance()->ChekColseInterval(Tmiddlepos, Templist))
		{

			for (int len = Breakforwardwaterlist[i].Num() - 1; len > -1; --len)
				Templistbreak.Add(Breakforwardwaterlist[i][len]);

		}
		else
			Templistbreak = Breakforwardwaterlist[i];


		Outtemp.Add(Templistbreak);

	}


	//float Length = 2.5f;
	//for (int i = 0; i < Breakforwardwaterlist.Num(); ++i)
	//{
	//	auto Templist = LayoutMath::GetInstance()->FVector3DtoFvector2D(tablelist[i]);
	//	int index = 0;
	//	int end = Breakforwardwaterlist[i].Num();
	//	for (int j = 0; j < end ; ++j)
	//	{
	//		
	//		float len = Length;
	//		if (j > 0 && j < end - 1)
	//		{
	//			len = FMath::Sqrt((Length * Length + 1.0f) * 2);
	//		    auto dir = LayoutMath::GetInstance()->GetDir(Breakforwardwaterlist[i][j], Breakforwardwaterlist[i][j - 1], Breakforwardwaterlist[i][j + 1], Templist);
	//			Breakforwardwaterlist[i][j] = -dir * len + Breakforwardwaterlist[i][j];
	//		}
	//		else
	//		{
	//			//Breakforwardwaterlist[i][j] = -ForDir[i][index] * len + Breakforwardwaterlist[i][j];
	//			index += 1;
	//		}
	//	}
	//}



	
	
	return Outtemp;
}

void LayoutMath::TableboardCount(const TArray<ExpansionBasic>& TempList,  TArray <FExpansionCabnetPillars>& Fluelist, TArray<TArray<FVector>>& ForwardPoints, TArray<TArray<FVector>>& LastPoints, TArray<TArray<FVector>>& ForwardDir)
{
	//TArray<TArray<int32>> Actorids;
	auto ClassifyData = GetClassifyData(TempList);
	auto AuadrangleList = GetAuadrangle(ClassifyData);

	//for (int i = 0;i < AuadrangleList.Num(); ++i)
	//{
	//	TArray<FVector2D>temp;
	//	temp.Add(FVector2D(AuadrangleList[i].LastPos1.X, AuadrangleList[i].LastPos1.Y));
	//	temp.Add(FVector2D(AuadrangleList[i].ForwordPos1.X, AuadrangleList[i].ForwordPos1.Y));
	//	temp.Add(FVector2D(AuadrangleList[i].ForwordPos2.X, AuadrangleList[i].ForwordPos2.Y));
	//	temp.Add(FVector2D(AuadrangleList[i].LastPos2.X, AuadrangleList[i].LastPos2.Y));
	//	for (int j = 0; j < Fluelist.Num(); ++j)
	//	{
	//		if (ChekColseInterval(Fluelist[j].Pos, temp))
	//			Fluelist[j].Dir = (temp[1] - temp[0]).GetSafeNormal();
	//			
	//	}
	//}

	auto Splitpoint = SplitAuadrangle(AuadrangleList);
	TArray<TArray<int>>AuadrangleIndex;
	ForwardPoints = GetForwardPoint(Splitpoint, AuadrangleIndex, ForwardDir);

	//if (AuadrangleIndex.Num() > 0)
//	{
	TArray< TArray<FVector>>outLast,TempLast;
	GetLastPoint(AuadrangleList, AuadrangleIndex, Splitpoint, ForwardPoints, Fluelist, LastPoints);


		/*for (int i = 0; i < PointList.Num(); ++i)
		{
			auto temp = outLast[i];
			temp.RemoveAt(0);
			temp.RemoveAt(temp.Num() - 1);
			PointList[i] += temp;
		}*/





	/*	TempLast = outLast;
		for (int i = 0; i < TempLast.Num(); ++i)
		{
			TArray<FVector> Temlist1;
			int nextnum = 0;
			for (int j = 0; j < TempLast[i].Num() - 1; ++j)
			{

				FLine line(TempLast[i][j], TempLast[i][j + 1]);
				TArray<FVector> Temlist;
				bool bIsStart, bIsEnd;
				if (PillarsCount(Fluelist, line, Temlist, bIsStart, bIsEnd))
				{
					
					for (auto g : Temlist)
						outLast[i].Insert(g, j + 1);
					nextnum += Temlist.Num();
					if (bIsStart)
					{
						outLast[i].RemoveAt(j);
						nextnum -= 1;
					}
					if (bIsEnd)
					{
						outLast[i].RemoveAt(j + nextnum + 1);
						nextnum -= 1;
					}

				}
			}*/
		


			////PointList[i].RemoveAt(0);
		//	PointList[i].RemoveAt(PointList[i].Num() - 1);
			//PointList[i] += outLast[i];
		//}
		//testout = outLast;




//	}
	//else if(PointList.Num() > 0)
	//{ 
	//	for (int i = 0; i < PointList.Num(); ++i)
	//	{
	//		FLine line(PointList[i].Last(), PointList[i][0]);
	//		TArray<FVector> Temlist;
	//		bool bIsStart,bIsEnd;
	//		if (PillarsCount(Fluelist, line, Temlist, bIsStart, bIsEnd))
	//		{
	//			if (bIsStart)
	//				PointList[i].RemoveAt(PointList[i].Num() - 1);
	//			if (bIsEnd)
	//				PointList[i].RemoveAt(0);

	//			PointList[i] += Temlist;
	//		}
	//	}
	//}

	//return PointList;
}

FVector LayoutMath::GetMiddPoint(FVector pos1, FVector pos2)
{
	auto dir = (pos1 - pos2).GetSafeNormal();
	auto len = (pos1 - pos2).Size() / 2.f;
	FVector temp = pos2 + dir * len;

	return temp;
}

void LayoutMath::GetLastPoint(const TArray<ExpansionAuadrangle>& AuadrangleList, TArray<TArray<int>>& AuadrangleIndex, const TMap<int, Atbestline>& componet, TArray<TArray<FVector>>& Forwaordpointlist,  TArray <FExpansionCabnetPillars>& Fluelist, TArray<TArray<FVector>>& OutLastPoint)
{
	//TArray<FVector> TempList;

	for (int i = 0; i < Forwaordpointlist.Num(); ++i)
	{
		TArray<FVector> TempList,orli;
		auto firstpoint = Forwaordpointlist[i][0];
		if (AuadrangleIndex[i].Num() > 0)
		{
			for (auto j : AuadrangleIndex[i])
			{
				auto temp = componet.Find(j);
				if (!temp)
					continue;

				if (temp->left == -1 && temp->right == -1)
					continue;

				if (temp->left > -1 && !temp->leftbIs)
				{
				
					auto tp = componet.Find(temp->left);
					TempList.Add(AuadrangleList[j].LastPos1);
					if (tp->left == temp->slef && !(AuadrangleList[tp->slef].LastPos1.Equals(AuadrangleList[temp->slef].ForwordPos1,3.f)))
					{
						orli.Add(AuadrangleList[temp->slef].ForwordPos1);
						orli.Add(AuadrangleList[tp->slef].LastPos1);
						
					}
					else if (tp->right == temp->slef && !(AuadrangleList[tp->slef].LastPos2.Equals(AuadrangleList[temp->slef].ForwordPos1, 3.f)))
					{
						orli.Add(AuadrangleList[temp->slef].ForwordPos1);
						orli.Add(AuadrangleList[tp->slef].LastPos2);
						
					}
					else
					{
						FLine templine(tp->data.ForwordPos1, tp->data.LastPos1);
						FLine templine1(tp->data.ForwordPos2, tp->data.LastPos2);
						FVector2D tempos(AuadrangleList[j].ForwordPos1.X, AuadrangleList[j].ForwordPos1.Y);
						FVector outpos;
						if (IsPointToLine(tempos, templine, 3.f, outpos))
						{
							TempList.Add(outpos);
							TempList.Add(tp->data.LastPos1);
						}
						else if (IsPointToLine(tempos, templine1, 3.f, outpos))
						{
							TempList.Add(outpos);
							TempList.Add(tp->data.LastPos2);
						}
					}
				}

				if (temp->right > -1 && !temp->rightbIs)
				{
					auto tp = componet.Find(temp->right);
					TempList.Add(AuadrangleList[j].LastPos2);
					if (tp->left == temp->slef && !(AuadrangleList[tp->slef].LastPos1.Equals(AuadrangleList[temp->slef].ForwordPos2, 3.f)))
					{
						orli.Add(AuadrangleList[temp->slef].ForwordPos2);
						orli.Add(AuadrangleList[tp->slef].LastPos1);
						
					}
					else if (tp->right == temp->slef && !(AuadrangleList[tp->slef].LastPos2.Equals(AuadrangleList[temp->slef].ForwordPos2, 3.f)))
					{
						orli.Add(AuadrangleList[temp->slef].ForwordPos2);
						orli.Add(AuadrangleList[tp->slef].LastPos2);
						
					}
					else
					{
						FLine templine(tp->data.ForwordPos1, tp->data.LastPos1);
						FLine templine1(tp->data.ForwordPos2, tp->data.LastPos2);
						FVector2D tempos(AuadrangleList[j].ForwordPos2.X, AuadrangleList[j].ForwordPos2.Y);
						FVector outpos;
						if (IsPointToLine(tempos, templine, 3.f, outpos))
						{
							TempList.Add(outpos);
							TempList.Add(tp->data.LastPos1);
						}
						else if (IsPointToLine(tempos, templine1, 3.f, outpos))
						{
							TempList.Add(outpos);
							TempList.Add(tp->data.LastPos2);
						}
					}

					
				

				}
			}
		}
		TempList += orli;
		if (TempList.Num() > 1)
		{
			
			TempList.Insert(firstpoint, 0);
			TempList.Add(Forwaordpointlist[i].Last());
		}
		else
		{
			//TempList += orli;
			TempList.Insert(Forwaordpointlist[i].Last(), 0);
			TempList.Add(firstpoint);
		}

		TArray<FVector> relist1;
		auto relist2 = TempList;
		TArray<FVector> tell;
		tell.Add(TempList[0]);
		relist1.Add(TempList[0]);
		for (int i = 0; i < tell.Num(); ++i)
		{
			
			for (auto j : relist2)
			{
				if (relist1.Find(j) > -1)

					continue;
				if (ERRORBANDEX(j.X, tell[i].X, 3.f) || ERRORBANDEX(j.Y, tell[i].Y, 3.f))
				{
					auto temppos = j;
					if (ERRORBANDEX(j.X, tell[i].X, 3.f))
						temppos.X = tell[i].X;
					else 
						temppos.Y = tell[i].Y;
					tell.Add(temppos);
					relist1.Add(j);
					break;
				}
			}
		}

		OutLastPoint.Add(tell);
	}

	

}

TMap<int, Atbestline> LayoutMath::SplitAuadrangle(TArray<ExpansionAuadrangle>& data)
{
	TMap<int, Atbestline>contentpoints;

	if (data.Num() == 1)
	{
		Atbestline Temp;
		Temp.data = data[0];
		contentpoints.Add(0, Temp);
	}
	else
	{
		for (int i = 0; i < data.Num(); ++i)
		{
			int left = -1, right = -1;
			Atbestline Temp;
			for (int j = 0; j < data.Num(); ++j)
			{
				if (i == j)
					continue;

				/*	auto tempdir1 = (data[i].LastPos1 - data[i].ForwordPos1).GetSafeNormal();
					auto Ipos1 = tempdir1 * 1.8f + data[i].ForwordPos1;
					auto Ipos2 = tempdir1 * 1.8f + data[i].ForwordPos2;

					auto tempdir2 = (data[j].LastPos1 - data[j].ForwordPos1).GetSafeNormal();
					auto jpos1 = tempdir2 * 1.8f + data[j].ForwordPos1;
					auto jpos2 = tempdir2 * 1.8f + data[j].ForwordPos2;*/

					//FLine line1(Ipos1, Ipos2);
					//FLine line2(jpos1, jpos2);

				FLine line1(data[i].ForwordPos1, data[i].ForwordPos2);
				FLine line2(data[j].ForwordPos1, data[j].ForwordPos2);

				int Type = IsInsertPoint(line1, line2, data[i], data[j]);
				

				if (Type > -1 && !IsGuanlian(data[i], data[j]))
						Type = -1;

				Temp.Type = Type;
				if (Type == 1)
				{
					Temp.left = j;
					Temp.leftbIs = true;
				}
				else if (Type == 2)
				{
					Temp.right = j;
					Temp.rightbIs = true;
				}
				else if (Type == 3)
				{
					Temp.left = j;
					Temp.leftbIs = false;
				}
				else if (Type == 4)
				{
					Temp.right = j;
					Temp.rightbIs = false;
				}

			}
			Temp.slef = i;
			Temp.data = data[i];
			contentpoints.Add(i, Temp);

		}
	}

	return contentpoints;
}


TArray<TArray<FVector>> LayoutMath::GetForwardPoint(TMap<int, Atbestline>& contentpoints, TArray<TArray<int>>&AuadrangleIndex, TArray<TArray<FVector>>&OutForwardDir)
{
	TArray<TArray<FVector>> TempList;


	TArray<int>clear;
	int startindex = 0;
	AuadrangleIndex.SetNum(contentpoints.Num());
	int j = -1;
	for (auto i : contentpoints)
	{
		if (clear.Find(i.Value.slef) > -1)
			continue;

		j += 1;
		clear.Add(i.Value.slef);

		if (i.Value.left == -1 && i.Value.right == -1)
		{
			TArray<FVector> Pointlist, dir;
			Pointlist.Add(i.Value.data.LastPos1);
			Pointlist.Add(i.Value.data.ForwordPos1);
			Pointlist.Add(i.Value.data.ForwordPos2);
			Pointlist.Add(i.Value.data.LastPos2);
			TempList.Add(Pointlist);

			FVector d1 = (i.Value.data.ForwordPos1 - i.Value.data.LastPos1).GetSafeNormal();
			FVector d2 = (i.Value.data.ForwordPos2 - i.Value.data.LastPos2).GetSafeNormal();
			dir.Add(d1);
			dir.Add(d2);
			OutForwardDir.Add(dir);
			continue;
		}
		
		//auto temp = i.Value;
		//int type = temp.left;
	
		TArray<int>Tmepindex;
		TArray<FVector> leftdir,rightdir;
		TArray<FVector> leftPointlist = GetTopPointList(contentpoints, leftdir,clear, i.Value, i.Value.left,true);
		for (int g = clear.Num() -1 ; g >= startindex; --g)
			Tmepindex.Add(clear[g]);
		startindex = clear.Num();

		TArray<FVector> rightPointlist = GetTopPointList(contentpoints, rightdir,clear, i.Value, i.Value.right,false);

		TArray<FVector>PointList,PointDir;

		for (int i = leftPointlist.Num() - 1; i >= 0; --i)
			PointList.Add(leftPointlist[i]);

		for (int i = leftdir.Num() - 1; i >= 0; --i)
			PointDir.Add(leftdir[i]);

		
		for (int g = startindex; g < clear.Num(); ++g)
			Tmepindex.Add(clear[g]);
		
		AuadrangleIndex[j] = Tmepindex;
		startindex = clear.Num();

		PointList += rightPointlist;
		PointDir += rightdir;
		OutForwardDir.Add(PointDir);

		TempList.Add(PointList);
	}


	return TempList;
}

TArray<FVector>  LayoutMath::GetTopPointList(TMap<int, Atbestline>& contentpoints, TArray<FVector>&forwarddir, TArray<int>& clear, Atbestline temp, int type, bool bIs)
{
	TArray<FVector> leftPointlist;
	if (type > -1)
	{

		FVector LastPos(0,0,0);
		while (true)
		{
			bool bIsford = bIs ? temp.leftbIs : temp.rightbIs;
			auto tptemp = *contentpoints.Find(type);

			FVector2D pos = FVector2D(temp.data.ForwordPos1.X, temp.data.ForwordPos1.Y);
			FLine line(tptemp.data.ForwordPos1, tptemp.data.ForwordPos2);
			int tp = GetOtherPointType(pos, line);

			//temp


			if (bIsford)
			{
				FVector tdir;
				auto tmp = bIs ? temp.data.ForwordPos1 : temp.data.ForwordPos2;
				
					if (tp == 1)
						 tdir = (tmp - tptemp.data.LastPos1).GetSafeNormal();
					else
						tdir = (tmp - tptemp.data.LastPos2).GetSafeNormal();
				
				forwarddir.Add(tdir);
				leftPointlist.Add(tmp);
			}
			else
			{
				FVector tmp;
				if (tp == 1)
				{
					leftPointlist.Add(tptemp.data.ForwordPos1);
					tmp = temp.data.LastPos2;
				}
				else if (tp == 2)
				{
					leftPointlist.Add(tptemp.data.ForwordPos2);
					tmp = temp.data.LastPos1;
				}
				

				FVector tdir = (leftPointlist.Last() - tmp).GetSafeNormal();
				forwarddir.Add(tdir);
			}

			if (tp == 1)
				type = tptemp.right;
			else if (tp == 2)
				type = tptemp.left;

			temp = tptemp;

			if (type == -1)
			{
				if (tp == 1)
				{
					leftPointlist.Add(tptemp.data.ForwordPos2);
					LastPos = tptemp.data.LastPos2;
					//leftPointlist.Add(tptemp.data.LastPos2);

				}
				else if (tp == 2)
				{
					leftPointlist.Add(tptemp.data.ForwordPos1);
					LastPos = tptemp.data.LastPos1;
					//leftPointlist.Add(tptemp.data.LastPos1);
				}
				FVector tdir = (leftPointlist.Last() - LastPos).GetSafeNormal();
				forwarddir.Add(tdir);

				clear.Add(tptemp.slef);
				break;
			}

			if (clear.Find(type) > -1)
			{
				clear.Add(temp.slef);
				break;
			}

			clear.Add(temp.slef);
		}

		leftPointlist.Add(LastPos);

	}
	else
	{
		if (bIs)
		{
			leftPointlist.Add(temp.data.ForwordPos1);
			leftPointlist.Add(temp.data.LastPos1);
		}
		else
		{
			leftPointlist.Add(temp.data.ForwordPos2);
			leftPointlist.Add(temp.data.LastPos2);
		}

		auto tdir = (leftPointlist[0] - leftPointlist[1]).GetSafeNormal();
		forwarddir.Add(tdir);
	} 

	return leftPointlist;
}



int LayoutMath::GetOtherPointType(FVector2D pos, FLine line)
{
	int type = -1;
	FVector2D out;
	GetSubPoint(pos, line, out);
	type = (line.Start - out).Size() < (line.End - out).Size() ? 1 : 2;

	return type;
}

bool LayoutMath::IsGuanlian(ExpansionAuadrangle  temp1, ExpansionAuadrangle  temp2)
{
	FLine templine1(temp1.ForwordPos1, temp1.ForwordPos2);
	FLine templine2(temp1.ForwordPos1, temp1.LastPos1);
	float len1 = templine1.GetLineLength() /2.f;
	auto pos1 = templine1.GetMiddlePoint() + (-templine2.GetNorm() * (templine2.GetLineLength() / 2.f));

	FLine templine3(temp2.ForwordPos1, temp2.ForwordPos2);
	FLine templine4(temp2.ForwordPos1, temp2.LastPos1);
	float len2 = templine3.GetLineLength() / 2.f;
	auto pos2 = templine3.GetMiddlePoint() + (-templine4.GetNorm() * (templine4.GetLineLength() / 2.f));

	auto dir1 = templine1.GetNorm();
	auto dir2 = templine3.GetNorm();
	dir1.Set(FMath::Abs(dir1.X), FMath::Abs(dir1.Y));
	dir2.Set(FMath::Abs(dir2.X), FMath::Abs(dir2.Y));
	

	float len3 = (pos1 - pos2).Size();
	
	return len3 <= (len1 + len2);
}

int LayoutMath::IsInsertPoint(FLine line1, FLine line2, ExpansionAuadrangle& data1, ExpansionAuadrangle& data2)
{
	float offset = 3.f,Templen;
	FVector2D out1, tempout;
	GetSubPoint(line1.Start, line2, out1);
	auto nor1 = line1.GetNorm();
	auto nor2 = line2.GetNorm();
	if (line1.Start.Equals(out1, 2.f) && ERRORBAND(nor1.X, nor2.X))
		return -1;

	if (line1.Start.Equals(out1, offset) && IsPointToLine(out1, line2))
		return 1;
	else if ((IsPointToLine(out1, line1) && (line1.Start - out1).Size() < 3.f))
	{
		Templen = (line1.Start - out1).Size();
		tempout = (out1 - line1.Start).GetSafeNormal() * Templen + FVector2D(data1.LastPos1.X, data1.LastPos1.Y);
		data1.LastPos1.X = tempout.X;
		data1.LastPos1.Y = tempout.Y;

		data1.ForwordPos1.X = out1.X;
		data1.ForwordPos1.Y = out1.Y;
		return 1;
	}

	GetSubPoint(line1.End, line2, out1);
	if (line1.End.Equals(out1, offset) && IsPointToLine(out1, line2))
		return 2;
	else if ((IsPointToLine(out1, line1) && (line1.End - out1).Size() < 3.f))
	{
		Templen = (line1.End - out1).Size();
		tempout = (out1 - line1.End).GetSafeNormal() * Templen + FVector2D(data1.LastPos2.X, data1.LastPos2.Y);
		data1.LastPos2.X = tempout.X;
		data1.LastPos2.Y = tempout.Y;

		data1.ForwordPos2.X = out1.X;
		data1.ForwordPos2.Y = out1.Y;
		return 2;
	}

	GetSubPoint(line2.Start, line1, out1);
	if ((line2.Start.Equals(out1, offset) && IsPointToLine(out1, line1)))
	{
		if((line1.Start - out1).Size() < (line1.End - out1).Size())
			return 3;
		else
			return 4;
	}
	else if ((IsPointToLine(out1, line2) && (line2.Start - out1).Size() < 3.f))
	{
		if ((line1.Start - out1).Size() < (line1.End - out1).Size())
		{
		/*	Templen = (line2.Start - out1).Size();
			tempout = (out1 - line2.Start).GetSafeNormal() * Templen + FVector2D(data2.LastPos1.X, data2.LastPos1.Y);
			data2.LastPos1.X = tempout.X;
			data2.LastPos1.Y = tempout.Y;

			data2.ForwordPos1.X = out1.X;
			data2.ForwordPos1.Y = out1.Y;*/

			Templen = (line2.Start - out1).Size();
			tempout = (out1 - line2.Start).GetSafeNormal() * Templen + FVector2D(data2.LastPos2.X, data2.LastPos2.Y);
			data2.LastPos2.X = tempout.X;
			data2.LastPos2.Y = tempout.Y;

			data2.ForwordPos2.X = out1.X;
			data2.ForwordPos2.Y = out1.Y;

			return 3;
		}
		else
		{
		/*	Templen = (line2.Start - out1).Size();
			tempout = (out1 - line2.Start).GetSafeNormal() * Templen + FVector2D(data2.LastPos2.X, data2.LastPos2.Y);
			data2.LastPos2.X = tempout.X;
			data2.LastPos2.Y = tempout.Y;

			data2.ForwordPos2.X = out1.X;
			data2.ForwordPos2.Y = out1.Y;*/

			Templen = (line2.Start - out1).Size();
			tempout = (out1 - line2.Start).GetSafeNormal() * Templen + FVector2D(data2.LastPos1.X, data2.LastPos1.Y);
			data2.LastPos1.X = tempout.X;
			data2.LastPos1.Y = tempout.Y;

			data2.ForwordPos1.X = out1.X;
			data2.ForwordPos1.Y = out1.Y;
			return 4;
		}
	}

	GetSubPoint(line2.End, line1, out1);
	if (line2.End.Equals(out1, offset) && IsPointToLine(out1, line1) )
	{
		
		if ((line1.Start - out1).Size() < (line1.End - out1).Size())
			return 3;
		else
			return 4;
	}
	else if ((IsPointToLine(out1, line2) && (line2.End - out1).Size() < 3.f))
	{
		if ((line1.Start - out1).Size() < (line1.End - out1).Size())
		{
		/*	Templen = (line2.End - out1).Size();
			tempout = (out1 - line2.End).GetSafeNormal() * Templen + FVector2D(data2.LastPos1.X, data2.LastPos1.Y);
			data2.LastPos1.X = tempout.X;
			data2.LastPos1.Y = tempout.Y;

			data2.ForwordPos1.X = out1.X;
			data2.ForwordPos1.Y = out1.Y;*/
			Templen = (line2.End - out1).Size();
			tempout = (out1 - line2.End).GetSafeNormal() * Templen + FVector2D(data2.LastPos2.X, data2.LastPos2.Y);
			data2.LastPos2.X = tempout.X;
			data2.LastPos2.Y = tempout.Y;

			data2.ForwordPos2.X = out1.X;
			data2.ForwordPos2.Y = out1.Y;

			return 3;
		}
		else
		{
			/*Templen = (line2.End - out1).Size();
			tempout = (out1 - line2.End).GetSafeNormal() * Templen + FVector2D(data2.LastPos2.X, data2.LastPos2.Y);
			data2.LastPos2.X = tempout.X;
			data2.LastPos2.Y = tempout.Y;

			data2.ForwordPos2.X = out1.X;
			data2.ForwordPos2.Y = out1.Y;*/

			Templen = (line2.End - out1).Size();
			tempout = (out1 - line2.End).GetSafeNormal() * Templen + FVector2D(data2.LastPos1.X, data2.LastPos1.Y);
			data2.LastPos1.X = tempout.X;
			data2.LastPos1.Y = tempout.Y;

			data2.ForwordPos1.X = out1.X;
			data2.ForwordPos1.Y = out1.Y;
			return 4;
		}
	}
	return -1;

}

void LayoutMath::RemovedPoint(TArray<TArray<FVector>>& PointList, const TArray<FExpansionLine>& WallList)
{
	for (int i = 0; i < PointList.Num(); ++i)
	{
		int endindex = PointList[i].Num() - 1;
		auto start = FVector2D(PointList[i][0].X, PointList[i][0].Y);
		auto end = FVector2D(PointList[i][endindex].X, PointList[i][endindex].Y);
		bool bIsStart = false, bIsEnd = false;
		for (auto j : WallList)
		{
			if (start.Equals(j.Start, 1.f) || start.Equals(j.End, 1.f))
				bIsStart = true;
			else if (end.Equals(j.Start, 1.f) || end.Equals(j.End, 1.f))
				bIsEnd = true;
		}
		if (bIsEnd)
		{
			auto dir = (PointList[i][endindex] - PointList[i][endindex - 1]).GetSafeNormal();
			
			PointList[i].RemoveAt(endindex);
			//PointList[i][endindex - 1] = PointList[i][endindex - 1] + dir * 2.4f;
		}
		else
		{
			auto dir = (PointList[i][endindex - 2] - PointList[i][endindex - 1]).GetSafeNormal();
			//PointList[i][endindex] = PointList[i][endindex] + dir * 1.65f;
			
		}

		if (bIsStart)
		{
			auto dir = (PointList[i][0] - PointList[i][1]).GetSafeNormal();
			
			PointList[i].RemoveAt(0);
			//PointList[i][0] = PointList[i][0] + dir * 2.4f;
		}
		else
		{
			auto dir = (PointList[i][2] - PointList[i][1]).GetSafeNormal();
			//PointList[i][0] = PointList[i][0] + dir * 1.65f;
			
		}
	}
}

void LayoutMath::RemovedPointfloor(TArray<TArray<FVector>>& PointList, const TArray <FExpansionLine>& WallList)
{
	TArray<TArray<FVector>>Dir;
	Dir.SetNum(PointList.Num());
	for (int i = 0; i < PointList.Num(); ++i)
	{
		int endindex = PointList[i].Num() - 1;
		auto start = FVector2D(PointList[i][0].X, PointList[i][0].Y);
		auto end = FVector2D(PointList[i][endindex].X, PointList[i][endindex].Y);
		bool bIsStart = false, bIsEnd = false;
		for (auto j : WallList)
		{
			if (start.Equals(j.Start, 1.f) || start.Equals(j.End, 1.f))
				bIsStart = true;
			else if (end.Equals(j.Start, 1.f) || end.Equals(j.End, 1.f))
				bIsEnd = true;
		}
		if (bIsEnd)
		{
			PointList[i].RemoveAt(endindex);

		}

		if (bIsStart)
		{
			PointList[i].RemoveAt(0);
		}


	}

}

void LayoutMath::MoveForwardPoints(TArray<TArray<FVector>>& forwardpoints, TArray<TArray<FVector>>& lastpoints, float length)
{
	for (int i = 0; i < forwardpoints.Num(); ++i)
	{
		auto temp = forwardpoints[i];
		//temp.RemoveAt(temp.Num() - 1);
		//temp.RemoveAt(0);
		TArray<FVector> Templast;
		for (int g = lastpoints[i].Num() - 1; g >= 0; --g)
			Templast.Add(lastpoints[i][g]);
		for (int j = 0; j < temp.Num(); ++j)
		{
			auto dir = (temp[j] - Templast[j]).GetSafeNormal();
			if (j > 0 && j < temp.Num() - 1)
			{
				auto len = FMath::Sqrt(length + length);
				forwardpoints[i][j + 1] = dir * len + forwardpoints[i][j + 1];
			}
			else
			forwardpoints[i][j] = dir * length + forwardpoints[i][j];
		}
		
	}
}

TArray<TArray<FVector>> LayoutMath::RemovedLastBreakwaterPoint(const TArray<TArray<FVector>>& ForwardList, const TArray<TArray<FVector>>& LastList,  const TArray<FExpansionLine>& WallList)
{
	TArray<TArray<FVector>>BreakwaterTemp;
	for (int i = 0; i < LastList.Num(); ++i)
	{
		TArray<FVector> Templist = LastList[i];
		int endindex = LastList[i].Num() - 1;
		auto start = FVector2D(LastList[i][0].X, LastList[i][0].Y);
		auto end = FVector2D(LastList[i][endindex].X, LastList[i][endindex].Y);
		bool bIsStart = false, bIsEnd = false, bIs = false;
		for (auto j : WallList)
		{
			if (start.Equals(j.Start, 3.f) || start.Equals(j.End, 3.f))
				bIsStart = true;
			else if (end.Equals(j.Start, 3.f) || end.Equals(j.End, 3.f))
				bIsEnd = true;
		}
		if (bIsEnd)
		{
			//Templist.RemoveAt(Templist.Num() - 1);
			if (Templist.Num() > 3)
				Templist.Insert(ForwardList[i][1],0);
			else
			{
				auto dir = (ForwardList[i][1] - Templist.Last()).GetSafeNormal();
				float len = GetAbs(dir.X) + GetAbs(dir.Y);
				if (ERRORBANDEX(len, 1.f,0.05f))
					Templist.Add(ForwardList[i][1]);
				else
				{
					Templist.Add(ForwardList[i][ForwardList[i].Num() - 2]);
					auto temptl = Templist;
					Templist.Empty();
					for (int len = temptl.Num() - 1; len >= 0; --len)
						Templist.Add(temptl[len]);
					bIs = true;
				}
			}
				//Templist.Insert(ForwardList[i][ForwardList[i].Num() - 2], 0);
		}
		if (bIsStart)
		{
			//Templist.RemoveAt(0);
			if (Templist.Num() > 4)
				Templist.Add(ForwardList[i][ForwardList[i].Num() - 2]);
			else
			{
				auto dir = (ForwardList[i][ForwardList[i].Num() - 2] - Templist[0]).GetSafeNormal();
				float len = GetAbs(dir.X) + GetAbs(dir.Y);
				if(len == 0.f)
					dir = (ForwardList[i][ForwardList[i].Num() - 3] - Templist[0]).GetSafeNormal();

				 len = GetAbs(dir.X) + GetAbs(dir.Y);
				if(ERRORBANDEX(len, 1.f, 0.05f))
					Templist.Insert(ForwardList[i][ForwardList[i].Num() - 2], 0);
				else
				{
					Templist.Insert(ForwardList[i][1], 0);
					auto temptl = Templist;
					Templist.Empty();
					for (int len = temptl.Num() - 1; len >= 0; --len)
						Templist.Add(temptl[len]);

					bIs = true;
				}

				
			}
				//Templist.Add(ForwardList[i][1]);
		}

	/*	if (Templist.Num() > LastList.Num())
		{
			TArray<FVector> relist1;
			auto relist2 = Templist;
			TArray<FVector> tell;
			tell.Add(Templist[0]);
			relist1.Add(Templist[0]);
			for (int i = 0; i < tell.Num(); ++i)
			{

				for (auto j : relist2)
				{
					if (relist1.Find(j) > -1)

						continue;
					if (ERRORBANDEX(j.X, tell[i].X, 3.f) || ERRORBANDEX(j.Y, tell[i].Y, 3.f))
					{
						auto temppos = j;
						if (ERRORBANDEX(j.X, tell[i].X, 3.f))
							temppos.X = tell[i].X;
						else
							temppos.Y = tell[i].Y;
						tell.Add(temppos);
						relist1.Add(j);
						break;
					}
				}
			}

			Templist = tell;
		}*/


		if ((Templist.Num() > 4 && !bIs))
		{
			auto tmlist = Templist;
			Templist.Empty();
			for (int i = tmlist.Num() - 1; i >= 0; --i)
				Templist.Add(tmlist[i]);
		}
		BreakwaterTemp.Add(Templist);
	
	}

	//for (int i = 0; i < LastList.Num(); ++i)
	//{
	//	TArray<FVector> Templist = LastList[i];
	//	int endindex = LastList[i].Num() - 1;
	//	auto start = FVector2D(LastList[i][0].X, LastList[i][0].Y);
	//	auto end = FVector2D(LastList[i][endindex].X, LastList[i][endindex].Y);
	//	bool bIsStart = false, bIsEnd = false;
	//	for (auto j : WallList)
	//	{
	//		if (start.Equals(j.Start, 3.f) || start.Equals(j.End, 3.f))
	//			bIsStart = true;
	//		else if (end.Equals(j.Start, 3.f) || end.Equals(j.End, 3.f))
	//			bIsEnd = true;
	//	}
	//	if (bIsEnd)
	//	{
	//		Templist.RemoveAt(Templist.Num() - 1);
	//	/*	if (Templist.Num() > 3)
	//			Templist.Insert(ForwardList[i][1], 0);
	//		else
	//			Templist.Add(ForwardList[i][1]);*/
	//		//Templist.Insert(ForwardList[i][ForwardList[i].Num() - 2], 0);
	//	}
	//	if (bIsStart)
	//	{
	//		Templist.RemoveAt(0);
	//	/*	if (Templist.Num() > 4)
	//			Templist.Add(ForwardList[i][ForwardList[i].Num() - 2]);
	//		else
	//			Templist.Insert(ForwardList[i][ForwardList[i].Num() - 2], 0);*/
	//		//Templist.Add(ForwardList[i][1]);
	//	}

	//
	//	BreakwaterTemp.Add(Templist);
	//}


	//BreakwaterTemp = Templist;
	return BreakwaterTemp;
}

bool LayoutMath::PillarsCount( TArray<FExpansionCabnetPillars>& Fluelist, FLine & line, TArray<FVector>& OutPointlist, bool& bIsStartClear, bool& bIsEndClear)
{
	bIsEndClear = false;
	bIsStartClear = false;

	if (Fluelist.Num() == 0)
		return false;

	TArray<float>Lens;
	TArray<FExpansionCabnetPillars>ConutPillars;
	for (int i = 0; i < Fluelist.Num(); ++i)
	{
		
		//FVector ti = FRotator(0, Fluelist[i].Angle,0).Vector();
		FVector2D movedir = -Fluelist[i].Dir;

		auto Size = GetPosToRotator(FRotator(0, Fluelist[i].Angle, 0), FVector(Fluelist[i].Size.X, Fluelist[i].Size.Y,0));
		GetABSvector(Size);
		Fluelist[i].Size.Set(Size.X, Size.Y);
		auto TempPos = movedir * FVector2D(Size.X, Size.Y) + Fluelist[i].Pos;
		if (IsPointToLine(TempPos, line))
		{
			auto length = (line.Start - TempPos).Size();
			if (Lens.Num() == 0)
			{
				Lens.Add(length);
				ConutPillars.Add(Fluelist[i]);
			}
			else
			{
				for (int j = 0; j < Lens.Num(); ++j)
				{
					if (length < Lens[j])
					{
						Lens.Insert(length,j);
						ConutPillars.Insert(Fluelist[i], j);
						break;
					}
				}
			}
		}
	}
	auto dir = line.GetNorm();
	for (auto i : ConutPillars)
	{
		//FVector ti = FRotator(0, 0, i.Angle).Vector();
		FVector2D movedir= - i.Dir;
		auto temps = movedir * i.Size  + i.Pos;
		auto last1 = dir * i.Size  + temps;
		auto forward1 = last1 + -movedir * (i.Size * 2.f);
		auto forward2 = forward1 + -dir * (i.Size * 2.f);
		auto last2 = forward2 + movedir * (i.Size * 2.f);
		OutPointlist.Add(FVector(last1.X, last1.Y,0));
		OutPointlist.Add(FVector(forward1.X, forward1.Y, 0));
		OutPointlist.Add(FVector(forward2.X, forward2.Y, 0));
		OutPointlist.Add(FVector(last2.X, last2.Y, 0));
	}

	if(OutPointlist.Num() == 0)
		return false;

	if (OutPointlist.Last().Equals(FVector(line.End.X, line.End.Y, 0), 1.f))
	{
		OutPointlist.RemoveAt(OutPointlist.Num() - 1);
		bIsEndClear = true;
	}
	if (OutPointlist[0].Equals(FVector(line.Start.X, line.Start.Y, 0), 1.f))
	{
		OutPointlist.RemoveAt(0);
		bIsStartClear = true;
	}

	return true;
}
FVector LayoutMath::GetDir(FVector other, FVector pos1, FVector pos2,TArray<FVector2D>& areas)
{
	TArray<FVector> dirlist;
	FVector dir1(0.5f, 0.5f, 0);
	FVector dir2(-0.5f, 0.5f, 0);
	FVector dir3(0.5f, -0.5f, 0);
	FVector dir4(-0.5f, -0.5f, 0);
	dirlist.Add(dir1);
	dirlist.Add(dir2);
	dirlist.Add(dir3);
	dirlist.Add(dir4);
	FVector Temp;
	FLine line1(pos1, pos2);
	line1 = line1.ShrinkLine(4.f);
	for (auto i : dirlist)
	{
		auto tpos = i * 10000.f + other;
		FLine line2(tpos, other);
		if (LineIntersect(line1, line2))
		{
			auto tpos = i * 2.f + other;
			FVector2D tmpespos(tpos.X, tpos.Y);
			if (ChekColseInterval(tmpespos, areas))
				Temp = i * -1.f;
			else
			    Temp = i;
			return Temp;
		}
	}

	return Temp;
}
//
//bool LayoutMath::IsPointAsLine(FVector2D & point, FLine & line)
//{
//	FVector2D outPos;
//	if (point.X == line.Start.X || point.Y == line.Start.Y)
//		outPos = point;
//	else
//		GetSubPoint(point, line, outPos);
//
//	float Length = line.GetLineLength();
//	float Slen = (outPos - line.Start).Size();
//	float Elen = (outPos - line.End).Size();
//
//	return ERRORBAND((Slen + Elen), Length);
//}



void  LayoutMath::GetSubPoint(FVector2D Pos, FLine line2, FVector2D& OutPos)
{
	FLine line1(line2.Start, Pos);
	float Dot = FVector2D::DotProduct(line1.GetNorm(), line2.GetNorm());
	OutPos = (line1.GetLineLength() * Dot) * -line2.GetNorm() + line2.Start;
}

bool LayoutMath::ChekColseInterval(const FVector2D & point, const TArray<FVector2D>& List)
{
	FVector2D Dir(0.f, 1.f);
	FLine Tmline(point, point + Dir * 10000.f);
	int AddIntersect = 0;
	for (int i = 0, Len = List.Num(); i < Len; ++i)
	{
		FLine Tl(List[i], List[(i + 1) % Len]);
		if (LineIntersect(Tl, Tmline))
			AddIntersect += 1;
	}

	return (AddIntersect % 2 == 1);
}

bool LayoutMath::LineIntersect(const FLine & line1, const FLine & line2)
{
	if (line1.Start == line2.End || line1.End == line2.Start || line1.Start == line2.Start || line1.End == line2.End)
		return false;
	if (Min(line1.Start.X, line1.End.X) <= Max(line2.Start.X, line2.End.X) &&
		Min(line2.Start.X, line2.End.X) <= Max(line1.Start.X, line1.End.X) &&
		Min(line1.Start.Y, line1.End.Y) <= Max(line2.Start.Y, line2.End.Y) &&
		Min(line2.Start.Y, line2.End.Y) <= Max(line1.Start.Y, line1.End.Y))
	{
		double u, v, w, z;
		u = (line2.Start.X - line1.Start.X)*(line1.End.Y - line1.Start.Y) - (line1.End.X - line1.Start.X)*(line2.Start.Y - line1.Start.Y);
		v = (line2.End.X - line1.Start.X)*(line1.End.Y - line1.Start.Y) - (line1.End.X - line1.Start.X)*(line2.End.Y - line1.Start.Y);
		w = (line1.Start.X - line2.Start.X)*(line2.End.Y - line2.Start.Y) - (line2.End.X - line2.Start.X)*(line1.Start.Y - line2.Start.Y);
		z = (line1.End.X - line2.Start.X)*(line2.End.Y - line2.Start.Y) - (line2.End.X - line2.Start.X)*(line1.End.Y - line2.Start.Y);
		if (u == 0.f && v == 0.f && w == 0.f && z == 0.f)
			return false;

		return (u*v <= 0.00000001 && w*z <= 0.00000001);
	}
	return false;
}

float LayoutMath::Min(float a, float b)
{
	return a < b ? a : b;
}

float LayoutMath::Max(float a, float b)
{
	return a > b ? a : b;
}

TArray<FVector2D> LayoutMath::FVector3DtoFvector2D(const TArray<FVector>& List)
{
	TArray<FVector2D> Templist;
	for (int i = 0; i < List.Num(); ++i)
		Templist.Add(FVector2D(List[i].X, List[i].Y));

	return Templist;
}
