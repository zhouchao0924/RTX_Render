
#pragma once

#include "Platform.h"

#if RS_PLATFORM==PLATFORM_WINDOWS && !USE_MX_ONLY
#include "fbxsdk.h"

class ImporterImpl
{
public:
	ImporterImpl();
	IObject *Import(const char *fbxFilename) override;
	void Initialize();
	void UnInitialize();
protected:
	bool InitFbxScene(const char *fbxFilename);
	void DestroyFbxScene();
	void AddMesh(FbxMesh *Mesh);
	void ImportMesh(FbxMesh *Mesh);
	void ImportNaterial(FbxSurfaceMaterial *fbxMaterial);
	int  ImportTexture(ETexSlot slot, const std::string &texFilename);
	void GetFbxMeshUVSet(FbxMesh *fbxMesh, std::vector<std::string> &UVSets);
	void GetFbxMeshMaterials(FbxMesh *fbxMesh);
protected:
	FbxManager				*m_pFbxManger;
	FbxScene				*m_pScene;
	FbxImporter				*m_Importer;
	std::vector<FbxMesh *>	 m_Meshes;
};

#endif

