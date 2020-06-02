// Copyright© 2017 ihomefnt All Rights Reserved.

#include "ObjFileAnsyTask.h"
//#include "Resource.h"
#include "ModelFile.h"
#include <fstream>
#include <iosfwd>
#include "AJHttpUploader.h"

ObjFileAnsyTask::ObjFileAnsyTask()
	:ResMgr(NULL)
	, eTaskState(ETaskState::_NONEState)
	, ModelFile(NULL)
{
	ObjFileNamePath.Empty();
}

ObjFileAnsyTask::ObjFileAnsyTask(UResourceMgr& Mgr,FString  filePath)
{
	ResMgr = &Mgr;
	eTaskState = ETaskState::_BacthCreate;
	ObjFileNamePath = filePath;
}

ObjFileAnsyTask::ObjFileAnsyTask(UModelFile* model, FString  filePath)
{
	eTaskState = ETaskState::_OneCreate;
	ModelFile = model;
	ObjFileNamePath = filePath;
}

//ObjFileAnsyTask::ObjFileAnsyTask(FString& OneObjFileName, FString& url)
//{
//	OneObjFileNameToUpload = OneObjFileName;
//	TaskUrl = url;
//}

ObjFileAnsyTask::~ObjFileAnsyTask()
{
	UE_LOG(LogTemp, Log,TEXT("ObjFileAnsyTask is Destroy"));
}

void ObjFileAnsyTask::DoWork()
{
	GLog->Log("--------------------------------------------------------------------");
	GLog->Log("End of prime numbers calculation on background thread");
	GLog->Log("--------------------------------------------------------------------");
	switch (eTaskState)
	{
	case ETaskState::_BacthCreate://批量创建obj文件
		BatchCreatObjFile();
		break;
	case ETaskState::_OneCreate://创建单个obj文件
		OneCreateObjFile(ModelFile);
			break;	
	//case ETaskState::_BacthCreate_Upload://批量创建并上传obj文件
	////	BatchCreatObjFile();

	//	break;
	//case ETaskState::_OneCreate_Upload://单个创建并上传obj文件
	//	//OneCreateObjFile(ModelFile);
	//	break;
	//case ETaskState::_BacthUpload://批量上传obj文件
	//	break;
	//case ETaskState::_OneUpload://上传单个obj文件
	//	break;

	}
}

void ObjFileAnsyTask::BatchCreatObjFile()
{
	TArray<FResourceInfo> ResInfoArr = ResMgr->PooledResource;

	for (int i = 0; i < ResInfoArr.Num(); ++i)
	{
		if (ResInfoArr[i].ResType== EResType::EResModel)
		{
			FString Resid = ResInfoArr[i].ResID;
			UModelFile * modelFile = Cast<UModelFile>(ResMgr->FindRes(Resid, true));
			modelFile->Load();
			OneCreateObjFile(modelFile);
		}
	}
}

void ObjFileAnsyTask::OneCreateObjFile(UModelFile *modelFile)
{
	if (modelFile)
	{
	 TArray<FModel*>& m_Models= modelFile->GetSubModels();
	 TArray<FVector> vertexData;
	 TArray<int32> FaceIndex;
	 int32 indexVertex = 1;
	 for (int i = 0; i < m_Models.Num(); ++i)
	 {
		
		 for (int j = 0; j < m_Models[i]->Sections.Num(); ++j)
		 {			
			//DealWithVertexData(vertexData, FaceIndex, m_Models[i]->Sections[j].ProcVertexBuffer, m_Models[i]->Sections[j].ProcIndexBuffer, indexVertex);
			 Make_TarryVertex(vertexData, m_Models[i]->Sections[j].ProcVertexBuffer);	
			 Make_TarryFaceIndex(FaceIndex, m_Models[i]->Sections[j].ProcIndexBuffer, indexVertex);
			indexVertex = vertexData.Num()+1;
		 }
		
	 }
	FString fileName=modelFile->Filename;
    int32 Subindex=fileName.Find(TEXT("/"),ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	fileName.RemoveAt(0, Subindex+1);
	fileName.RemoveFromEnd(TEXT(".mx"));
	 SaveObjFile(std::move(vertexData), std::move(FaceIndex), fileName);
	// _Mesh_Simplify->MeshSimplify(30000);
	
	}

}

//void ObjFileAnsyTask::OneUploadObjFile()
//{
//	UAJHttpUploader* OneTaskUpload=UAJHttpUploader::UploadFile(OneObjFileNameToUpload, TaskUrl, 0);
//}

void ObjFileAnsyTask::SaveObjFile(TArray<FVector>&& vertexArr, TArray<int32>&&FaceIndex, FString outFileName)
{
	FString path = ObjFileNamePath + outFileName+TEXT(".obj");
	std::ofstream fout(*path, std::ios::out);
	if (fout)
	{
		for (int32 i = 0; i < vertexArr.Num(); ++i)
		{
			fout << "v " << vertexArr[i].X << " " << vertexArr[i].Y << " "<< vertexArr[i].Z << std::endl;
		}
		for (int32 i = 0; i < FaceIndex.Num(); i+=3)
		{
			fout << "f " << FaceIndex[i+2] << " " << FaceIndex[i+1] << " " << FaceIndex[i]<< std::endl;
		}
		fout.close();
	}
	
}

void ObjFileAnsyTask::Make_TarryVertex(TArray<FVector>& vertex, TArray<FProcMeshVertex>& ProcVertexBuf)
{
	for (int i = 0; i < ProcVertexBuf.Num(); ++i)
	{
		vertex.Emplace(ProcVertexBuf[i].Position);
	}
}

void ObjFileAnsyTask::Make_TarryFaceIndex(TArray<int32>& FaceIndex, TArray<uint32>& ProcIndexBuffer, int32 num)
{
	for (int i = 0; i < ProcIndexBuffer.Num(); ++i)
	{
		FaceIndex.Emplace(ProcIndexBuffer[i]+num);
	}
}

void ObjFileAnsyTask::DealWithVertexData(TArray<FVector>& trueVertexData, TArray<int32>& trueFaceIndex, TArray<FProcMeshVertex>& untreatedProcVertexBuf, TArray<int32>& untreatedProcIndexBuffer, int32 num)
{
	bool *bVertexUsed = new bool[untreatedProcVertexBuf.Num()]{false};
	for (int32 i = 0; i < untreatedProcIndexBuffer.Num(); ++i)
	{
		 bVertexUsed[untreatedProcIndexBuffer[i]] = true;
	}
	TArray<int32> indexNum;
	int curFalseDot=0;
	for (int32 i = 0; i < untreatedProcVertexBuf.Num(); ++i)
	{
		if (false == bVertexUsed[i])
		{
			curFalseDot++;
		}
		else
		{
			trueVertexData.Emplace(untreatedProcVertexBuf[i].Position);
		}
		indexNum.Emplace(curFalseDot);
	}

	for (int32 i = 0; i < untreatedProcIndexBuffer.Num(); i++)
	{	
		int curFaceIndex = untreatedProcIndexBuffer[i] - indexNum[untreatedProcIndexBuffer[i]];//当前顶点前面有多少个孤立点都减去
		trueFaceIndex.Emplace(curFaceIndex+ num);
	}
	delete[] bVertexUsed;
}


