// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// FBX casts null pointer to a reference
THIRD_PARTY_INCLUDES_START
#include <fbxsdk.h>
THIRD_PARTY_INCLUDES_END

#include "ModelFile.h"
#include "TextureImporter.h"
#include "StandardMaterialCollection.h"
#include "ModelImporter.generated.h"

#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)


UCLASS(BlueprintType)
class UModelImporter : public UObject
{
	GENERATED_UCLASS_BODY()
protected:

	struct TriGroup
	{
		int32			MaterialIndex;
		int32			StartIndex;
		int32			EndIndex;
		TArray<uint32>	Triangles;
	};

	struct MeshInfo
		{
		MeshInfo()
			:fbxMesh(NULL)
			, Root(NULL)
			, Skeleton(INDEX_NONE)
		{
		}
		int32						GroupIndex;
		int32						LodLevel;
		int32						NumTriangle;
		FString						Name;
		FString						LodGroupName;
		int32						MorphNum;
		FbxNode						*Root;
		FString						SkeletonRoot;
		int32						SkeletonElemNum;
		int32						Skeleton;
		int32						NumControlPoints;
		FbxMesh						*fbxMesh;
		TArray<TriGroup>			TriangleGroups;
		FTransform					GeometryOffset;
		FModel						*Model;
		TArray<FVector>				UCXVertices;
		TArray<int32>				UCXIndices;
	};

	struct MaterialInfo
	{
		MaterialInfo()
			:MaterialSurface(NULL)
			,Material(NULL)
		{
		}
		FString Name;
		FModelMaterialUE4  *Material;
		FbxSurfaceMaterial *MaterialSurface;
	};

	struct LayerInfoSet
	{
		LayerInfoSet(FbxLayer *BaseLayer)
			:NormalReferenceMode(FbxLayerElement::eDirect)
			, NormalMappingMode(FbxLayerElement::eByControlPoint)
			, BiNormalMappingMode(FbxLayerElement::eByControlPoint)
			, TangentReferenceMode(FbxLayerElement::eDirect)
			, TangentMappingMode(FbxLayerElement::eByControlPoint)
			, layerElementUV(NULL)
			, UVReferenceMode(NULL)
			, UVMappingMode(NULL)
			, UvSetCount(0)
		{
			layerNormal = BaseLayer->GetNormals();
			layerBinormal = BaseLayer->GetBinormals();
			layerTangent = BaseLayer->GetTangents();
		}

		~LayerInfoSet()
		{
			SAFE_DELETE_ARRAY(layerElementUV);
			SAFE_DELETE_ARRAY(UVReferenceMode);
			SAFE_DELETE_ARRAY(UVMappingMode);
		}

		bool HasBNTNormal() { return layerBinormal && layerBinormal && layerTangent; }

		//normal reference mode
		FbxLayerElementNormal *layerNormal;
		FbxLayerElement::EReferenceMode NormalReferenceMode;
		FbxLayerElement::EMappingMode NormalMappingMode;

		//binormal reference mode
		FbxLayerElementBinormal *layerBinormal;
		FbxLayerElement::EReferenceMode BiNormalReferenceMode;
		FbxLayerElement::EMappingMode BiNormalMappingMode;

		//tangent reference mode
		FbxLayerElementTangent *layerTangent;
		FbxLayerElement::EReferenceMode TangentReferenceMode;
		FbxLayerElement::EMappingMode TangentMappingMode;

		//uv reference mode
		int32 UvSetCount;
		FbxLayerElementUV **layerElementUV;
		FbxLayerElement::EReferenceMode *UVReferenceMode;
		FbxLayerElement::EMappingMode *UVMappingMode;
	};

	struct TextureInfo
	{
		FString FileName;
		ETexSlot Slot;
		FModelTexture *Texture;
	};

	struct NodeInfo
	{
		FbxNode		*node;
		FString		Name;
		int32		ParentIndex;
		FTransform  WorldTransform;
		FTransform	ParentTransform;
	};

	struct Context
	{
		Context()
			:NumGeometryCount(0)
			, Time(0)
			, Pose(NULL)
			, ModelFile(NULL)
// 			, CompoundModelFile(NULL)
		{
		}
		FString					FilePath;
		int32					NumGeometryCount;
		FbxTime					Time;
		FbxPose					*Pose;
		TArray<MeshInfo>		Meshes;
		UModelFile				*ModelFile;
// 		UCompoundModelFile		*CompoundModelFile;
		TArray<UModelFile *>	Models;
		TArray<FTransform>		ModelOffsetTransforms;
		TArray<MaterialInfo>	Materials;
		TArray<TextureInfo>		Textures;
		TArray<NodeInfo>		AllNodes;
		bool IsHandChange() { return false; }
	};
public:
	bool Initialize();
	void Unitialize();
	bool Import(UModelFile *ModelFile, const FString &InFbxFilename);
// 	bool ImportCompoundModel (UCompoundModelFile *ModelFile, const FString &InFbxFilename);
	UTextureImporter *GetTextureImporter() { return TexImporter; }
	void RedirectMaterialPath(UModelFile *modelFile);
protected:
	bool ConstructFbxScene(Context &ctx, const FString &InFbxFilename);
	void DestroyFbxScene();
	void CollectSceneInfo(Context &ctx, FbxScene *Scene);
	FbxAMatrix GetGlobalMatrix(FbxPose *pose, FbxNode *node, const FbxTime &pTime);
	void ImportMeshes(Context &ctx);
	void ImportUCX(Context &ctx);
	void ImportSkeleton(Context &ctx);
	FbxAMatrix GetGeometry(FbxNode *node);
	FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
	void GetFbxMeshUVSet(FbxMesh *fbxMesh, TArray<FString> &UVSets);
	void GetFbxMeshMaterials(Context &ctx, UModelFile *Model, FbxMesh *fbx, MeshInfo &meshInfo);
	void GetLayerElementRefAndMappingMode(FbxLayerElement *layerElement, FbxLayerElement::EReferenceMode &refMode, FbxLayerElement::EMappingMode &mappingMode);
	void GetUVLayerElementRefAndMappingMode(FbxMesh *fbxMesh, TArray<FString> &UVSet, FbxLayerElementUV **&layerElement, FbxLayerElement::EReferenceMode *&refMode, FbxLayerElement::EMappingMode *&mappingMode);
	void GetFbxMeshTriangleGroup(FbxMesh *fbxMesh, MeshInfo &meshInfo);
	void BuildMeshElement(Context &ctx, FProcMeshSection &Section, FbxMesh *fbxMesh, int32 TriangleGroupIndex, MeshInfo &info, LayerInfoSet &layerInfoSet);
	int32 ImportMaterial(Context &ctx, UModelFile *Model, FbxSurfaceMaterial *InMaterial, MeshInfo &meshInfo);
	int32 ImportTexture(Context &ctx, UModelFile *Model, ETexSlot slot, FModelMaterial *material, const FString &TexFilename);
	bool IsUCX(MeshInfo &Info);
	int32 FindUCX(Context &ctx, MeshInfo &Info);
	static FVector ToPosition(const FbxVector4 &pos);
	static FVector ToVector(const FbxVector4 &vec);
	static FTransform ToTransform(const FbxAMatrix &pose);
protected:
	UPROPERTY()
	UTextureImporter	*TexImporter;
	FbxManager			*m_pFbxManger;
	FbxScene			*m_pScene;
	FbxImporter			*m_Importer;
};


