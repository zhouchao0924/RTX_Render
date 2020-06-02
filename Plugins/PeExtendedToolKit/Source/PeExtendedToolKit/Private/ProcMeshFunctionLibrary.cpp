#pragma once

#include "ProcMeshFunctionLibrary.h"
#include "PeExtendedToolKitPrivatePCH.h"
#include "PeExtendedToolKit.h"
#include "FileHelper.h"

FProcMeshConfig UProcMeshFunctionLibrary::GetProcMeshSection(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex)
{
	FProcMeshConfig tempproc;
 
	//判断是否超出范围
	if (SectionIndex >= ProcMeshComponent->GetNumSections())
	{
		return tempproc;
	}
	else
	{
		if (ProcMeshComponent->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num() >= 1)
		{
			FProcMeshSection* Section = nullptr;//定义Section

			 //判断SectionIndex是否超出范围，如果超出则设置为最大值。如果没超出则直接设置为SectionIndex的值
			if (SectionIndex < ProcMeshComponent->GetNumSections())
			{
				Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//初始化指定部分结构体(部分信息数组)
			}
			else
			{
				Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);//初始化指定部分结构体(部分信息数组)
			}



			FProcMeshVertex* ModifyVert = 0;//定义ModifyVert并初始化(顶点信息数组)
			uint32* Triang = 0;//定义Triang并初始化(三角面数组)

			int32 VerticeNum = Section->ProcVertexBuffer.Num(); //获得顶点数量
			int32 TrianglesNum = Section->ProcIndexBuffer.Num();//获得三角面数量

			//初始化数组数量

			tempproc.Vertices.SetNum(VerticeNum, false);
			tempproc.Normals.SetNum(VerticeNum, false);
			tempproc.UV0.SetNum(VerticeNum, false);
			//	tempproc.VertexColors.SetNum(VerticeNum, false);
			tempproc.Tangents.SetNum(VerticeNum, false);
			tempproc.Triangles.SetNum(TrianglesNum, false);


			//设置Vertices数组信息
			for (int LoopID = 0; LoopID < VerticeNum; LoopID = LoopID++)
			{
				ModifyVert = &Section->ProcVertexBuffer[LoopID]; //对ModifyVert赋指定ID的值
																 //设置数组id的值
				tempproc.Vertices[LoopID] = ModifyVert->Position;
				tempproc.Normals[LoopID] = ModifyVert->Normal;
				tempproc.UV0[LoopID] = ModifyVert->UV0;
				//	tempproc.VertexColors[LoopID] = ModifyVert->Color;
				tempproc.Tangents[LoopID] = ModifyVert->Tangent;
			}

			//设置Triangles数组信息
			for (int ID = 0; ID < TrianglesNum; ID = ID++)
			{
				Triang = &Section->ProcIndexBuffer[ID]; //对Triang赋指定ID的值

				tempproc.Triangles[ID] = *Triang;//设置数组id的值
			}

			return tempproc;
		}
	}
	return tempproc;
}














FProcMeshConfig GetConfig(TArray<FProcMeshConfig> config, int32 id)
{
	FProcMeshConfig nul;
	if (id >= config.Num())
	{
		return nul;
	}
	else
	{
		return config[id];
	}

}


void UProcMeshFunctionLibrary::MergeProceduralMesh(UProceduralMeshComponent* ProceduralMesh, UProceduralMeshComponent* Target, FVector Offset, bool CreateCollision)
{

	TArray<FProcMeshConfig> Config01; //原始信息
	TArray<FProcMeshConfig> Config;

	TArray<FColor> VertexColors;
	FProcMeshConfig temp;

	//初始化原始信息
	for (int i = 0; i < ProceduralMesh->GetNumSections(); i++)
	{
		Config01.Add(GetProcMeshSection(ProceduralMesh, i));
	}

	//初始化合并目标信息
	for (int i = 0; i < Target->GetNumSections(); i++)
	{
		Config.Add(GetProcMeshSection(Target, i));
	}



	Target->DestroyComponent();
	ProceduralMesh->ClearAllMeshSections();

	for (int i = 0; i < FMath::Max(Config01.Num(), Config.Num()); i++)
	{
		temp = MergeProceduralMeshConfig(GetConfig(Config01, i), GetConfig(Config, i), Offset);
		ProceduralMesh->CreateMeshSection(i, temp.Vertices, temp.Triangles, temp.Normals, temp.UV0, VertexColors, temp.Tangents, CreateCollision);


	}

}



FProcMeshConfig UProcMeshFunctionLibrary::MergeProceduralMeshConfig(FProcMeshConfig Target, FProcMeshConfig Parameter, FVector Offset)
{
	int32 length;
	FProcMeshConfig TempProcMesh;
	length = Target.Vertices.Num();
	TempProcMesh = Target;

	for (int i = 0; i < Parameter.Vertices.Num(); i++)
	{
		if(Parameter.Vertices.IsValidIndex(i))
		{
			TempProcMesh.Vertices.Add(Parameter.Vertices[i] + Offset);//顶点
		}
		if(Parameter.Normals.IsValidIndex(i))
		{
			TempProcMesh.Normals.Add(Parameter.Normals[i]);//法线
		}
		if(Parameter.UV0.IsValidIndex(i))
		{
			TempProcMesh.UV0.Add(Parameter.UV0[i]);//UV
		}
		if(Parameter.Tangents.IsValidIndex(i))
		{
			TempProcMesh.Tangents.Add(Parameter.Tangents[i]);//切线
		}
 
	}
	//三角面
	for (int i = 0; i < Parameter.Triangles.Num(); i++)
	{
		if(Parameter.Triangles.IsValidIndex(i))
		{
			TempProcMesh.Triangles.Add(Parameter.Triangles[i] + length);//三角面
		}
	}

	return TempProcMesh;
}




//获取ProceduralMesh的顶点、法线、UV、切线列表
void UProcMeshFunctionLibrary::GetProceduralMeshVertexList(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents, bool& bCreateCollision, bool& SectionVisible)
{

	//判断是否超出范围
	if (ProcMeshComponent->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num() >= 1)
	{

		FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(0);//定义Section

																			 //判断SectionIndex是否超出范围，如果超出则设置为最大值。如果没超出则直接设置为SectionIndex的值

		if (SectionIndex < ProcMeshComponent->GetNumSections())
		{
			Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//初始化指定部分结构体(部分信息数组)
		}
		else
		{
			Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);//初始化指定部分结构体(部分信息数组)
		}

		FProcMeshVertex* ModifyVert = &Section->ProcVertexBuffer[0];//定义ModifyVert并初始化(顶点信息数组)
		uint32* Triang = &Section->ProcIndexBuffer[0];//定义Triang并初始化(三角面数组)

		int32 VerticeNum = Section->ProcVertexBuffer.Num(); //获得顶点数量
		int32 TrianglesNum = Section->ProcIndexBuffer.Num();//获得三角面数量

															//初始化数组数量

		Vertices.SetNum(VerticeNum, false);
		Normals.SetNum(VerticeNum, false);
		UV0.SetNum(VerticeNum, false);
		VertexColors.SetNum(VerticeNum, false);
		Tangents.SetNum(VerticeNum, false);
		Triangles.SetNum(TrianglesNum, false);

		//设置Vertices数组信息
		for (int LoopID = 0; LoopID <VerticeNum; LoopID = LoopID++)
		{
			ModifyVert = &Section->ProcVertexBuffer[LoopID]; //对ModifyVert赋指定ID的值
															 //设置数组id的值
			Vertices[LoopID] = ModifyVert->Position;
			Normals[LoopID] = ModifyVert->Normal;
			UV0[LoopID] = ModifyVert->UV0;
			VertexColors[LoopID] = ModifyVert->Color;
			Tangents[LoopID] = ModifyVert->Tangent;
		}

		//设置Triangles数组信息
		for (int ID = 0; ID <TrianglesNum; ID = ID++)
		{
			Triang = &Section->ProcIndexBuffer[ID]; //对Triang赋指定ID的值

			Triangles[ID] = *Triang;//设置数组id的值
		}

		bCreateCollision = Section->bEnableCollision;
		SectionVisible = Section->bSectionVisible;
	}
}
//获取ProceduralMesh的指定顶点、法线、UV、切线 信息
void UProcMeshFunctionLibrary::GetProceduralMeshVertexIndex(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index, FVector &Vertices, FVector& Normals, FVector2D& UV0, FColor& VertexColors, FProcMeshTangent& Tangents, bool& EnableCollision, bool& SectionVisible)
{
	FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//定义Section ，并初始化
	FProcMeshVertex* ModifyVert = &Section->ProcVertexBuffer[Index];//定义ModifyVert并初始化ModifyVert
	Vertices = ModifyVert->Position;//顶点
	Normals = ModifyVert->Normal;//法线
	UV0 = ModifyVert->UV0;//UV
	VertexColors = ModifyVert->Color;//顶点颜色
	Tangents = ModifyVert->Tangent;//切线
	EnableCollision = Section->bEnableCollision;
	SectionVisible = Section->bSectionVisible;
}


//获取ProceduralMesh的指定三角面信息
int32 UProcMeshFunctionLibrary::GetProceduralMeshTriangles(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index)
{
	FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(0);//获取指定部分结构体

																		 //判断SectionIndex是否超出范围，如果超出则设置为最大值。如果没超出则直接设置为SectionIndex的值
	if (SectionIndex < ProcMeshComponent->GetNumSections())
	{
		Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);
	}

	uint32* Triang = &Section->ProcIndexBuffer[Index];//定义Triang
	return *Triang;//三角面
}

//获取顶点数量
int32 UProcMeshFunctionLibrary::GetProceduralMeshNum_Vertice(UProceduralMeshComponent* ProcMesh, int32 SectionIndex)
{
	FProcMeshSection* Section = ProcMesh->GetProcMeshSection(0);//定义Section

																//判断SectionIndex是否超出范围，如果超出则设置为最大值。如果没超出则直接设置为SectionIndex的值
	if (SectionIndex < ProcMesh->GetNumSections())
	{
		Section = ProcMesh->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMesh->GetProcMeshSection(ProcMesh->GetNumSections() - 1);
	}
	return Section->ProcVertexBuffer.Num() - 1;//获取顶点数量
}

//获取三角面数量
int32 UProcMeshFunctionLibrary::GetProceduralMeshNum_Triangles(UProceduralMeshComponent* ProcMesh, int32 SectionIndex)
{
	FProcMeshSection* Section = ProcMesh->GetProcMeshSection(0);//定义Section

																//判断SectionIndex是否超出范围，如果超出则设置为最大值。如果没超出则直接设置为SectionIndex的值
	if (SectionIndex < ProcMesh->GetNumSections())
	{
		Section = ProcMesh->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMesh->GetProcMeshSection(ProcMesh->GetNumSections() - 1);
	}

	return Section->ProcIndexBuffer.Num() - 1;//获取三角面数量
}




//将字符串转化为数组
TArray<FString> GetStringArray(const FString& SourceString)
{
	TArray<FString> SeparatedChars;

	if (!SourceString.IsEmpty())
	{
		for (auto CharIt(SourceString.CreateConstIterator()); CharIt; ++CharIt)
		{
			TCHAR Char = *CharIt;
			SeparatedChars.Add(FString(1, &Char));
		}

		// Remove the null terminator on the end
		SeparatedChars.RemoveAt(SeparatedChars.Num() - 1, 1);
	}

	return SeparatedChars;
}

//分割文本
TArray<FString>ParseIntoArray(const FString& SourceString, const FString& Delimiter, const bool CullEmptyStrings)
{
	TArray<FString> SeparatedStrings;
	const int32 nArraySize = SourceString.ParseIntoArray(SeparatedStrings, *Delimiter, CullEmptyStrings);
	return SeparatedStrings;
}

//载入文本
bool MLoadString(FString& Contents, FString FullFilePath)
{
	FString File;
	if (FullFilePath == "" || FullFilePath == " ") return false;
	if (!FFileHelper::LoadFileToString(File, *FullFilePath, FFileHelper::EHashOptions::None))
	{
		return false;
	}
	Contents = File;
	return true;
}
//获取指定字符数量
int32 UProcMeshFunctionLibrary::SpecifiedCharacterNumber(FString InString, FString Character)
{
	int32 C = 0;
	TArray<FString>R;
	R = GetStringArray(InString);
	for (int i = 0; i < R.Num() - 1; i++)
	{
		if (R[i] == Character)
		{
			C++;
		}
	}
	return C;
}


int32 FindSubstring(const FString& SearchIn, const FString& Substring, bool bUseCase, bool bSearchFromEnd, int32 StartPosition)
{
	ESearchCase::Type Case = bUseCase ? ESearchCase::CaseSensitive : ESearchCase::IgnoreCase;
	ESearchDir::Type Dir = bSearchFromEnd ? ESearchDir::FromEnd : ESearchDir::FromStart;

	return SearchIn.Find(Substring, Case, Dir, StartPosition);
}
bool StringContains(const FString& SearchIn, const FString& Substring, bool bUseCase, bool bSearchFromEnd)
{
	return FindSubstring(SearchIn, Substring, bUseCase, bSearchFromEnd, 0) != -1;
}

bool UProcMeshFunctionLibrary::ObjModeIsTriangular(FString Path)
{
	FString ObjText;//obj文本
	TArray<FString> Cont;//obj分行数组
	MLoadString(ObjText, Path);	//载入obj文本
	Cont = ParseIntoArray(ObjText, "\n", true);//分行储存

	for (int i = 0; i < Cont.Num() - 1; i++)
	{
		if (StringContains(Cont[i], "/", false, false))
		{
			if (SpecifiedCharacterNumber(Cont[i], "/")>6)
			{
				return false;
			}
			else
			{
				return true;
			}
			break;
		}
	}

	return false;
}

TArray<FProcMeshConfig>UProcMeshFunctionLibrary::InputObjMode(FString Path, bool MirrorZ)
{
	FString MtlText;

	FString SLeft;

	Path.Split(".", &SLeft, NULL, ESearchCase::CaseSensitive, ESearchDir::FromEnd);

	MLoadString(MtlText, SLeft + ".mtl");//载入Mtl


	FString ObjText;//obj文本
	TArray<FString> Cont;//obj分行数组
	MLoadString(ObjText, Path);	//载入obj文本
	Cont = ParseIntoArray(ObjText, "\n", true);//分行储存




	FVector RVE;
	FVector2D RUV;

	if (MirrorZ)
	{
		RVE = FVector(-1, 1, 1);
		RUV = FVector2D(1, -1);
	}
	else
	{
		RVE = FVector(-1, 1, -1);
		RUV = FVector2D(1, 1);
	}

	TArray<FVector> Verlist;
	TArray<FVector> Normal;
	TArray<FVector2D> UV;

	TArray<FProcMeshConfig>out;

	for (int i = 0; i < Cont.Num() - 1; i++)
	{
		FString LeftS;
		FString RightS;

		if (Cont[i].Split("v ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//顶点
		{
			TArray<FString>Value;
			//分割三个值
			Value = ParseIntoArray(RightS, " ", true);
			Verlist.Add(FVector(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]), FCString::Atof(*Value[2]))*RVE);
			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("v %s"), *RightS);
		}

		if (Cont[i].Split("vn ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//法线
		{
			TArray<FString>Value;
			//分割三个值
			Value = ParseIntoArray(RightS, " ", true);
			Normal.Add(FVector(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]), FCString::Atof(*Value[2]))*RVE);
			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("vn %s"), *RightS);
		}
		if (Cont[i].Split("vt ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//纹理坐标
		{
			TArray<FString>Value;
			//分割三个值
			Value = ParseIntoArray(RightS, " ", true);
			UV.Add(FVector2D(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]))*RUV);

			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("vt %s"), *RightS);
		}
		if (Cont[i].Split("g ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//分割obj当前行内容
		{
			FString MatName;
			Cont[i + 1].Split("usemtl ", NULL, &MatName, ESearchCase::CaseSensitive, ESearchDir::FromStart);
			FString CS;
			MtlText.Split("newmtl " + MatName, NULL, &CS, ESearchCase::CaseSensitive, ESearchDir::FromStart);
			FString CS2;
			CS.Split("map_Ka ", NULL, &CS2, ESearchCase::CaseSensitive, ESearchDir::FromStart);
			FString zout;
			CS2.Split("\r\n", &zout, NULL, ESearchCase::CaseSensitive, ESearchDir::FromStart);

			FProcMeshConfig Config;
			Config.MaterialName = zout;
			out.Add(Config);
		}

		if (Cont[i].Split("f ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//分割obj当前行内容
		{
			TArray<FString> m;
			m = ParseIntoArray(RightS, " ", false); //分割空格
			for (int p = 0; p < m.Num() - 1; p++) //三个顶点循环
			{
				int ID;
				if (MirrorZ)	ID = p; else ID = 2 - p;
				TArray<FString> value;

				if (StringContains(m[ID], "/", false, false))
				{
					value = ParseIntoArray(m[ID], "/", false);
					if (value.Num() > -1 && value[0] != "") out[out.Num() - 1].Vertices.Add(Verlist[FCString::Atoi(*value[0]) - 1]);//ADD顶点
					if (value.Num() > -1 && value[2] != "") out[out.Num() - 1].Normals.Add(Normal[FCString::Atoi(*value[2]) - 1]);//ADD法线
					if (value.Num() > -1 && value[1] != "") out[out.Num() - 1].UV0.Add(UV[FCString::Atoi(*value[1]) - 1]);//ADD UV
					out[out.Num() - 1].Triangles.Add(out[out.Num() - 1].Normals.Num() - 1);
				}
			}
		}
	}
	return out;
}







