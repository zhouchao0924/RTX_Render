#pragma once

#include "ProcMeshFunctionLibrary.h"
#include "PeExtendedToolKitPrivatePCH.h"
#include "PeExtendedToolKit.h"
#include "FileHelper.h"

FProcMeshConfig UProcMeshFunctionLibrary::GetProcMeshSection(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex)
{
	FProcMeshConfig tempproc;
 
	//�ж��Ƿ񳬳���Χ
	if (SectionIndex >= ProcMeshComponent->GetNumSections())
	{
		return tempproc;
	}
	else
	{
		if (ProcMeshComponent->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num() >= 1)
		{
			FProcMeshSection* Section = nullptr;//����Section

			 //�ж�SectionIndex�Ƿ񳬳���Χ���������������Ϊ���ֵ�����û������ֱ������ΪSectionIndex��ֵ
			if (SectionIndex < ProcMeshComponent->GetNumSections())
			{
				Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//��ʼ��ָ�����ֽṹ��(������Ϣ����)
			}
			else
			{
				Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);//��ʼ��ָ�����ֽṹ��(������Ϣ����)
			}



			FProcMeshVertex* ModifyVert = 0;//����ModifyVert����ʼ��(������Ϣ����)
			uint32* Triang = 0;//����Triang����ʼ��(����������)

			int32 VerticeNum = Section->ProcVertexBuffer.Num(); //��ö�������
			int32 TrianglesNum = Section->ProcIndexBuffer.Num();//�������������

			//��ʼ����������

			tempproc.Vertices.SetNum(VerticeNum, false);
			tempproc.Normals.SetNum(VerticeNum, false);
			tempproc.UV0.SetNum(VerticeNum, false);
			//	tempproc.VertexColors.SetNum(VerticeNum, false);
			tempproc.Tangents.SetNum(VerticeNum, false);
			tempproc.Triangles.SetNum(TrianglesNum, false);


			//����Vertices������Ϣ
			for (int LoopID = 0; LoopID < VerticeNum; LoopID = LoopID++)
			{
				ModifyVert = &Section->ProcVertexBuffer[LoopID]; //��ModifyVert��ָ��ID��ֵ
																 //��������id��ֵ
				tempproc.Vertices[LoopID] = ModifyVert->Position;
				tempproc.Normals[LoopID] = ModifyVert->Normal;
				tempproc.UV0[LoopID] = ModifyVert->UV0;
				//	tempproc.VertexColors[LoopID] = ModifyVert->Color;
				tempproc.Tangents[LoopID] = ModifyVert->Tangent;
			}

			//����Triangles������Ϣ
			for (int ID = 0; ID < TrianglesNum; ID = ID++)
			{
				Triang = &Section->ProcIndexBuffer[ID]; //��Triang��ָ��ID��ֵ

				tempproc.Triangles[ID] = *Triang;//��������id��ֵ
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

	TArray<FProcMeshConfig> Config01; //ԭʼ��Ϣ
	TArray<FProcMeshConfig> Config;

	TArray<FColor> VertexColors;
	FProcMeshConfig temp;

	//��ʼ��ԭʼ��Ϣ
	for (int i = 0; i < ProceduralMesh->GetNumSections(); i++)
	{
		Config01.Add(GetProcMeshSection(ProceduralMesh, i));
	}

	//��ʼ���ϲ�Ŀ����Ϣ
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
			TempProcMesh.Vertices.Add(Parameter.Vertices[i] + Offset);//����
		}
		if(Parameter.Normals.IsValidIndex(i))
		{
			TempProcMesh.Normals.Add(Parameter.Normals[i]);//����
		}
		if(Parameter.UV0.IsValidIndex(i))
		{
			TempProcMesh.UV0.Add(Parameter.UV0[i]);//UV
		}
		if(Parameter.Tangents.IsValidIndex(i))
		{
			TempProcMesh.Tangents.Add(Parameter.Tangents[i]);//����
		}
 
	}
	//������
	for (int i = 0; i < Parameter.Triangles.Num(); i++)
	{
		if(Parameter.Triangles.IsValidIndex(i))
		{
			TempProcMesh.Triangles.Add(Parameter.Triangles[i] + length);//������
		}
	}

	return TempProcMesh;
}




//��ȡProceduralMesh�Ķ��㡢���ߡ�UV�������б�
void UProcMeshFunctionLibrary::GetProceduralMeshVertexList(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FLinearColor>& VertexColors, TArray<FProcMeshTangent>& Tangents, bool& bCreateCollision, bool& SectionVisible)
{

	//�ж��Ƿ񳬳���Χ
	if (ProcMeshComponent->GetProcMeshSection(SectionIndex)->ProcVertexBuffer.Num() >= 1)
	{

		FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(0);//����Section

																			 //�ж�SectionIndex�Ƿ񳬳���Χ���������������Ϊ���ֵ�����û������ֱ������ΪSectionIndex��ֵ

		if (SectionIndex < ProcMeshComponent->GetNumSections())
		{
			Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//��ʼ��ָ�����ֽṹ��(������Ϣ����)
		}
		else
		{
			Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);//��ʼ��ָ�����ֽṹ��(������Ϣ����)
		}

		FProcMeshVertex* ModifyVert = &Section->ProcVertexBuffer[0];//����ModifyVert����ʼ��(������Ϣ����)
		uint32* Triang = &Section->ProcIndexBuffer[0];//����Triang����ʼ��(����������)

		int32 VerticeNum = Section->ProcVertexBuffer.Num(); //��ö�������
		int32 TrianglesNum = Section->ProcIndexBuffer.Num();//�������������

															//��ʼ����������

		Vertices.SetNum(VerticeNum, false);
		Normals.SetNum(VerticeNum, false);
		UV0.SetNum(VerticeNum, false);
		VertexColors.SetNum(VerticeNum, false);
		Tangents.SetNum(VerticeNum, false);
		Triangles.SetNum(TrianglesNum, false);

		//����Vertices������Ϣ
		for (int LoopID = 0; LoopID <VerticeNum; LoopID = LoopID++)
		{
			ModifyVert = &Section->ProcVertexBuffer[LoopID]; //��ModifyVert��ָ��ID��ֵ
															 //��������id��ֵ
			Vertices[LoopID] = ModifyVert->Position;
			Normals[LoopID] = ModifyVert->Normal;
			UV0[LoopID] = ModifyVert->UV0;
			VertexColors[LoopID] = ModifyVert->Color;
			Tangents[LoopID] = ModifyVert->Tangent;
		}

		//����Triangles������Ϣ
		for (int ID = 0; ID <TrianglesNum; ID = ID++)
		{
			Triang = &Section->ProcIndexBuffer[ID]; //��Triang��ָ��ID��ֵ

			Triangles[ID] = *Triang;//��������id��ֵ
		}

		bCreateCollision = Section->bEnableCollision;
		SectionVisible = Section->bSectionVisible;
	}
}
//��ȡProceduralMesh��ָ�����㡢���ߡ�UV������ ��Ϣ
void UProcMeshFunctionLibrary::GetProceduralMeshVertexIndex(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index, FVector &Vertices, FVector& Normals, FVector2D& UV0, FColor& VertexColors, FProcMeshTangent& Tangents, bool& EnableCollision, bool& SectionVisible)
{
	FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);//����Section ������ʼ��
	FProcMeshVertex* ModifyVert = &Section->ProcVertexBuffer[Index];//����ModifyVert����ʼ��ModifyVert
	Vertices = ModifyVert->Position;//����
	Normals = ModifyVert->Normal;//����
	UV0 = ModifyVert->UV0;//UV
	VertexColors = ModifyVert->Color;//������ɫ
	Tangents = ModifyVert->Tangent;//����
	EnableCollision = Section->bEnableCollision;
	SectionVisible = Section->bSectionVisible;
}


//��ȡProceduralMesh��ָ����������Ϣ
int32 UProcMeshFunctionLibrary::GetProceduralMeshTriangles(UProceduralMeshComponent* ProcMeshComponent, int32 SectionIndex, int32 Index)
{
	FProcMeshSection* Section = ProcMeshComponent->GetProcMeshSection(0);//��ȡָ�����ֽṹ��

																		 //�ж�SectionIndex�Ƿ񳬳���Χ���������������Ϊ���ֵ�����û������ֱ������ΪSectionIndex��ֵ
	if (SectionIndex < ProcMeshComponent->GetNumSections())
	{
		Section = ProcMeshComponent->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMeshComponent->GetProcMeshSection(ProcMeshComponent->GetNumSections() - 1);
	}

	uint32* Triang = &Section->ProcIndexBuffer[Index];//����Triang
	return *Triang;//������
}

//��ȡ��������
int32 UProcMeshFunctionLibrary::GetProceduralMeshNum_Vertice(UProceduralMeshComponent* ProcMesh, int32 SectionIndex)
{
	FProcMeshSection* Section = ProcMesh->GetProcMeshSection(0);//����Section

																//�ж�SectionIndex�Ƿ񳬳���Χ���������������Ϊ���ֵ�����û������ֱ������ΪSectionIndex��ֵ
	if (SectionIndex < ProcMesh->GetNumSections())
	{
		Section = ProcMesh->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMesh->GetProcMeshSection(ProcMesh->GetNumSections() - 1);
	}
	return Section->ProcVertexBuffer.Num() - 1;//��ȡ��������
}

//��ȡ����������
int32 UProcMeshFunctionLibrary::GetProceduralMeshNum_Triangles(UProceduralMeshComponent* ProcMesh, int32 SectionIndex)
{
	FProcMeshSection* Section = ProcMesh->GetProcMeshSection(0);//����Section

																//�ж�SectionIndex�Ƿ񳬳���Χ���������������Ϊ���ֵ�����û������ֱ������ΪSectionIndex��ֵ
	if (SectionIndex < ProcMesh->GetNumSections())
	{
		Section = ProcMesh->GetProcMeshSection(SectionIndex);
	}
	else
	{
		Section = ProcMesh->GetProcMeshSection(ProcMesh->GetNumSections() - 1);
	}

	return Section->ProcIndexBuffer.Num() - 1;//��ȡ����������
}




//���ַ���ת��Ϊ����
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

//�ָ��ı�
TArray<FString>ParseIntoArray(const FString& SourceString, const FString& Delimiter, const bool CullEmptyStrings)
{
	TArray<FString> SeparatedStrings;
	const int32 nArraySize = SourceString.ParseIntoArray(SeparatedStrings, *Delimiter, CullEmptyStrings);
	return SeparatedStrings;
}

//�����ı�
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
//��ȡָ���ַ�����
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
	FString ObjText;//obj�ı�
	TArray<FString> Cont;//obj��������
	MLoadString(ObjText, Path);	//����obj�ı�
	Cont = ParseIntoArray(ObjText, "\n", true);//���д���

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

	MLoadString(MtlText, SLeft + ".mtl");//����Mtl


	FString ObjText;//obj�ı�
	TArray<FString> Cont;//obj��������
	MLoadString(ObjText, Path);	//����obj�ı�
	Cont = ParseIntoArray(ObjText, "\n", true);//���д���




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

		if (Cont[i].Split("v ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//����
		{
			TArray<FString>Value;
			//�ָ�����ֵ
			Value = ParseIntoArray(RightS, " ", true);
			Verlist.Add(FVector(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]), FCString::Atof(*Value[2]))*RVE);
			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("v %s"), *RightS);
		}

		if (Cont[i].Split("vn ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//����
		{
			TArray<FString>Value;
			//�ָ�����ֵ
			Value = ParseIntoArray(RightS, " ", true);
			Normal.Add(FVector(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]), FCString::Atof(*Value[2]))*RVE);
			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("vn %s"), *RightS);
		}
		if (Cont[i].Split("vt ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//��������
		{
			TArray<FString>Value;
			//�ָ�����ֵ
			Value = ParseIntoArray(RightS, " ", true);
			UV.Add(FVector2D(FCString::Atof(*Value[0]), FCString::Atof(*Value[1]))*RUV);

			//	UE_LOG(LogBlueprintUserMessages, Warning, TEXT("vt %s"), *RightS);
		}
		if (Cont[i].Split("g ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//�ָ�obj��ǰ������
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

		if (Cont[i].Split("f ", &LeftS, &RightS, ESearchCase::CaseSensitive, ESearchDir::FromStart))//�ָ�obj��ǰ������
		{
			TArray<FString> m;
			m = ParseIntoArray(RightS, " ", false); //�ָ�ո�
			for (int p = 0; p < m.Num() - 1; p++) //��������ѭ��
			{
				int ID;
				if (MirrorZ)	ID = p; else ID = 2 - p;
				TArray<FString> value;

				if (StringContains(m[ID], "/", false, false))
				{
					value = ParseIntoArray(m[ID], "/", false);
					if (value.Num() > -1 && value[0] != "") out[out.Num() - 1].Vertices.Add(Verlist[FCString::Atoi(*value[0]) - 1]);//ADD����
					if (value.Num() > -1 && value[2] != "") out[out.Num() - 1].Normals.Add(Normal[FCString::Atoi(*value[2]) - 1]);//ADD����
					if (value.Num() > -1 && value[1] != "") out[out.Num() - 1].UV0.Add(UV[FCString::Atoi(*value[1]) - 1]);//ADD UV
					out[out.Num() - 1].Triangles.Add(out[out.Num() - 1].Normals.Num() - 1);
				}
			}
		}
	}
	return out;
}







