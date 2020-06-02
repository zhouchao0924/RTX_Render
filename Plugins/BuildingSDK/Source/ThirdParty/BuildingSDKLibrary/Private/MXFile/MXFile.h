
#pragma once

#include "IMeshObject.h"
#include "MXTexture.h"
#include "ISerialize.h"
#include "Class/Object.h"
#include "Mesh/MeshObject.h"
#include "Mesh/SurfaceObject.h"

enum EResType
{
	EResSurface,
	EResModel,
	EResScene,
	EResUnknown
};

struct FChunk
{
	int		Offset;
	int		Size;
	void Serialize(ISerialize &Ar, int Ver);
};

#define  RESOURCE_HEADERVER_1	1			//->2	jason DRInfo
#define  RESOURCE_HEADERVER_2	2			//->3	Header 
#define  RESOURCE_HEADERVER		3

struct FResourceSummary
{
	FResourceSummary();
	EResType					ResType;
	int							HeadVersion;
	std::string					ResID;
	std::string					ResourceName;
	int							LocalVersion;
	int							ModifyVersion;
	bool						bCompressed;
	std::vector<std::string>	Dependences;
	int							BodyVersion;

	struct {
		int					SizeX;
		int					SizeY;
		std::vector<char>	Data;
	} Image;

	virtual void  Serialize(ISerialize &Ar);
	virtual int   GetHeadVer() { return RESOURCE_HEADERVER; }
	virtual int   GetCodeVer() { return 0; }
};

enum ECenterAdjustType
{
	Original,

	BottomCenter,
	TopCenter,
	LeftCenter,
	RightCenter,
	FrontCenter,
	BackCenter,

	TopFrontEdgeCenter,
	TopBackEdgeCenter,
	TopLeftEdgeCenter,
	TopRightEdgeCenter,

	BottomFrontEdgeCenter,
	BottomBackEdgeCenter,
	BottomLeftEdgeCenter,
	BottomRightEdgeCenter,

	LeftFrontEdgeCenter,
	RightFrontEdgeCenter,
	LeftBackEdgeCenter,
	RightBackEdgeCenter
};

#define  MODELFILE_HEADER_VER	3
#define  MODELFILE_BODY_VER_1	1	// -> 2  collision
#define  MODELFILE_BODY_VER_2	2   // -> 3  UCX  
#define  MODELFILE_BODY_VER_3	3	// -> 4	 Offset
#define  MODELFILE_BODY_VER_4	4	// -> 5  Scale
#define  MODELFILE_BODY_VER_5	5	// -> 6  Depth Width Height replace scale3d
#define  MODELFILE_BODY_VER_6	6	// -> 7  FModel ModelName
#define  MODELFILE_BODY_VER_7	7	// -> 8  SurfaceObject  { OverrideVec2Parameters  OverrideVec3Parameters }
#define  MODELFILE_BODY_VER		8

class MXFile : public Object, public IMeshObjectCallback
{
	DEFIN_CLASS()

	struct FHeader : public FResourceSummary
	{
		std::vector<FChunk>	Models;
		std::vector<FChunk>	Materials;
		std::vector<FChunk>	Textures;
		void Serialize(ISerialize &Ar);
		int GetHeadVer() override { return MODELFILE_HEADER_VER; }
		int GetCodeVer() override { return MODELFILE_BODY_VER; }
	};

	struct FViewLoc
	{
		kVector3D	EyeLoc;
		kVector3D	FocusLoc;
		float		EyeDistance;
	};
public:
	MXFile();
	FResourceSummary *GetSummary() { return &m_Header; }
	static MXFile *LoadFromFile(const char *InFilename);
	int   GetMeshCount() override { return (int)Meshes.size(); }
	IMeshObject *GetMeshObject(int MeshIndex, bool bBuildIfNotExist = true);
	int  GetSurfaceCount() override { return (int)m_Materials.size(); }
	ISurfaceObject *GetSurfaceObject(int SurfaceIndex) override { return m_Materials[SurfaceIndex]; }
	int   GetTextureCount() { return (int)m_Textures.size(); }
	bool  GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, int &Format) override;
	bool  GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes) override;
	void Serialize(ISerialize &Ar);
	void SerializeHeader(ISerialize &Ar);
	void ResetSize();
	void CaclScale3D();
	ISurfaceObject *GetMeshSurface(int SurfaceID) override;
	void Link(int SurfaceID, int SubModelIndex) override;
	void UnLink(int SurfaceID, int SubModelIndex) override;
protected:
	int								Id;
	std::string						URL;
	std::string						JsonStr;
	FViewLoc						ViewLoc;
	FHeader							m_Header;
	std::vector<SurfaceObject*>		m_Materials;
	std::vector<FModelTexture*>		m_Textures;
public:
	ECenterAdjustType				CenterType;
	kVector3D						Offset;
	int								DepthInMM;
	int								WidthInMM;
	int								HeightInMM;
	kBox3D							LocalBounds;
	std::string						Filename;
	kVector3D						Scale3D;
	std::vector<MeshObject*>		Meshes;
};

