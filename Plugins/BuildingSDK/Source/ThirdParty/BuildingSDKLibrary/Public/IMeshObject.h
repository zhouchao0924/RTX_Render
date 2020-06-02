
#pragma once

#include "IClass.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kColor.h"
#include "Math/kString.h"

enum EUVGenMethod
{
	EUV_World,
	EUV_LeftTop
};

class IMeshObject
{
public:
	virtual const char *GetName() = 0;
	virtual kBox3D GetBounds() = 0;
	virtual ISurfaceObject *GetSurfaceObject(int SectionIndex) = 0;
	virtual int  GetSectionCount() = 0;
	virtual bool GetSectionMesh(int SectionIndex, float *&pVertices, float *&pNormals, float *&pTangents, float *&pUVs, float *&pLightmapUVs, int &NumVerts, int *&pIndices, int &NumIndices) = 0;
	virtual bool HitTest(const kVector3D &RayStart, const kVector3D &RayDir, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr, float RayLen = 100000.0f) = 0;
	virtual int  GetLineCount() = 0;
	virtual void GetLine(int index, const kVector3D *&pVertices, int &NumVerts) = 0;
	virtual int  GetConvexCollisonCount() = 0;
	virtual bool GetConvexCollision(int iConvex, kArray<kVector3D> &Vertices, kBox3D &BoundBox) = 0;
};

enum EPFormat
{
	EPF_Unknown = 0,
	EPF_A32B32G32R32F = 1,
	EPF_B8G8R8A8 = 2,
	EPF_G8 = 3,
	EPF_G16 = 4,
	EPF_DXT1 = 5,
	EPF_DXT3 = 6,
	EPF_DXT5 = 7,
	EPF_UYVY = 8,
	EPF_FloatRGB = 9,
	EPF_FloatRGBA = 10,
	EPF_DepthStencil = 11,
	EPF_ShadowDepth = 12,
	EPF_R32_FLOAT = 13,
	EPF_G16R16 = 14,
	EPF_G16R16F = 15,
	EPF_G16R16F_FILTER = 16,
	EPF_G32R32F = 17,
	EPF_A2B10G10R10 = 18,
	EPF_A16B16G16R16 = 19,
	EPF_D24 = 20,
	EPF_R16F = 21,
	EPF_R16F_FILTER = 22,
	EPF_BC5 = 23,
	EPF_V8U8 = 24,
	EPF_A1 = 25,
	EPF_FloatR11G11B10 = 26,
	EPF_A8 = 27,
	EPF_R32_UINT = 28,
	EPF_R32_SINT = 29,
	EPF_PVRTC2 = 30,
	EPF_PVRTC4 = 31,
	EPF_R16_UINT = 32,
	EPF_R16_SINT = 33,
	EPF_R16G16B16A16_UINT = 34,
	EPF_R16G16B16A16_SINT = 35,
	EPF_R5G6B5_UNORM = 36,
	EPF_R8G8B8A8 = 37,
	EPF_A8R8G8B8 = 38,	// Only used for legacy loading; do NOT use!
	EPF_BC4 = 39,
	EPF_R8G8 = 40,
	EPF_ATC_RGB = 41,
	EPF_ATC_RGBA_E = 42,
	EPF_ATC_RGBA_I = 43,
	EPF_X24_G8 = 44,	// Used for creating SRVs to alias a DepthStencil buffer to read Stencil. Don't use for creating textures.
	EPF_ETC1 = 45,
	EPF_ETC2_RGB = 46,
	EPF_ETC2_RGBA = 47,
	EPF_R32G32B32A32_UINT = 48,
	EPF_R16G16_UINT = 49,
	EPF_ASTC_4x4 = 50,	// 8.00 bpp
	EPF_ASTC_6x6 = 51,	// 3.56 bpp
	EPF_ASTC_8x8 = 52,	// 2.00 bpp
	EPF_ASTC_10x10 = 53,	// 1.28 bpp
	EPF_ASTC_12x12 = 54,	// 0.89 bpp
	EPF_BC6H = 55,
	EPF_BC7 = 56,
	EPF_R8_UINT = 57,
	EPF_L8 = 58,
	EPF_XGXR8 = 59,
	EPF_R8G8B8A8_UINT = 60,
	EPF_R8G8B8A8_SNORM = 61,
	EPF_R16G16B16A16_UNORM = 62,
	EPF_R16G16B16A16_SNORM = 63,
	EPF_MAX = 64,
};

enum ESurfaceType
{
	ERefMaterial,
	EUE4Material,
};

class ISurfaceObject;
class IMeshBuilder
{
public:
	virtual void Begin(IObject *Object) = 0;

	virtual IMeshObject *GetMeshObject() = 0;
	virtual int  GetSurfaceCount() = 0;
	virtual int  NewSurface(ESurfaceType SurfaceType, const char *Uri) = 0;
	virtual ISurfaceObject *GetSurface(int SurfaceIndex) = 0;
	virtual void SetSurface(int SectionIndex, int SurfaceIndex) = 0;
	
	virtual int  GetTextureCount() = 0;
	virtual int  NewTexture(const char *filename) = 0;
	virtual int  NewTexture(int SizeX, int SizeY,  unsigned char *RawData, bool bGrayImage = false, bool bHasAlpha = true) = 0; //bGrayImage: if true 8 bit pre pixel  else 32 bit pre pixel
	
	virtual int  GetSectionCount() = 0;
	virtual int  NewSection(int SurfaceIndex) = 0;
	virtual int  AddVert(int SectionIndex, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV) = 0;
	virtual void AddTri(int SectionIndex, int V0, int V1, int V2) = 0;
	virtual int  AddConvexCollision(kVector3D *pVertices, int NumVerts) = 0;
	virtual int  GetConvexCount() = 0;

	virtual void End() = 0;
};

class ISurfaceObject
{
public:
	virtual int GetUserID() = 0;
	virtual IObject *GetObject() = 0;

	virtual int GetSurfaceType() = 0;
	virtual const kString &GetName() = 0;
	virtual const char *GetUri() = 0;
	virtual const char *GetRootUri() = 0;

	virtual void SetScalar(const CharPtr &name, float Value) = 0;
	virtual void SetVector2D(const CharPtr &name, const kPoint &Value) = 0;
	virtual void SetVector3D(const CharPtr &name, const kVector3D &Value) = 0;
	virtual void SetVector(const CharPtr &name, const kVector4D &Value) = 0;

	virtual float GetScalar(const CharPtr &name) = 0;
	virtual kPoint GetVector2D(const CharPtr &name) = 0;
	virtual kVector3D GetVector3D(const CharPtr &name) = 0;
	virtual kVector4D GetVector(const CharPtr &name) = 0;

	virtual int  GetTextureIndex(const CharPtr &name) = 0;
	virtual void SetTextureIndex(const CharPtr &name, int index) = 0;
	virtual int  GetTextureIndex(ETexSlot slot) = 0;
	virtual void SetTextureIndex(ETexSlot slot, int index) = 0;
	virtual ISurfaceTexture *GetTexture(int index) = 0;

	virtual IValue *FindParamValue(const CharPtr &name) = 0;

	virtual int GetScalarCount() = 0;
	virtual int GetVector2DCount() = 0; 
	virtual int GetVector3DCount() = 0;
	virtual int GetVectorCount() = 0;
	
	virtual float GetScalar(int ParamIndex, CharPtr &OutParamname) = 0;
	virtual kPoint GetVector2D(int ParamIndex, CharPtr &OutParamname) = 0;
	virtual kVector3D GetVector3D(int ParamIndex, CharPtr &OutParamname) = 0;
	virtual kVector4D GetVector(int ParamIndex, CharPtr &OutParamname) = 0;
};

class ISurfaceTexture
{
public:
	virtual IObject	*GetObject() = 0;
	virtual int		 GetMipCount() = 0;
	virtual EPFormat GetCompressedFormat(int MipIndex) = 0;
	virtual bool	GetMipInfo(int MipIndex, int &SizeX, int &SizeY, kArray<unsigned char> &RawData, EPFormat &PxielFormat) = 0;
	virtual void	GetSource(int &SizeX, int &SizeY, kArray<unsigned char> &RawData, EPFormat &PixelFormat) = 0;
	virtual void	SetCompressedImage(int MipIndex, EPFormat PFormat, int SizeX, int SizeY, const unsigned char *pCompressedData, unsigned int szData) = 0;
	virtual bool	SaveTextureToFile(int mipIndex, kArray<unsigned char> *ImageData, const char *Filename) = 0;
};



