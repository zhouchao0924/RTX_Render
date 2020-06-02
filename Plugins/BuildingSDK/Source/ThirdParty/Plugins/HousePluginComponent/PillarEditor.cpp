#include "PillarEditor.h"
#include "PluginMain.h"
#include "Class/Pillar.h"



ObjectID PillarEditor::AddPillar(bool IsAnch, float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		ObjectID PillarObjID = Suite->AddModel(PillarType);
		IObject* PluginObj = Suite->GetObject(PillarObjID);
		
		//get wallDirect vector
		kPoint WallDirVect= kPoint(-Forward.Y, Forward.X);
		
		SetPillarAttr(PluginObj, Height, Width, Length, IsAnch, Loc,Forward);
		SetMeshData(PluginObj, Height, Width, Length, Loc, Forward, WallDirVect);
		
		return PillarObjID;
	}

	return INVALID_OBJID;
}


bool PillarEditor::UpdatePillar(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject* PluginObj = Suite->GetObject(ObjID);
		Pillar* PillarPlug = (Pillar*)PluginObj->GetPlugin();
		PillarMesh = PluginObj->GetMeshObject(0, false);

		if (true)
		{
			bool UpdateBool = false;

			//set the original angle of pillar primitive
			float BaseYawVal = 0.0;
			bool IsSetAng = PillarPlug->GetIsAngle();
			if (!IsSetAng)
			{
				PillarPlug->SetIsAngle(true);
				PillarPlug->SetOrigAngle(RotAngle);
			}
			BaseYawVal = RotAngle;

			kVector3D ChangeVect;
			kXform RotMatrix = PluginObj->GetTransform();
			kPoint OrigLoc = PillarPlug->GetLocation();
			kPoint DefLoc = PillarPlug->GetDefaultLoc();
			kPoint ChangeLoc = Loc;
			kVector3D RotScale = RotMatrix.GetMatrix().GetScale();
			//calculate the component transform value
			TMatrix43Temp<float> ScaleMatrix, RotateMatrix, TransMatrix, TotalMatrix;
			TotalMatrix.MakeIdentity();
			ScaleMatrix.MakeIdentity();
			RotateMatrix.MakeIdentity();
			TransMatrix.MakeIdentity();

			float DeltaX, DeltaY;
			DeltaX = ChangeLoc.X - OrigLoc.X;
			DeltaY = ChangeLoc.Y - OrigLoc.Y;

					
			float DForwAng = BaseYawVal;
			float RForwAng = BaseYawVal * PI / 180.0;

			ScaleMatrix.SetScale(RotScale.X, RotScale.Y, RotScale.Z);
			RotateMatrix.SetRotationZ(RForwAng);
			TransMatrix.SetOrigin(DeltaX, DeltaY, 0);
			TotalMatrix = ScaleMatrix * RotateMatrix*TransMatrix;

			kVector3D DeltaPos = TotalMatrix.TransformVect(kVector3D(DefLoc, 0.0));
			ChangeVect = kVector3D(ChangeLoc, 0.0) - DeltaPos;
	
			kVector3D RotVect = TotalMatrix.GetRotationXYZ();
			RotMatrix.Scale = TotalMatrix.GetScale();
			RotMatrix.Rotation = kRotation(RotVect.Y, DForwAng, RotVect.X);
			RotMatrix.Location = ChangeVect + TotalMatrix.GetOrigin();
				
			PillarPlug->SetLocation(ChangeLoc);

			PluginObj->SetTransform(RotMatrix);
			PluginObj->MarkNeedUpdate(EChannelTransform);
			UpdateBool = true;
			
			//the illegal value
			if (isnan(RotMatrix.Location.X) || isinf(RotMatrix.Location.X) || isnan(RotMatrix.Location.Y) || isinf(RotMatrix.Location.Y) ||
				isnan(RotMatrix.Location.Z) || isinf(RotMatrix.Location.Z))
			{
				return false;
			}
			ModulateNormalVal(RotMatrix);
			
			BorderVect = PillarPlug->GetOrigBorder();
			for (size_t i = 0; i < BorderVect.size(); i++)
			{
				kVector3D OrigBoder = BorderVect.at(i);
				kVector3D TransBord = TotalMatrix.TransformVect(OrigBoder)+ChangeVect;
				BorderVect.at(0) = TransBord;
			}
			PillarPlug->SetBorder(BorderVect);
			BorderVect.clear();

			return UpdateBool;
		}
	}

	return false;
}

bool PillarEditor::UpdatePillarAttr(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject *PlugObj = Suite->GetObject(ObjID);
		Pillar *PillarObj = (Pillar*)PlugObj->GetPlugin();
		PillarMesh = PlugObj->GetMeshObject(0, false);

		//set model transform value
		kXform RotMatrix = PlugObj->GetTransform();
		kPoint OrigLoc = PillarObj->GetLocation();
		float DeltaX, DeltaY;
		DeltaX = Loc.X - OrigLoc.X;
		DeltaY = Loc.Y - OrigLoc.Y;
	
		//set pole plugin angle value
		bool IsSetAng = PillarObj->GetIsAngle();
		if (!IsSetAng)
		{
			PillarObj->SetIsAngle(true);
			PillarObj->SetOrigAngle((Angle));
		}

		//calculate the scale of x&y anxis
		float WScale, LScale, ZScale, OrigWidth, OrigLen, OrigHeight, DefaultW, DefaultL,DefaultZ;
		kVector3D DefSize = PillarObj->GetSize();
		DefaultW = DefSize.X;
		DefaultL = DefSize.Y;
		DefaultZ = DefSize.Z;

		if (Width > 0.0 && Length > 0.0)
		{
			OrigWidth = Width;
			OrigLen = Length;
			PillarObj->SetLength(Length);
			PillarObj->SetWidth(Width);
			float widUV, lenUV;
			widUV = Width / 100.0;
			lenUV = Length / 100.0;
			SetDefUV(widUV, lenUV);
		}
		else
		{
			OrigWidth = PillarObj->GetWidth();
			OrigLen = PillarObj->GetLength();
		}
		OrigHeight = PillarObj->GetHeight();

		WScale = OrigWidth / DefaultW;
		LScale = OrigLen / DefaultL;
		ZScale = OrigHeight / 280.0;
		
		//modulate the width and length scale value
		float RotDegree = Angle;
		float RotRadian = RotDegree * PI / 180.0;
		//calculate tube model change matrix

		//define the identity matrix for keeping the scale, rotate and translate vector
		//the changing order is scale->rotate->translate
		TMatrix43Temp<float> ScaleMatrix, RotateMatrix, TransMatrix, TotalMatrix;
		TotalMatrix.MakeIdentity();
		ScaleMatrix.MakeIdentity();
		RotateMatrix.MakeIdentity();
		TransMatrix.MakeIdentity();

		
		/*set the scale matrix*/
		ScaleMatrix.SetScale(WScale, LScale, ZScale);
		RotateMatrix.SetRotationZ(RotRadian);
		TransMatrix.SetOrigin(DeltaX, DeltaY, 0.0);
		TotalMatrix = ScaleMatrix * RotateMatrix*TransMatrix;

		/*compute the world translate of center location*/
		kPoint DefaultLoc = PillarObj->GetDefaultLoc();
		kVector3D DeltaPos = TotalMatrix.TransformVect(kVector3D(DefaultLoc, 0.0));
		kVector3D ChangeVect = kVector3D(Loc, 0.0) - DeltaPos;

		RotMatrix.Scale = TotalMatrix.GetScale();
		kVector3D TotalRotVal = TotalMatrix.GetRotationXYZ();
		RotMatrix.Rotation = kRotation(TotalRotVal.Y, RotDegree, TotalRotVal.X);
		RotMatrix.Location = TotalMatrix.GetOrigin() + ChangeVect;

		//the illegal value
		if (isnan(RotMatrix.Location.X) || isinf(RotMatrix.Location.X) || isnan(RotMatrix.Location.Y) || isinf(RotMatrix.Location.Y) ||
			isnan(RotMatrix.Location.Z) || isinf(RotMatrix.Location.Z))
		{
			return false;
		}
		ModulateNormalVal(RotMatrix);
		
		//update the borderVertex value
		BorderVect = PillarObj->GetOrigBorder();
		for (size_t i = 0; i < BorderVect.size(); i++)
		{
			kVector3D OrigBoder = BorderVect.at(i);
			kVector3D TransBord = TotalMatrix.TransformVect(OrigBoder)+ChangeVect;
			BorderVect.at(i) = TransBord;
		}
		PillarObj->SetBorder(BorderVect);
		BorderVect.clear();

		PlugObj->SetTransform(RotMatrix);
		PlugObj->MarkNeedUpdate(EChannelTransform);

		//set location and forward value
		PillarObj->SetLocation(Loc);

		return true;
	}

	return false;
}

bool PillarEditor::SetPillarSurface(ObjectID ObjID, ObjectID SectionIndex, const char *MaterialUri, int MaterialType)
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

void PillarEditor::DeletePillar(ObjectID ObjID)
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

//private function
kVector3D PillarEditor::GetVertexNormal(const kVector3D &Vert, const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2)
{
	kVector3D FirstVect, SecondVect, ThirdVect, normal;
	FirstVect = AdjVert0 - Vert;
	SecondVect = AdjVert1 - Vert;
	ThirdVect = AdjVert2 - Vert;

	kVector3D TriNorm1, TriNorm2, TriNorm3;
	TriNorm1 = FirstVect.CrossProduct(SecondVect);
	TriNorm2 = FirstVect.CrossProduct(ThirdVect);
	TriNorm3 = SecondVect.CrossProduct(ThirdVect);

	normal = (TriNorm1 + TriNorm2 + TriNorm3) / 3;
	//normal.Normalize();

	return normal;
}

void PillarEditor::SetPillarAttr(IObject* Obj, float Height, float Width, float Length, bool IsAnchor, const kPoint &Loc, const kPoint &Forward)
{
	Pillar* PluginObj = (Pillar*)Obj->GetPlugin();
	PluginObj->SetWidth(Width);
	PluginObj->SetLength(Length);
	PluginObj->SetHeight(Height);
	PluginObj->SetLocation(Loc);
	PluginObj->SetDefaultLoc(Loc);

	kVector3D size = kVector3D(Width, Length, Height);
	PluginObj->SetSize(size);

}

void PillarEditor::SetMeshData(IObject* Obj, float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward, const kPoint &WallDir)
{
	if (Obj)
	{
		float width, length;
		width = Width;
		length = Length;

		PillarMesh = Obj->GetMeshObject();
		if (PillarMesh)
		{
			PillarMeshBuilder = Obj->BeginMesh();
			SetVertexData(Loc, Forward, WallDir, Height, width, length);
			PillarMeshBuilder->End();

			Pillar* PlugPillar = (Pillar*)Obj->GetPlugin();
			PlugPillar->SetOrigBorder(BorderVect);
			BorderVect.clear();
		}
		
	}

}

void PillarEditor::SetVertexData(const kPoint &Loc, const kPoint &Forward, const kPoint &WallDir, float Height, float Width, float Length)
{
	std::string MaterialUri = "";
	int SurfaceID[12], SectIndex[12];
	for (int i = 0; i < 12; ++i)
	{
		SurfaceID[i] = PillarMeshBuilder->NewSurface(ESurfaceType::ERefMaterial, MaterialUri.c_str());
	}
	for (int i = 0; i < 12; ++i)
	{
		SectIndex[i] = PillarMeshBuilder->NewSection(SurfaceID[i]);
	}

	float dx = Width / 2;
	float dy = Length / 2;
	float SinForw = (Forward.Y / Forward.Size());
	float CosForw = (Forward.X / Forward.Size());
	float SinHoriz = (WallDir.Y / WallDir.Size());
	float CosHoriz = (WallDir.X / WallDir.Size());

	kPoint WidthVector = kPoint(dx*CosForw, dx*SinForw);
	kPoint LengthVector = kPoint(dy*CosHoriz, dy*SinHoriz);

	
	kPoint DownLeft = Loc - WidthVector - LengthVector;
	kPoint UpLeft = Loc + WidthVector - LengthVector;
	kPoint DownRight = Loc - WidthVector + LengthVector;
	kPoint UpRight = Loc + WidthVector + LengthVector;

	float ConsHeight = 280.0;
	kVector3D V0, V1, V2, V3, V4, V5, V6, V7;
	V0 = kVector3D(DownLeft.x, DownLeft.y, 0.0);
	V1 = kVector3D(DownRight.x, DownRight.y, 0.0);
	V2 = kVector3D(UpRight.x, UpRight.y, 0.0);
	V3 = kVector3D(UpLeft.x, UpLeft.y, 0.0);
	V4 = kVector3D(DownLeft.x, DownLeft.y, ConsHeight);
	V5 = kVector3D(DownRight.x, DownRight.y, ConsHeight);
	V6 = kVector3D(UpRight.x, UpRight.y, ConsHeight);
	V7 = kVector3D(UpLeft.x, UpLeft.y, ConsHeight);

	kVector3D tangent = kVector3D(0.0, 0.0, 1.0);
	kPoint lightUV = kPoint(0, 0);
	int index[24];
	kVector3D NormalVert[6];
	NormalVert[0] = kVector3D(0.0, 0.0, 0.85);
	NormalVert[1] = kVector3D(0.0, 0.0, -1.0);
	NormalVert[2] = kVector3D(-1.0, 0.0, 0.0);
	NormalVert[3] = kVector3D(1.0, 0.0, 0.0);
	NormalVert[4] = kVector3D(0.0, -1.0, 0.0);
	NormalVert[5] = kVector3D(0.0, 1.0, 0.0);
	/**bottm */
	kVector3D  AddV[6][4]{{V4,V5,V6,V7},{V0,V1,V2,V3},{V0,V1,V5,V4},{V2,V3,V7,V6},{V3,V0,V4,V7},{V1,V2,V6,V5} };
	for (int i = 0; i < 6; ++i)
	{
		std::vector<kPoint> uvArr = GetVertexUV(AddV[i][0], AddV[i][1], AddV[i][2], AddV[i][3]);
		for (int j = 0; j < 4; ++j)
		{
			index[j + i * 4] = AddVert(SectIndex[i], AddV[i][j], NormalVert[i], tangent, uvArr[j], lightUV);
		}
	}
	int indexCW[6][6] = { { 0,2,3,0,1,2},{4,6,5,4,7,6},{8,9,10,8,10,11},{12,14,15,12,13,14},{17,18,19,16,17,19},{20,21,22,20,22,23} };
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; j += 3)
			AddTri(SectIndex[i], index[indexCW[i][j]], index[indexCW[i][j + 1]], index[indexCW[i][j + 2]]);
	}

	//set border vertex vector value
	if (!BorderVect.empty())
	{
		BorderVect.clear();
	}
	BorderVect.push_back(V0);
	BorderVect.push_back(V1);
	BorderVect.push_back(V2);
	BorderVect.push_back(V3);
}

int PillarEditor::AddVert(int SectionIndex, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV)
{
	return PillarMeshBuilder->AddVert(SectionIndex, Vert, Normal, Tan, UV, LightMapUV);
}

void PillarEditor::AddTri(int MeshSection, int V0, int V1, int V2)
{
	PillarMeshBuilder->AddTri(MeshSection, V0, V1, V2);
}

bool PillarEditor::GetWallDirect(ObjectID WallId, kPoint& Start, kPoint& End)
{
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (WallId != INVALID_OBJID)
	{
		if (Suite)
		{
			IObject* WallObj = Suite->GetObject(WallId);
			if (WallObj)
			{
				IValue *WallCorner0, *WallCorner1;
				WallCorner0 = &WallObj->GetPropertyValue("P0");
				WallCorner1 = &WallObj->GetPropertyValue("P1");
				IObject* Corner0 = Suite->GetObject(WallCorner0->IntValue());
				IObject* Corner1 = Suite->GetObject(WallCorner1->IntValue());
				IValue *PIV, *PIV1;
				PIV = &Corner0->GetPropertyValue("Location");
				PIV1 = &Corner1->GetPropertyValue("Location");
				Start = PIV->Vec2Value();
				End = PIV1->Vec2Value();

				return true;
			}

		}
	}

	return false;
}

std::vector<kPoint> PillarEditor::GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3)
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

bool PillarEditor::GetPillarBorder(ObjectID ObjID, kVector3D* OutBorder)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (ObjID != INVALID_OBJID && Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		Pillar* PillarObj = (Pillar*)PlugObj->GetPlugin();
		int Len = PillarObj->GetBorder().size();
		std::vector<kVector3D>BorderArr(PillarObj->GetBorder());
		if (BorderArr.size() == 4)
		{
			for (size_t i = 0; i < BorderArr.size(); i++)
			{
				OutBorder[i] = BorderArr[i];
			}

			return true;
		}

	}

	return false;
}

bool PillarEditor::SetPillarHeightVal(ObjectID ObjID, float Height)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (ObjID != INVALID_OBJID && Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		Pillar* PillarObj = (Pillar*)PlugObj->GetPlugin();
		if (Height > 0.0)
		{
			float OrigHeight = 280.0;
			PillarObj->SetHeight(Height);
			kXform RotMatrix = PlugObj->GetTransform();
			float ZScale = Height / OrigHeight;
			kVector3D OrigScale = RotMatrix.Scale;
			OrigScale.z = ZScale;
			RotMatrix.Scale = OrigScale;
			PlugObj->SetTransform(RotMatrix);
			PlugObj->MarkNeedUpdate(EChannelTransform);
			return true;
		}

		return false;
	}
	return false;
}

void PillarEditor::SetDefUV(float wU, float lU)
{
	if (PillarMesh)
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
			bool IsGetSect = PillarMesh->GetSectionMesh(i, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices);
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

void PillarEditor::ModulateNormalVal(kXform &Transform)
{
	if (PillarMesh)
	{
		for (size_t i = 2; i < 6; i++)
		{
			float *pVertices = nullptr;
			float *pNormals = nullptr;
			float *pTangents = nullptr;
			float *pUVs = nullptr;
			float *pLightmapUVs = nullptr;
			int nVerts = 0, nIndices = 0;
			int *pIndices = nullptr;
			bool IsGetSect = PillarMesh->GetSectionMesh(i, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices);
			if (IsGetSect)
			{
				kVector3D CurScale = Transform.Scale;
				for (int i = 0; i < nVerts; i++)
				{
					/*int startInd = i * 3;
					kVector3D TempNorm = kVector3D(0.0);
					TempNorm.x = pNormals[startInd];
					TempNorm.y = pNormals[startInd + 1];
					TempNorm.z = pNormals[startInd + 2];
					kVector3D OutNorm = (Transform.GetMatrix_ZUp()).TransformNormal(TempNorm);
					OutNorm.Normalize();
					pNormals[startInd] = OutNorm.x;
					pNormals[startInd + 1] = OutNorm.y;
					pNormals[startInd + 2] = OutNorm.z;*/

					if (abs(CurScale.z - 1.0) > 0.00005)
					{
						int uvStart = i * 2;
						float origV = pUVs[uvStart + 1];
						pUVs[uvStart + 1] = (origV*CurScale.z);
					}
				}
			}
		}
	}
}