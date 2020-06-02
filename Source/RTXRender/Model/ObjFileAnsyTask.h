// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "ResourceMgr.h"
#include "ProceduralMeshComponent.h"
/**
 * 
 */
enum class ETaskState:uint8
{
	_NONEState,//
	_BacthCreate,//
	_BacthUpload,//
	_OneCreate,
	_OneUpload,//
	_BacthCreate_Upload,//
	_OneCreate_Upload
};
class UModelFile;
class  ObjFileAnsyTask: public  FNonAbandonableTask
{
public:
	ObjFileAnsyTask();
	ObjFileAnsyTask(UResourceMgr& Mgr, FString  filePath);//批量产生Obj文件
	//ObjFileAnsyTask(TArray<FString>&& ObjFileNameArr,FString& url);//批量上传obj文件
	ObjFileAnsyTask(UModelFile* model, FString filePath);//产生单个Obj文件
	//ObjFileAnsyTask(FString& OneObjFileName, FString& url);//上传单个obj文件

	~ObjFileAnsyTask();
	 void DoWork();
	 FORCEINLINE TStatId GetStatId() const
	 {
		 RETURN_QUICK_DECLARE_CYCLE_STAT(ObjFileAnsyTask, STATGROUP_ThreadPoolAsyncTasks);
	 }
	 void BatchCreatObjFile();//批量生产Obj文件
	 void OneCreateObjFile(UModelFile *model);//只生产一个obj文件--->由已知的model生成
	// void BatchUploadObjFile();
	// void OneUploadObjFile();
	 void SaveObjFile(TArray<FVector>&& vertexArr,TArray<int32>&&FaceIndex,FString outFileName);//保存obj文件
	 void Make_TarryVertex(TArray<FVector>& vertex, TArray<FProcMeshVertex>& ProcVertexBuf);//保存一个mx模型的所有顶点
	 void Make_TarryFaceIndex(TArray<int32>& FaceIndex,TArray<uint32>& ProcIndexBuffer,int32 num);//保存一个mx模型的所有面的索引
	 void DealWithVertexData(TArray<FVector>& trueVertexData,TArray<int32>& trueFaceIndex, TArray<FProcMeshVertex>& untreatedProcVertexBuf,TArray<int32>& untreatedProcIndexBuffer, int32 num);//处理孤立点
public:
	UResourceMgr* ResMgr;
	UModelFile* ModelFile;
	ETaskState eTaskState;
	FString ObjFileNamePath;

	//FString OneObjFileNameToUpload;
	//TArray<FString> ObjFileNameArrToUpLoad;

	//FString TaskUrl;
};
