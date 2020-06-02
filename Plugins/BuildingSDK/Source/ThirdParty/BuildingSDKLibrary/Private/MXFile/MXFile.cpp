
#include "assert.h"
#include "MXFile.h"
#include "MXTexture.h"
#include "IBuildingSDK.h"
#include "Mesh/MeshObject.h"
#include "Stream/FileStream.h"

static std::string FingerMark = "1*&343!2)*Yds;Qd^`}/?3,";

std::string LoadStr(ISerialize &Ar)
{
	std::string Value;
	assert(Ar.IsLoading());

	int Len = 0;
	Ar << Len;
	bool LoadUCS2Char = Len < 0;
	if (LoadUCS2Char)
	{
		Len = -Len;
	}

	if (Len > 0)
	{
		if (LoadUCS2Char)
		{
			std::vector<wchar_t> str(Len);
			Ar.Serialize(&str[0], Len * sizeof(wchar_t));
			Value = (char *)&str[0];
		}
		else
		{
			std::vector<char> str(Len);
			Ar.Serialize(&str[0], Len * sizeof(char));
			Value = &str[0];
		}
	}

	return Value;
}

void SaveStr(ISerialize &Ar, std::string &str)
{

}

bool LoadBool(ISerialize &Ar)
{
	int Value = 0;
	Ar << Value;
	return Value != 0;
}

kBox3D LoadBox(ISerialize &Ar)
{
	kBox3D Value;
	char bValid;
	Ar << Value;
	Ar << bValid;
	return Value;
}

void SaveBox(ISerialize &Ar, kBox3D box)
{

}

void FChunk::Serialize(ISerialize &Ar, int Ver)
{
	Ar << Offset;
	Ar << Size;
}

FResourceSummary::FResourceSummary()
	: bCompressed(0)
	, HeadVersion(-1)
	, ResType(EResUnknown)
	, ModifyVersion(0)
{
}

void  FResourceSummary::Serialize(ISerialize &Ar)
{
	HeadVersion = GetHeadVer();
	BodyVersion = GetCodeVer();

	if (Ar.IsLoading())
	{
		Ar << HeadVersion;
		ResID = LoadStr(Ar);
		Ar << LocalVersion;
		Ar << bCompressed;
		ResourceName = LoadStr(Ar);

		//ver2 <--
		if (HeadVersion <= RESOURCE_HEADERVER_2)
		{
			int	SizeX, SizeY, SizeZ;
			std::string Brand, Subfamily, CategoryName, Desc;
			char	PerfPosition;
			bool	bUsePhysics;
			Brand = LoadStr(Ar);
			Subfamily = LoadStr(Ar);
			CategoryName = LoadStr(Ar);
			Ar << PerfPosition;
			Ar << bUsePhysics;
			Ar << SizeX;
			Ar << SizeY;
			Ar << SizeZ;
			Ar << Desc;
		}

		if (HeadVersion > RESOURCE_HEADERVER_2)
		{
			Ar << ModifyVersion;
		}

		int NumDeps = 0;
		Ar << NumDeps;
		Dependences.resize(NumDeps);
		for (int i = 0; i < Dependences.size(); ++i)
		{
			Dependences[i] = LoadStr(Ar);
		}

		Ar << BodyVersion;
	}
	else if (Ar.IsSaving())
	{
		Ar << HeadVersion;
		Ar << ResID;
		Ar << LocalVersion;
		Ar << bCompressed;
		Ar << ResourceName;
		Ar << ModifyVersion;

		int NumDeps = Dependences.size();
		Ar << NumDeps;
		for (int i = 0; i < Dependences.size(); ++i)
		{
			Ar << Dependences[i];
		}

		Ar << BodyVersion;
	}
}

MXFile::MXFile()
	:Scale3D(1.0f)
{
}

BEGIN_DERIVED_CLASS(MXFile, Object)
END_CLASS()

void MXFile::FHeader::Serialize(ISerialize &Ar)
{
	FResourceSummary::Serialize(Ar);

	if (Ar.IsSaving())
	{
		Ar << Image.SizeX;
		Ar << Image.SizeY;

		int NumBytes = Image.Data.size();
		Ar << NumBytes;
		if (NumBytes > 0)
		{
			Ar.Serialize(&Image.Data[0], NumBytes);
		}

		int NumModel = Models.size();
		Ar << NumModel;
		for (int i = 0; i < NumModel; ++i)
		{
			Models[i].Serialize(Ar, BodyVersion);
		}

		int NumMtrl = Materials.size();
		Ar << NumMtrl;
		for (int i = 0; i < NumMtrl; ++i)
		{
			Materials[i].Serialize(Ar, BodyVersion);
		}

		int NumTex = Textures.size();
		Ar << NumTex;
		for (int i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}
	}
	else if (Ar.IsLoading())
	{
		Ar << Image.SizeX;
		Ar << Image.SizeY;

		int NumBytes = 0;
		Ar << NumBytes;
		Image.Data.resize(NumBytes);
		if (NumBytes > 0)
		{
			Ar.Serialize(&Image.Data[0], NumBytes);
		}

		int NumModel = 0;
		Ar << NumModel;
		Models.resize(NumModel);
		for (int i = 0; i < NumModel; ++i)
		{
			Models[i].Serialize(Ar, BodyVersion);
		}

		int NumMtrl = 0;
		Ar << NumMtrl;
		Materials.resize(NumMtrl);
		for (int i = 0; i < NumMtrl; ++i)
		{
			Materials[i].Serialize(Ar, BodyVersion);
		}

		int NumTex = 0;
		Ar << NumTex;
		Textures.resize(NumTex);
		for (int i = 0; i < NumTex; ++i)
		{
			Textures[i].Serialize(Ar, BodyVersion);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SaveMark(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		Ar << FingerMark;
	}
}

bool CheckMark(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		std::string Mark = LoadStr(Ar);
		if (Mark == FingerMark)
		{
			return true;
		}
	}
	return false;
}

bool SkipMark(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		char resType = 0;
		Ar << resType;
		return CheckMark(Ar);
	}
	return false;
}

MXFile *MXFile::LoadFromFile(const char *InFilename)
{
	MXFile *mxFile = NULL;

	ISerialize *Reader = GetBuildingSDK()->CreateFileReader(InFilename);
	if (Reader)
	{
		if (SkipMark(*Reader))
		{
			mxFile = new MXFile();
			mxFile->Filename = InFilename;
			mxFile->Serialize(*Reader);
		}
		Reader->Close();
	}

	return mxFile;
}

void MXFile::ResetSize()
{
	kVector3D Size = LocalBounds.GetExtent()*Scale3D*10.0f;
	DepthInMM = std::round(Size.X);
	WidthInMM = std::round(Size.Y);
	HeightInMM = std::round(Size.Z);
}

void MXFile::CaclScale3D()
{
	kBox3D Bounds = LocalBounds;
	kVector3D Size = Bounds.GetExtent();
	Scale3D.X = DepthInMM *0.1f / Size.X;
	Scale3D.Y = WidthInMM *0.1f / Size.Y;
	Scale3D.Z = HeightInMM*0.1f / Size.Z;
}

ISurfaceObject *MXFile::GetMeshSurface(int Index)
{
	if (Index >= 0 && Index < m_Materials.size())
	{
		return m_Materials[Index];
	}
	return nullptr;
}

void MXFile::Link(int SurfaceID, int SubModelIndex)
{
}

void MXFile::UnLink(int SurfaceID, int SubModelIndex)
{
}

void MXFile::Serialize(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		int HeaderPos = Ar.Tell();

		int NumModels = Meshes.size();
		m_Header.Models.resize(NumModels);

		int NumMaterials = m_Materials.size();
		m_Header.Materials.resize(NumMaterials);

		int NumTextures = m_Textures.size();
		m_Header.Textures.resize(NumTextures);

		SerializeHeader(Ar);

		//serialize models
		for (int iModel = 0; iModel < NumModels; ++iModel)
		{
			FChunk &modelChunk = m_Header.Models[iModel];
			modelChunk.Offset = (int)Ar.Tell();
			Meshes[iModel]->Serialize(Ar, m_Header.BodyVersion);
			modelChunk.Size = int(Ar.Tell() - modelChunk.Offset);
		}

		//serialize materials
		for (int iMaterial = 0; iMaterial < NumMaterials; ++iMaterial)
		{
			FChunk &mtrlChunk = m_Header.Materials[iMaterial];
			mtrlChunk.Offset = (int)Ar.Tell();

			int type = (int)m_Materials[iMaterial]->GetType();
			Ar << type;

			m_Materials[iMaterial]->Serialize(Ar, m_Header.BodyVersion);
			mtrlChunk.Size = int(Ar.Tell() - mtrlChunk.Offset);
		}

		//serialze texture
		for (int iTexture = 0; iTexture < m_Header.Textures.size(); ++iTexture)
		{
			FChunk &texChunk = m_Header.Textures[iTexture];
			texChunk.Offset = (int)Ar.Tell();
			m_Textures[iTexture]->Serialize(Ar, m_Header.BodyVersion);
			texChunk.Size = int(Ar.Tell() - texChunk.Offset);
		}

		Ar << LocalBounds;
		Ar << ViewLoc.EyeLoc;
		Ar << ViewLoc.FocusLoc;
		Ar << ViewLoc.EyeDistance;

		if (m_Header.BodyVersion > MODELFILE_BODY_VER_3)
		{
			KSERIALIZE_ENUM(ECenterAdjustType, CenterType);
			Ar << Offset;
		}

		if (m_Header.BodyVersion == MODELFILE_BODY_VER_5)
		{
			Ar << Scale3D;
			ResetSize();
		}
		else if (m_Header.BodyVersion > MODELFILE_BODY_VER_5)
		{
			Ar << DepthInMM;
			Ar << WidthInMM;
			Ar << HeightInMM;
			CaclScale3D();
		}
		else
		{
			ResetSize();
		}

		int TailPos = Ar.Tell();
		Ar.Seek(HeaderPos);

		SerializeHeader(Ar);

		Ar.Seek(TailPos);
	}
	else if (Ar.IsLoading())
	{
		SerializeHeader(Ar);

		//serialize models
		Meshes.resize(m_Header.Models.size());
		for (int iModel = 0; iModel < m_Header.Models.size(); ++iModel)
		{
			FChunk modelChunk = m_Header.Models[iModel];
			Ar.Seek(modelChunk.Offset);

			MeshObject *meshObj = new MeshObject(nullptr);
			meshObj->SetMeshObjectCallback(this);
			meshObj->Serialize(Ar, m_Header.BodyVersion);
			Meshes[iModel] = meshObj;
		}

		//serialize materials
		for (int iMaterial = 0; iMaterial < m_Header.Materials.size(); ++iMaterial)
		{
			FChunk mtrlChunk = m_Header.Materials[iMaterial];
			Ar.Seek(mtrlChunk.Offset);

			int type = 0;
			Ar << type;

			SurfaceObject *pSurface = new SurfaceObject();
			pSurface->Serialize(Ar, m_Header.BodyVersion);
			m_Materials.push_back(pSurface);
		}

		//serialze texture
		for (int iTexture = 0; iTexture < m_Header.Textures.size(); ++iTexture)
		{
			FChunk texChunk = m_Header.Textures[iTexture];
			Ar.Seek(texChunk.Offset);
			FModelTexture *newTexutre = new FModelTexture();
			newTexutre->Serialize(Ar, m_Header.BodyVersion);
			m_Textures.push_back(newTexutre);
		}

		LocalBounds = LoadBox(Ar);
		Ar << ViewLoc.EyeLoc;
		Ar << ViewLoc.FocusLoc;
		Ar << ViewLoc.EyeDistance;
	}
}

void MXFile::SerializeHeader(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		Ar << Id;
		Ar << URL;

		FResourceSummary *Header = GetSummary();
		if (Header)
		{
			Header->Serialize(Ar);
		}
		Ar << JsonStr;
	}
	else if (Ar.IsLoading())
	{
		Ar << Id;
		URL = LoadStr(Ar);

		FResourceSummary *Header = GetSummary();
		if (Header)
		{
			Header->Serialize(Ar);
			if (Header->HeadVersion > RESOURCE_HEADERVER_1)
			{
				JsonStr = LoadStr(Ar);
			}
		}
	}
}

IMeshObject *MXFile::GetMeshObject(int MeshIndex/* = 0*/, bool bBuildIfNotExist /*= true*/)
{
	IMeshObject *MeshObj = nullptr;
	if (MeshIndex >= 0 && MeshIndex < Meshes.size())
	{
		MeshObj = Meshes[MeshIndex];
	}
	return MeshObj;
}

bool MXFile::GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, int &Format)
{
	if (iTex >= 0 && iTex < m_Textures.size())
	{
		FModelTexture *pTexture = m_Textures[iTex];
		if (pTexture)
		{
			Width = pTexture->Source.SizeX;
			Height = pTexture->Source.SizeY;
			MipCount = (int)pTexture->Source.CompressedImages.size();
			Pitch = Width * 2;
			Format = pTexture->Source.CompressedImages[0].PixelFormat;
			return true;
		}
	}
	return false;
}

bool MXFile::GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes)
{
	if (iTex >= 0 && iTex < m_Textures.size())
	{
		FModelTexture *pTexture = m_Textures[iTex];
		if (pTexture && MipIndex >= 0 && MipIndex < pTexture->Source.CompressedImages.size())
		{
			FTextureSourceInfo::FCompressedImage2D &Image = pTexture->Source.CompressedImages[MipIndex];
			pData = &Image.RawData[0];
			nBytes = (int)Image.RawData.size();
			return true;
		}
	}
	return false;
}


