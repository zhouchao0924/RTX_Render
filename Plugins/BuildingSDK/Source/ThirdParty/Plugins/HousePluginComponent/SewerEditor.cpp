#include "SewerEditor.h"
#include "PluginMain.h"
#include "Class/Sewer.h"


ObjectID SewerEditor::AddSewer(float Diameter, const kPoint &Loc)
{
	IBuildingResourceMgr* ResMgr = GSDK->GetResourceMgr();
	ISuite* Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	float sewHeight = 0.5;
	if (Suite)
	{
		ObjectID SewerObjId = Suite->AddModel(SewerType);
		IObject* SewerObj = Suite->GetObject(SewerObjId);
		SewerObj->SetVector3D("Location", kVector3D(Loc, 0.55));
		if (SewerObj)
		{
			SetSewerAttribute(SewerObj, Diameter, sewHeight, Loc);		
		}
		
		return SewerObjId;
	}
	return INVALID_OBJID;
}

bool SewerEditor::UpdateSewerAttr(ObjectID ObjID, float Diameter, const kPoint &Loc)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);

		Sewer* SewObj = (Sewer*)PlugObj->GetPlugin();
		kXform RotMatrix = PlugObj->GetTransform();
		kPoint TLoc = SewObj->GetLocation();
		kVector3D OrigLoc = kVector3D(TLoc, 0.55);
		float DeltaX, DeltaY;
		DeltaX = Loc.X - OrigLoc.X;
		DeltaY = Loc.Y - OrigLoc.Y;
		float RadScale, DefaultRad;
		kPoint DefSize = SewObj->GetSize();
		DefaultRad = DefSize.X;
		if (Diameter>0.0)
		{
			RadScale = Diameter / 15.0;
			SewObj->SetDiam(Diameter);
		}
		else
		{
			float CurDiam = SewObj->GetDiam();
			RadScale = CurDiam / 15.0;
		}

		SewObj->SetLocation(Loc);
		TMatrix43Temp<float> ScaleMatrix, RotateMatrix, TransMatrix, TotalMatrix;
		ScaleMatrix.MakeIdentity();
		RotateMatrix.MakeIdentity();
		TransMatrix.MakeIdentity();
		TotalMatrix.MakeIdentity();

		ScaleMatrix.SetScale(RadScale, RadScale, 1.0);
		TransMatrix.SetOrigin(DeltaX, DeltaY, 0.55);
		TotalMatrix = ScaleMatrix *RotateMatrix*TransMatrix;

		/*calculate the world translation value of center point*/
		kVector3D DefLoc = SewObj->GetDefaultLoc();
		kVector3D DeltaPos = TotalMatrix.TransformVect(kVector3D(DefLoc.X,DefLoc.Y, 0.0));
		kVector3D ModVect = kVector3D(Loc, 0.55) - DeltaPos;

		RotMatrix.Scale = TotalMatrix.GetScale();
		RotMatrix.Location = ModVect + TotalMatrix.GetOrigin();

		//screening the illegal value
		if (IsNaN(RotMatrix.Location.X)||IsNaN(RotMatrix.Location.Y)||IsNaN(RotMatrix.Location.Z))
		{
			return false;
		}
		PlugObj->SetTransform(RotMatrix);
		PlugObj->MarkNeedUpdate(EChannelTransform);

		return true;
	}

	return false;
}

bool SewerEditor::SetSewerSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	bool IsSet = false;
	if (Suite)
	{
		IObject* Obj = Suite->GetObject(ObjID);
		IMeshObject* MeshObj = Obj->GetMeshObject();
		if (MeshObj)
		{
			int SectNum = MeshObj->GetSectionCount();
			if (SectionIndex < SectNum)
			{
				Obj->SetSurfaceUri(MeshObj, MaterialType, MaterialUri, SectionIndex);
				Obj->MarkNeedUpdate(EChannelSurface);
				IsSet = true;
			}
		}
	}
	return IsSet;
}

void SewerEditor::DeleteSewer(ObjectID ObjID)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject* Obj = Suite->GetObject(ObjID);
		IPluginObject* PlugObj = Obj->GetPlugin();
		delete PlugObj;

		Obj->Delete();
	}
}

//protected funcion
void SewerEditor::SetSewerAttribute(IObject* Obj, float Diameter, float Height, const kPoint &Loc)
{
	Sewer* SewerObj = (Sewer*)Obj->GetPlugin();
	if (SewerObj)
	{
		SewerObj->SetDiam(Diameter);
		SewerObj->SetHeght(Height);
		kVector3D ModLoc = kVector3D(Loc, 0.55f);
		SewerObj->SetDefaultLoc(ModLoc);
		SewerObj->SetLocation(Loc);
		kPoint DefSize = kPoint(Diameter, Height);
		SewerObj->SetSize(DefSize);
	}
}

//private function
void SewerEditor::SetSewerVertex(IObject* Obj, const kPoint &Location, float Diam, float Height)
{
	Sewer* SewerObj = (Sewer*)Obj->GetPlugin();
	if (SewerObj)
	{
		SewerMesh = Obj->GetMeshObject();
		if (SewerMesh)
		{
			SewerMeshBuilder = Obj->BeginMesh();
			//SetVertexData(Location, Diam, Height);
			SetCylinderData(Location, Diam, Height);
			SewerMeshBuilder->End();
		}
	}
	
}

void SewerEditor::SetCylinderData(const kPoint &Location, float Diam, float Height)
{
	std::string MaterialUri = "";
	int SurfaceID, SectIndex;
	SurfaceID = SewerMeshBuilder->NewSurface(ESurfaceType::ERefMaterial, MaterialUri.c_str());
	SectIndex = SewerMeshBuilder->NewSection(SurfaceID);

	int Segments = 200;
	float modHeight = 0.5;
	float radius = Diam / 2.0;
	
	kPoint WidVect = kPoint(radius, 0.0);
	kPoint LenVect = kPoint(0.0, radius);

	kPoint DownLeft, UpLeft, DownRight, UpRight;
	DownLeft = Location - WidVect;
	DownRight = Location - LenVect;
	UpRight = Location + WidVect;
	UpLeft = Location + LenVect;

	kVector3D V0, V1, V2, V3, V4, V5, V6, V7;
	V0 = kVector3D(DownLeft.x, DownLeft.y, 0.0);
	V1 = kVector3D(DownRight.x, DownRight.y, 0.0);
	V2 = kVector3D(UpRight.x, UpRight.y, 0.0);
	V3 = kVector3D(UpLeft.x, UpLeft.y, 0.0);
	V4 = kVector3D(DownLeft.x, DownLeft.y, modHeight);
	V5 = kVector3D(DownRight.x, DownRight.y, modHeight);
	V6 = kVector3D(UpRight.x, UpRight.y, modHeight);
	V7 = kVector3D(UpLeft.x, UpLeft.y, modHeight);
	kVector3D points[8]={ V0,V1,V2,V3,V4,V5,V6,V7 };
	
	kVector3D p0(V0.x, V0.y, modHeight);
	kVector3D pVLast(V7.x, V7.y, 0.0);
	kVector3D Normal(1.0, 0.0, 0.0);
	kVector3D tangent = kVector3D(0.0, 0.0, 1.0);
	kPoint UV = kPoint(0.5, 0.5);
	kPoint lightUV = kPoint(0, 0);

	float angle = 2*PI /(Segments);
	float sinA, cosA;
	/*sinA = Sin(angle);
	cosA = Cos(angle);*/
	int pStart, pLast;
	pStart = AddVert(SectIndex, p0, Normal, tangent, UV, lightUV);
	pLast = AddVert(SectIndex, pVLast, Normal, tangent, UV, lightUV);

	//temp points array
	std::vector<kVector3D> wp;
	for (size_t i = 0; i < 8; i++)
	{
		wp.push_back(points[i]);
	}
	//wp.resize(points->Size());
	for (int i = 0; i < Segments; i++)
	{
		sinA = Sin(angle*(i+1));
		cosA = Cos(angle*(i + 1));
		for (int j = 0; j < 7; j++)
		{
			kVector3D p1 = wp[j];
			kVector3D p2 = wp[j + 1];

			kVector3D DeltP1, DeltP2;
			DeltP1 = p1 - kVector3D(Location, 0.0);
			DeltP2 = p2 - kVector3D(Location, 0.0);
			float deltaX1, deltaX2, deltaY1, deltaY2;
			deltaX1 = (DeltP1.X) * cosA - (DeltP1.Y) * sinA;
			deltaY1 = (DeltP1.X) * sinA + (DeltP1.Y) * cosA;
			deltaX2 = (DeltP2.X) * cosA - (DeltP2.Y) * sinA;
			deltaY2 = (DeltP2.X) * sinA + (DeltP2.Y) * cosA;

			kVector3D p1r(Location.X+deltaX1, Location.Y+deltaY1, p1.Z);
			kVector3D p2r(Location.X + deltaX2, Location.Y + deltaY2, p2.Z);
			int indP1, indP1r, indP2, indP2r;
			indP1 = AddVert(SectIndex, p1, Normal, tangent, UV, lightUV);
			indP1r = AddVert(SectIndex, p1r, Normal, tangent, UV, lightUV);
			indP2 = AddVert(SectIndex, p2, Normal, tangent, UV, lightUV);
			indP2r = AddVert(SectIndex, p2r, Normal, tangent, UV, lightUV);

			if (j==0)
			{
				AddTri(SectIndex, indP1, pStart, indP1r);
			}
			AddTri(SectIndex, indP1, indP1r, indP2);
			AddTri(SectIndex, indP2, indP1r, indP2r);

			if (j==(points->Size()-2))
			{
				AddTri(SectIndex, indP2, indP2r, pLast);
				wp[j + 1] = p2r;
			}
			wp[j] = p1r;
		}
	}
}


int SewerEditor::AddVert(int SectionIndex, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV)
{
	return SewerMeshBuilder->AddVert(SectionIndex, Vert, Normal, Tan, UV, LightMapUV);
}

void SewerEditor::AddTri(int MeshSection, int V0, int V1, int V2)
{
	SewerMeshBuilder->AddTri(MeshSection, V0, V1, V2);
}
std::vector<kPoint> SewerEditor::GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3)
{
	std::vector<kPoint> UVArr;
	UVArr.clear();
	kVector3D tempVec = AdjVert2 - AdjVert0;
	/*float L = tempVec.z / (abs(tempVec.x) + abs(tempVec.y));*/
	float L = tempVec.z / 100.0f;
	float W = 0.0;
	if (abs(tempVec.x) < 0.005)
	{
		W = abs(tempVec.y) / 100.0;
	}
	else if (abs(tempVec.y) < 0.005)
	{
		W = abs(tempVec.x) / 100.0;
	}
	UVArr.push_back(kPoint(0, L));
	UVArr.push_back(kPoint(W, L));
	UVArr.push_back(kPoint(W, 0));
	UVArr.push_back(kPoint(0, 0));
	return UVArr;
}

void SewerEditor::SetDefUV(float wU, float lU)
{
	if (SewerMesh)
	{
		for (size_t i = 2; i < 5; i++)
		{
			float *pVertices = nullptr;
			float *pNormals = nullptr;
			float *pTangents = nullptr;
			float *pUVs = nullptr;
			float *pLightmapUVs = nullptr;
			int nVerts = 0, nIndices = 0;
			int *pIndices = nullptr;
			bool IsGetSect = SewerMesh->GetSectionMesh(i, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices);
			if (IsGetSect)
			{
				if (i == 2 || i == 3)
				{
					pUVs[2] = lU;
					pUVs[4] = lU;
				}
				else
				{
					pUVs[2] = wU;
					pUVs[4] = wU;
				}
			}
		}

	}
}