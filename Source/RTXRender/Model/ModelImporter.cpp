

#include "ModelImporter.h"
#include "ResourceMgr.h"
#include "ConvexHullPhysicBody.h"
#include "StandardMaterialCollection.h"
#include "ajdr/Editor/EditorUtils.h"

bool UModelImporter::ConstructFbxScene(Context &ctx, const FString &InFbxFilename)
{
	if (!m_pFbxManger)
	{
		return false;
	}

	int lFileMajor, lFileMinor, lFileRevision;

	bool bOk = true;
	ctx.FilePath = FPaths::GetPath(InFbxFilename);

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	m_pScene = FbxScene::Create(m_pFbxManger, "My Scene");
	if (!m_pScene)
	{
		bOk = false;
		goto finish;
	}

	// Create an importer.
	m_Importer = FbxImporter::Create(m_pFbxManger, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = m_Importer->Initialize(TCHAR_TO_ANSI(*InFbxFilename), -1, m_pFbxManger->GetIOSettings());
	m_Importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
	if (!lImportStatus)
	{
		bOk = false;
		goto finish;
	}

	if (!m_Importer->Import(m_pScene))
	{
		FbxGeometryConverter converter(m_pFbxManger);
		converter.Triangulate(m_pScene, true);
		bOk = false;
		goto finish;
	}

finish:
	if (!bOk)
	{
		DestroyFbxScene();
	}

	return bOk;
}

void UModelImporter::DestroyFbxScene()
{
	if (m_Importer)
	{
		m_Importer->Destroy();
		m_Importer = NULL;
	}

	if (m_pScene)
	{
		m_pScene->Destroy();
		m_pScene = NULL;
	}
}

bool UModelImporter::Import(UModelFile *ModelFile, const FString &InFbxFilename)
{
	Context ctx;
	ctx.ModelFile = ModelFile;

	if (!ConstructFbxScene(ctx, InFbxFilename))
	{
		return false;
	}

	CollectSceneInfo(ctx, m_pScene);

	ImportUCX(ctx);

	ImportMeshes(ctx);

	DestroyFbxScene();

	if (ctx.ModelFile)
	{
		ctx.ModelFile->Compress();

		//Generate Collision
		GenerateModelFileConvexHull(ctx.ModelFile, 0.1f, 12, false);

		if (IsInGameThread())
		{
			RedirectMaterialPath(ctx.ModelFile);
		}
	}

	return true;
}

// bool UModelImporter::ImportCompoundModel(UCompoundModelFile *ModelFile, const FString &InFbxFilename)
// {
// 	Context ctx;
// 	ctx.CompoundModelFile = ModelFile;
// 
// 	FString ResName = FPaths::GetBaseFilename(InFbxFilename);
// 	ctx.CompoundModelFile->SetResourceName(ResName);
// 
// 	if (!ConstructFbxScene(ctx, InFbxFilename))
// 	{
// 		return false;
// 	}
// 
// 	CollectSceneInfo(ctx, m_pScene);
// 
// 	ImportUCX(ctx);
// 
// 	ImportMeshes(ctx);
// 
// 	ImportSkeleton(ctx);
// 
// 	DestroyFbxScene();
// 
// 	if (ctx.CompoundModelFile)
// 	{
// 		for (int32 i = 0; i < ctx.AllNodes.Num(); ++i)
// 		{
// 			NodeInfo &node = ctx.AllNodes[i];
// 			ctx.CompoundModelFile->AddBone(node.Name, node.ParentIndex, node.ParentTransform);
// 		}
// 
// 		for (int32 i = 0; i < ctx.Models.Num(); ++i)
// 		{
// 			UModelFile *Model = ctx.Models[i];
// 			if (Model)
// 			{
// 				ctx.CompoundModelFile->AddModel(Model, *Model->GetResourceName(), ctx.ModelOffsetTransforms[i]);
// 			}
// 		}
// 	}
// 
// 	return true;
// }

bool UModelImporter::Initialize()
{
	m_pFbxManger = FbxManager::Create();
	if (!m_pFbxManger)
	{
		return false;
	}

	FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxManger, IOSROOT);
	m_pFbxManger->SetIOSettings(ios);

	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	ios->SetBoolProp(IMP_FBX_LINK, true);
	ios->SetBoolProp(IMP_FBX_SHAPE, true);
	ios->SetBoolProp(IMP_FBX_GOBO, true);
	ios->SetBoolProp(IMP_FBX_ANIMATION, true);
	ios->SetBoolProp(IMP_SKINS, true);
	ios->SetBoolProp(IMP_DEFORMATION, true);
	ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	ios->SetBoolProp(IMP_TAKE, true);

	int lSDKMajor, lSDKMinor, lSDKRevision;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	return true;
}

void UModelImporter::Unitialize()
{
	if (m_pFbxManger)
	{
		m_pFbxManger->Destroy();
		m_pFbxManger = NULL;
	}
}

UModelImporter::UModelImporter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	,m_pFbxManger(NULL)
	,m_pScene(NULL)
{
	TexImporter = NewObject<UTextureImporter>();
}

void UModelImporter::CollectSceneInfo(Context &ctx, FbxScene *Scene)
{
	int32 NumGeometryCount = 0;
	TArray<FbxNode*> LinkNodes;

	if (m_pScene->GetPoseCount() > 0)
	{
		ctx.Pose = m_pScene->GetPose(0);
	}

	for (int i = 0; i < Scene->GetGeometryCount(); ++i)
	{
		FbxGeometry *Geometry = Scene->GetGeometry(i);

		if (Geometry->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxNode *GeoNode = Geometry->GetNode();
			if (!GeoNode)
			{
				continue;
			}

			if (LinkNodes.Find(GeoNode) != INDEX_NONE)
			{
				continue;
			}

			ctx.NumGeometryCount++;

			FbxMesh *Mesh = (FbxMesh *)Geometry;
			int32 Index = ctx.Meshes.AddDefaulted(1);
			MeshInfo &meshInfo = ctx.Meshes[Index];
			meshInfo.Name = ANSI_TO_TCHAR(GeoNode->GetName());
			meshInfo.fbxMesh = Mesh;
			meshInfo.NumTriangle = Mesh->GetPolygonCount();
			meshInfo.NumControlPoints = Mesh->GetControlPointsCount();
			meshInfo.GeometryOffset = ToTransform(GetGeometry(GeoNode));
		}
	}
}

FbxAMatrix UModelImporter::GetGeometry(FbxNode *node)
{
	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(lT, lR, lS);
}

FbxAMatrix UModelImporter::GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));
	return lPoseMatrix;
}

FbxAMatrix UModelImporter::GetGlobalMatrix(FbxPose *pose, FbxNode *node, const FbxTime &pTime)
{
	bool bFoundPosition = false;
	FbxAMatrix lGlobalPosition;
	if (pose != NULL)
	{
		int lNodeIndex = pose->Find(node);
		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pose->IsBindPose() || !pose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;
				if (node->GetParent())
				{
					lParentGlobalPosition = GetGlobalMatrix(pose, node->GetParent(), pTime);
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			bFoundPosition = true;
		}
	}

	if (!bFoundPosition)
	{
		lGlobalPosition = node->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

bool UModelImporter::IsUCX(MeshInfo &Info)
{
	if (Info.fbxMesh)
	{
		FbxNode *node = Info.fbxMesh->GetNode();
		if (node)
		{
			FString MeshName = ANSI_TO_TCHAR(node->GetName());
			Info.Name = MeshName;
			if (MeshName.Find(TEXT("UCX")) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

void UModelImporter::ImportMeshes(Context &ctx)
{
	for (int i = 0; i < ctx.Meshes.Num(); ++i)
	{
		MeshInfo &Info = ctx.Meshes[i];
		
		if (IsUCX(Info))
		{
			continue;
		}

		UModelFile *Model = ctx.ModelFile;
		if (!Model)
		{
			Model = UResourceMgr::GetResourceMgr()->NewModel();
			ctx.Models.Add(Model);
			ctx.ModelOffsetTransforms.Add(Info.GeometryOffset);
			Model->LocalBounds.Init();
		}

		FbxMesh *fbxMesh = Info.fbxMesh;

		if (!fbxMesh->IsTriangleMesh())
		{
			continue;
		}

		Model->SetResourceName(Info.Name);
		Info.Model = Model->NewModel();
		Info.Model->ModelName = Info.Name;
		Info.Model->Bounds.Init();

		fbxMesh->RemoveBadPolygons();

		int layerCount = fbxMesh->GetLayerCount();
		if (layerCount == 0)
		{
			return;
		}

		FbxLayer *BaseLayer = fbxMesh->GetLayer(0);
		if (!BaseLayer)
		{
			return;
		}

		//��� uv ����
		TArray<FString> UVSets;
		GetFbxMeshUVSet(fbxMesh, UVSets);

		//���ģ�����ò���
		GetFbxMeshMaterials(ctx, Model, fbxMesh, Info);

		//��ȡLayer��Ϣ
		LayerInfoSet LayerInfos(BaseLayer);
		LayerInfos.UvSetCount = UVSets.Num();

		bool HasNTB = LayerInfos.HasBNTNormal();
		GetLayerElementRefAndMappingMode(LayerInfos.layerNormal, LayerInfos.NormalReferenceMode, LayerInfos.NormalMappingMode);
		GetLayerElementRefAndMappingMode(LayerInfos.layerBinormal, LayerInfos.BiNormalReferenceMode, LayerInfos.BiNormalMappingMode);
		GetLayerElementRefAndMappingMode(LayerInfos.layerTangent, LayerInfos.TangentReferenceMode, LayerInfos.TangentMappingMode);

		//uv layer mode
		GetUVLayerElementRefAndMappingMode(fbxMesh, UVSets, LayerInfos.layerElementUV, LayerInfos.UVReferenceMode, LayerInfos.UVMappingMode);

		//get tirangle group
		GetFbxMeshTriangleGroup(fbxMesh, Info);

		int32 NumSection = Info.TriangleGroups.Num();
		Info.Model->Sections.SetNum(NumSection);

		for (int32 TriangleGroupIndex = 0; TriangleGroupIndex < NumSection; ++TriangleGroupIndex)
		{
			FProcMeshSection &Section = Info.Model->Sections[TriangleGroupIndex];
			Section.Reset();

			BuildMeshElement(ctx, Section, fbxMesh, TriangleGroupIndex, Info, LayerInfos);

			int32 nVertices = Section.ProcVertexBuffer.Num();

			Section.ProcIndexBuffer.SetNum(nVertices);

			for (int32 index = 0; index < nVertices; ++index)
			{
				Section.ProcIndexBuffer[index] = index;
			}

			Info.Model->Bounds += Section.SectionLocalBox;
			Model->LocalBounds += Section.SectionLocalBox;
		}

		Model->ResetSize();

		int32 UCXIndex = FindUCX(ctx, Info);
		if (UCXIndex != INDEX_NONE)
		{
			MeshInfo &UCXInfo = ctx.Meshes[UCXIndex];
			Info.Model->UCXData.Vertices = UCXInfo.UCXVertices;
			Info.Model->UCXData.Indices = UCXInfo.UCXIndices;
			GenerateModelConvexHull(Info.Model, 0.1f, 12);
		}
	}
}

int32 UModelImporter::FindUCX(Context &ctx, MeshInfo &Info)
{
	int32 FoundIndex = INDEX_NONE;

	if (Info.Model)
	{
		FString UCXName = FString::Printf(TEXT("UCX_%s"), *Info.Name);
		for (int i = 0; i < ctx.Meshes.Num(); ++i)
		{
			MeshInfo &CurInfo = ctx.Meshes[i];
			if (CurInfo.Name == UCXName && CurInfo.UCXVertices.Num() > 0)
			{
				FoundIndex = i;
				break;
			}
		}
	}

	return FoundIndex;
}

const int32 ReCCWIndex[] = { 0, 1, 2 };

void UModelImporter::ImportUCX(Context &ctx)
{
	for (int meshIndex = 0; meshIndex < ctx.Meshes.Num(); ++meshIndex)
	{
		MeshInfo &Info = ctx.Meshes[meshIndex];

		if (!IsUCX(Info))
		{
			continue;
		}

		FbxMesh *fbxMesh = Info.fbxMesh;

		if (!fbxMesh->IsTriangleMesh())
		{
			continue;
		}

		fbxMesh->RemoveBadPolygons();

		int NumCtrlPoints = fbxMesh->GetControlPointsCount();
		Info.UCXVertices.SetNum(NumCtrlPoints);

		for (int32 iVert = 0; iVert < NumCtrlPoints; ++iVert)
		{
			FbxVector4 Position = fbxMesh->GetControlPoints()[iVert];
			FVector Loc = ToPosition(Position);
			Info.UCXVertices[iVert] = Loc;
		}


		int32 PolyCount = fbxMesh->GetPolygonCount();
		Info.UCXIndices.SetNum(PolyCount * 3);

		for (int32 triIndex = 0; triIndex < PolyCount; ++triIndex)
		{
			for (int32 VertIndex = 0; VertIndex < 3; ++VertIndex)
			{
				int32 ControlPointIndex = fbxMesh->GetPolygonVertex(triIndex, ReCCWIndex[VertIndex]);
				
				Info.UCXIndices[triIndex * 3 + VertIndex] = ControlPointIndex;
			}
		}
	}
}

void UModelImporter::BuildMeshElement(Context &ctx, FProcMeshSection &Section, FbxMesh *fbxMesh, int32 TriangleGroupIndex, MeshInfo &info, LayerInfoSet &layerInfoSet)
{
	int32 NumTriagles = info.TriangleGroups[TriangleGroupIndex].Triangles.Num();
	int32 NumVerts = 3 * NumTriagles;

	Section.ProcVertexBuffer.SetNum(NumVerts);

	//polygon information
	for (int32 TriangleElmIndex = 0; TriangleElmIndex < NumTriagles; ++TriangleElmIndex)
	{
		int32 PolygonIndex = info.TriangleGroups[TriangleGroupIndex].Triangles[TriangleElmIndex];
		int32 VertIndex = 0;

		for (VertIndex = 0; VertIndex < 3; ++VertIndex)
		{
			int32 VtxTempIndex = 3 * TriangleElmIndex + VertIndex;
			int32 FbxTempIndex = 3 * PolygonIndex + ReCCWIndex[VertIndex];
			
			//position
			check (fbxMesh->GetPolygonSize(PolygonIndex) == 3);
			int32 ControlPointIndex = fbxMesh->GetPolygonVertex(PolygonIndex, ReCCWIndex[VertIndex]);
			FbxVector4 Position = fbxMesh->GetControlPoints()[ControlPointIndex];
			FVector Loc = ToPosition(Position);
			Section.ProcVertexBuffer[VtxTempIndex].Position = Loc;
			Section.SectionLocalBox += Loc;

			bool bHasNTBInformation = layerInfoSet.layerNormal && layerInfoSet.layerTangent && layerInfoSet.layerBinormal;

			//normal
			int32 NormalMapIndex = (layerInfoSet.NormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : FbxTempIndex;
			int32 NormalValueIndex = (layerInfoSet.NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : (layerInfoSet.layerNormal ? layerInfoSet.layerNormal->GetIndexArray().GetAt(NormalMapIndex) : 0);
			FbxVector4 TempValue = layerInfoSet.layerNormal? layerInfoSet.layerNormal->GetDirectArray().GetAt(NormalValueIndex) : FbxVector4();
			Section.ProcVertexBuffer[VtxTempIndex].Normal = ToVector(TempValue);

			if (bHasNTBInformation)
			{
				FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
				FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
				if (layerInfoSet.layerTangent)
				{
					TangentReferenceMode = layerInfoSet.layerTangent->GetReferenceMode();
					TangentMappingMode = layerInfoSet.layerTangent->GetMappingMode();
				}

				FbxLayerElement::EReferenceMode BinormalReferenceMode(FbxLayerElement::eDirect);
				FbxLayerElement::EMappingMode BinormalMappingMode(FbxLayerElement::eByControlPoint);
				if (layerInfoSet.layerBinormal)
				{
					BinormalReferenceMode = layerInfoSet.layerBinormal->GetReferenceMode();
					BinormalMappingMode = layerInfoSet.layerBinormal->GetMappingMode();
				}
				
				int TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ?
					ControlPointIndex : FbxTempIndex;

				int TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ?
					TangentMapIndex : layerInfoSet.layerTangent->GetIndexArray().GetAt(TangentMapIndex);

				TempValue = layerInfoSet.layerTangent->GetDirectArray().GetAt(TangentValueIndex);
				Section.ProcVertexBuffer[VtxTempIndex].Tangent.TangentX = ToVector(TempValue);
			}

			Section.ProcVertexBuffer[VtxTempIndex].Color = FColor::White;
		}

		//uv
		if (layerInfoSet.UvSetCount > 0)
		{
			if (layerInfoSet.layerElementUV[0] != NULL)
			{
				for (int32 VertexIndex = 0; VertexIndex < 3; ++VertexIndex)
				{
					int32 VtxTempIndex = 3 * TriangleElmIndex + VertexIndex;
					int32 FbxTempIndex = 3 * PolygonIndex + ReCCWIndex[VertexIndex];
					int32 ControlPoint = fbxMesh->GetPolygonVertex(PolygonIndex, ReCCWIndex[VertexIndex]);
					int32 UVMapIndex = layerInfoSet.UVMappingMode[0] == FbxLayerElement::eByControlPoint ? ControlPoint : FbxTempIndex;
					int32 UVIndex = layerInfoSet.UVReferenceMode[0] == FbxLayerElement::eDirect ? UVMapIndex : layerInfoSet.layerElementUV[0]->GetIndexArray().GetAt(UVMapIndex);
					FbxVector2 UVVector = layerInfoSet.layerElementUV[0]->GetDirectArray().GetAt(UVIndex);
					Section.ProcVertexBuffer[VtxTempIndex].UV0 = FVector2D(float(UVVector[0]), 1.0f - float(UVVector[1]));
				}
			}
		}
	}
}

void UModelImporter::GetFbxMeshUVSet(FbxMesh *fbxMesh, TArray<FString> &UVSets)
{
	int32 LayerCount = fbxMesh->GetLayerCount();
	if (LayerCount > 0)
	{
		for (int32 UVLayerIndex = 0; UVLayerIndex < LayerCount; ++UVLayerIndex)
		{
			FbxLayer *layer = fbxMesh->GetLayer(UVLayerIndex);
			int32 UVSetCount = layer->GetUVSetCount();
			if (UVSetCount > 0)
			{
				FbxArray<FbxLayerElementUV const*> ElUVs = layer->GetUVSets();
				for (int32 UVIndex = 0; UVIndex < UVSetCount; ++UVIndex)
				{
					FbxLayerElementUV const * ElementUV = ElUVs[UVIndex];
					if (ElementUV != NULL)
					{
						FString UVName = ANSI_TO_TCHAR(ElementUV->GetName());
						UVSets.Add(UVName);
					}
				}
			}
		}
	}
}

void UModelImporter::GetFbxMeshMaterials(Context &ctx, UModelFile *Model, FbxMesh *fbxMesh, MeshInfo &meshInfo)
{
	FbxNode * node = fbxMesh->GetNode();
	int32 MaterialCount = node->GetMaterialCount();
	meshInfo.Model->RefMaterials.SetNum(MaterialCount);

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
	{
		FbxSurfaceMaterial *fbxMaterial = node->GetMaterial(MaterialIndex);
		int32 MatIndex = ImportMaterial(ctx, Model, fbxMaterial, meshInfo);
		meshInfo.Model->RefMaterials[MaterialIndex] = MatIndex;
	}
}

int32 UModelImporter::ImportMaterial(Context &ctx, UModelFile *Model, FbxSurfaceMaterial *InMaterial, MeshInfo &meshInfo)
{
	for (int32 MaterialIndex = 0; MaterialIndex < ctx.Materials.Num(); ++MaterialIndex)
	{
		MaterialInfo &Info = ctx.Materials[MaterialIndex];
		if (Info.MaterialSurface == InMaterial)
		{
			return MaterialIndex;
		}
	}

	int32 texSlots[ETexSlot::eMax];
	for (int32 i = 0; i < ETexSlot::eMax; ++i)
	{
		texSlots[i] = INDEX_NONE;
	}

	int32 Index = ctx.Materials.AddDefaulted(1);
	MaterialInfo &materialInfo = ctx.Materials[Index];
	materialInfo.MaterialSurface = InMaterial;
	materialInfo.Material = Model->NewMaterial();
	materialInfo.Material->Name = ANSI_TO_TCHAR(InMaterial->GetName());

	//diffuse map  ok
	FbxProperty fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString DiffuseMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());			
			texSlots[ETexSlot::eDiffuse] = ImportTexture(ctx, Model, ETexSlot::eDiffuse, materialInfo.Material, DiffuseMap);
		}
	}

	//specular map  ok
	fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString SpecularMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eSpecular] = ImportTexture(ctx, Model, ETexSlot::eSpecular, materialInfo.Material, SpecularMap);
		}
	}

	//emissive map  ok
	fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString EmissiveMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eEmissive] = ImportTexture(ctx, Model, ETexSlot::eEmissive, materialInfo.Material, EmissiveMap);
		}
	}

	//opacity map ok
	fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString AlphaMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eOpacity] = ImportTexture(ctx, Model, ETexSlot::eOpacity, materialInfo.Material, AlphaMap);
		}
	}

	//roughness map 
	fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString RoughnessMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eRoughness] = ImportTexture(ctx, Model, ETexSlot::eRoughness, materialInfo.Material, RoughnessMap);
		}
	}

	//metallic map
	fbxProperty = InMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString MatallicMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eMetallic] = ImportTexture(ctx, Model, ETexSlot::eMetallic, materialInfo.Material, MatallicMap);
		}
	}

	//normal map 
	fbxProperty = InMaterial->FindProperty("Bump");
	if (fbxProperty.IsValid())
	{
		int32 LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			FString NormalMap = FPaths::GetCleanFilename(fbxTexture->GetRelativeFileName());
			texSlots[ETexSlot::eNormal] = ImportTexture(ctx, Model, ETexSlot::eNormal, materialInfo.Material, NormalMap);
		}
	}
	
	UObject *WorldObject = ctx.ModelFile;
// 	if (!WorldObject) 
// 	{
// 		WorldObject = ctx.CompoundModelFile;
// 	}
	
	UStandardMaterialCollection *DefCollection = UEditorUtils::GetMaterialCollection(WorldObject);

	if (DefCollection!=NULL)
	{
		int32 FoundIndex = INDEX_NONE;

		FString Name = materialInfo.Material->Name;
		Name.FindChar(_T('_'), FoundIndex);

		if (FoundIndex != INDEX_NONE)
		{
			Name = Name.Mid(0, FoundIndex);
		}

		Name = FString::Printf(TEXT("M_%s_"), *Name);

		for (int32 i = 0; i < ETexSlot::eMax; ++i)
		{
			if (texSlots[i] != INDEX_NONE)
			{
				Name += GetTexSlotKey(ETexSlot(i));
			}
		}
	}

	return Index;
}

void UModelImporter::ImportSkeleton(Context &ctx)
{
	TArray<FbxNode *> AllNodes;

	FbxNode *Root = m_pScene->GetRootNode();
	if (Root)
	{
		TArray<FbxNode *> Stack;
		Stack.Add(Root);

		while (Stack.Num() > 0)
		{
			FbxNode *node = Stack.Pop();
			int32 Num = node->GetChildCount(false);

			NodeInfo nodeInfo;
			nodeInfo.node = node;
			nodeInfo.Name = ANSI_TO_TCHAR(node->GetName());
			FbxAMatrix LocalMatrix = node->EvaluateLocalTransform();
			nodeInfo.ParentTransform = ToTransform(LocalMatrix);

			ctx.AllNodes.Add(nodeInfo);

			int32 nodeIndex = AllNodes.AddUnique(node);
			for (int32 childIndex = 0; childIndex < Num; ++childIndex)
			{
				FbxNode *child = node->GetChild(childIndex);
				Stack.Add(child);
			}
		}

		for (int32 index = 0; index < ctx.AllNodes.Num(); ++index)
		{
			NodeInfo &nodeInfo = ctx.AllNodes[index];
			nodeInfo.ParentIndex = INDEX_NONE;
			FbxNode *parentNode = nodeInfo.node->GetParent();
			for (int32 nodeIndex = 0; nodeIndex < ctx.AllNodes.Num(); ++nodeIndex)
			{
				if (ctx.AllNodes[nodeIndex].node == parentNode)
				{
					nodeInfo.ParentIndex = nodeIndex;
					break;
				}
			}
		}

		for (int32 index = 0; index < ctx.AllNodes.Num(); ++index)
		{
			NodeInfo &nodeInfo = ctx.AllNodes[index];
			if (nodeInfo.ParentIndex != INDEX_NONE)
			{				
				NodeInfo &parentInfo = ctx.AllNodes[nodeInfo.ParentIndex];
				nodeInfo.WorldTransform = nodeInfo.ParentTransform*parentInfo.WorldTransform;
			}
			else
			{
				nodeInfo.WorldTransform = nodeInfo.ParentTransform;
			}
		}
	}
}

void UModelImporter::RedirectMaterialPath(UModelFile *modelFile)
{
	if (modelFile!=NULL)
	{
		UStandardMaterialCollection *DefCollection = UEditorUtils::GetMaterialCollection(modelFile);
		if (DefCollection)
		{
			for (int32 i = 0; i < modelFile->GetNumberOfMaterial(); ++i)
			{
				FModelMaterial *mtrl = modelFile->GetMaterial(i);
				if (mtrl->GetType() == EMaterialUE4)
				{
					FModelMaterialUE4 *ue4Mtrl = (FModelMaterialUE4*)mtrl;
					UMaterialInterface *Material = DefCollection->GetMaterialByName(ue4Mtrl->Name);
					if (Material)
					{
						ue4Mtrl->UE4Path = Material->GetPathName();
					}
				}
			}
		}
	}
}

int32 UModelImporter::ImportTexture(Context &ctx, UModelFile *Model, ETexSlot slot, FModelMaterial *material, const FString &TexFilename)
{
	int32 TexIndex = INDEX_NONE;

	for (int32 i = 0; i < ctx.Textures.Num(); ++i)
	{
		if (ctx.Textures[i].FileName == TexFilename)
		{
			TexIndex = i;
			break;
		}
	}

	if (TexIndex == INDEX_NONE && TexImporter)
	{
		FString Filename = FString::Printf(TEXT("%s\\%s"), *ctx.FilePath, *TexFilename);
		FTextureSourceInfo *TextureSourceInfo = TexImporter->ImportFromFile(ctx.ModelFile, slot, Filename);
		if (TextureSourceInfo)
		{
			TextureInfo Info;
			Info.Slot = slot;
			Info.FileName = TexFilename;
			Info.Texture = Model->NewTexture();
			Info.Texture->SetData(*TextureSourceInfo);
			Info.Texture->Name = *FPaths::GetBaseFilename(TexFilename);
			TexIndex = ctx.Textures.Add(Info);		
			FString TexName = GetTexSlotName(slot);
			material->SetTextureValue(*TexName, TexIndex);
		}
	}
	return TexIndex;
}

void UModelImporter::GetLayerElementRefAndMappingMode(FbxLayerElement *layerElement, FbxLayerElement::EReferenceMode &refMode, FbxLayerElement::EMappingMode &mappingMode)
{
	if (layerElement)
	{
		refMode = layerElement->GetReferenceMode();
		mappingMode = layerElement->GetMappingMode();
	}
}

void UModelImporter::GetUVLayerElementRefAndMappingMode(FbxMesh *fbxMesh, TArray<FString> &UVSet, FbxLayerElementUV **&layerElement, FbxLayerElement::EReferenceMode *&refMode, FbxLayerElement::EMappingMode *&mappingMode)
{
	int32 LayerCount = fbxMesh->GetLayerCount();

	int32 UVSetCount = UVSet.Num();
	if (UVSetCount > 0)
	{
		layerElement = new FbxLayerElementUV *[UVSetCount];
		mappingMode = new FbxLayerElement::EMappingMode[UVSetCount];
		refMode = new FbxLayerElement::EReferenceMode[UVSetCount];
	}
	for (int32 UVIndex = 0; UVIndex < UVSetCount; ++UVIndex)
	{
		bool bFoundUV = false;
		layerElement[UVIndex] = NULL;
		for (int32 UVLayerIndex = 0; !bFoundUV&&UVLayerIndex < LayerCount; UVLayerIndex++)
		{
			FbxLayer * layer = fbxMesh->GetLayer(UVLayerIndex);
			UVSetCount = layer->GetUVSetCount();
			if (UVSetCount > 0)
			{
				FbxArray<const FbxLayerElementUV*> EleUVs = layer->GetUVSets();
				for (int32 FBXUVIndex = 0; FBXUVIndex < UVSetCount; ++FBXUVIndex)
				{
					FbxLayerElementUV const *ElementUV = EleUVs[FBXUVIndex];
					if (ElementUV)
					{
						const char *UVSetName = ElementUV->GetName();
						FString LocalUVSetName = ANSI_TO_TCHAR(UVSetName);
						if (LocalUVSetName == UVSet[UVIndex])
						{
							layerElement[UVIndex] = const_cast<FbxLayerElementUV *>(ElementUV);
							refMode[UVIndex] = layerElement[UVIndex]->GetReferenceMode();
							mappingMode[UVIndex] = layerElement[UVIndex]->GetMappingMode();
							break;
						}
					}
				}
			}
		}
	}
}

void UModelImporter::GetFbxMeshTriangleGroup(FbxMesh *fbxMesh, MeshInfo &meshInfo)
{
	//material refernce mode
	FbxGeometryElementMaterial *layerMaterial = fbxMesh->GetElementMaterial();
	if (layerMaterial != NULL)
	{
		FbxLayerElement::EReferenceMode MaterialReferenceMode(FbxLayerElement::eDirect);
		FbxLayerElement::EMappingMode MaterialMappingMode(FbxLayerElement::eByControlPoint);

		GetLayerElementRefAndMappingMode(layerMaterial, MaterialReferenceMode, MaterialMappingMode);

		meshInfo.TriangleGroups.SetNum(meshInfo.Model->RefMaterials.Num());
		for (int32 i = 0; i < meshInfo.TriangleGroups.Num(); ++i)
		{
			meshInfo.TriangleGroups[i].MaterialIndex = i;
		}

		MaterialMappingMode = FbxGeometryElement::eNone;
		FbxLayerElementArrayTemplate<int>* pMaterialIndices = &layerMaterial->GetIndexArray();
		for (int32 TriangleIndex = 0; TriangleIndex < meshInfo.NumTriangle; ++TriangleIndex)
		{
			int32 materialIndex = pMaterialIndices->GetAt(TriangleIndex);
			if (meshInfo.TriangleGroups.IsValidIndex(materialIndex))
			{
				meshInfo.TriangleGroups[materialIndex].Triangles.Add(TriangleIndex);
			}
		}

		for (int32 i = meshInfo.TriangleGroups.Num() - 1; i >= 0; --i)
		{
			if (meshInfo.TriangleGroups[i].Triangles.Num() == 0)
			{
				meshInfo.TriangleGroups.RemoveAt(i);
				meshInfo.Model->RefMaterials.RemoveAt(i);
			}
		}
	}

	if (meshInfo.TriangleGroups.Num() > 0)
	{
		meshInfo.TriangleGroups[0].StartIndex = 0;
		for (int32 i = 0; i < meshInfo.TriangleGroups.Num(); ++i)
		{
			TriGroup &Group = meshInfo.TriangleGroups[i];
			Group.EndIndex = Group.StartIndex + Group.Triangles.Num();
			if (i < meshInfo.TriangleGroups.Num() - 1)
			{
				meshInfo.TriangleGroups[i+1].StartIndex = Group.EndIndex;
			}
		}
	}
}

FVector UModelImporter::ToPosition(const FbxVector4 &vec)
{
	return FVector(float(-vec[0]), float(vec[1]), float(vec[2]));
}

FVector UModelImporter::ToVector(const FbxVector4 &vec)
{
	return FVector(-float(vec[0]), float(vec[1]), float(vec[2]));
}

FTransform UModelImporter::ToTransform(const FbxAMatrix &pose)
{
	FbxVector4 posVec = pose.GetT();
	FVector T = FVector(float(posVec[0]), float(posVec[1]), float(posVec[2]));
	FbxQuaternion Q = pose.GetQ();
	FQuat Quat(Q.GetAt(0), Q.GetAt(1), Q.GetAt(2), Q.GetAt(3));
	FTransform PoseTM = FTransform(Quat, T);
	return PoseTM;
}

