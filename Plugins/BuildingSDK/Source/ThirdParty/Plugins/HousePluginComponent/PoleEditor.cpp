#include "PoleEditor.h"
#include "PluginMain.h"
#include "Class/Pole.h"


ObjectID PoleEditor::AddPole(float WallHeight, float Width, float Length, /*bool IsWidth,*/ const kPoint &Loc, const kPoint &Forward)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		ObjectID PoleObjID = Suite->AddModel(PoleType);
		SetPoleAttribute(PoleObjID, Width, Length, WallHeight, Loc, Forward);
		return PoleObjID;
	}
	return INVALID_OBJID;
}

bool PoleEditor::UpdatePole(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject* Obj = Suite->GetObject(ObjID);
		PoleMesh=Obj->GetMeshObject(0, false);
		Pole* PlugObj = (Pole*)Obj->GetPlugin();
		kPoint OrigPosit = PlugObj->GetLocation();
		float WScale, LScale, ZScale, OrigWidth, OrigLen,OrigHeight;

		//set pole plugin angle value
		bool IsSetAng = PlugObj->GetIsAngle();
		if (!IsSetAng)
		{
			PlugObj->SetIsAngle(true);
			PlugObj->SetOrigAngle((Angle));
		}

		//judge the revert of width and length scale
		kXform RotMatrix = Obj->GetTransform();
		
		//the distance to wall is width or not 
		float OrigChangeW, OrigChangeL,OrigChangeZ;
		kVector3D DefSize = PlugObj->GetSize();
		OrigChangeW = DefSize.X;
		OrigChangeL = DefSize.Y;
		OrigChangeZ = DefSize.Z;

		if (Width > 0.0 && Length > 0.0)
		{
			OrigWidth = Width;
			OrigLen = Length;
			PlugObj->SetLength(Length);
			PlugObj->SetWidth(Width);
			float widUV, lenUV;
			widUV = Width / 100.0;
			lenUV = Length / 100.0;
			SetDefUV(widUV, lenUV);
		}
		else
		{
			OrigWidth = PlugObj->GetWidth();
			OrigLen = PlugObj->GetLength();
		}
		OrigHeight = PlugObj->GetHeight();

		WScale = OrigWidth / OrigChangeW;
		LScale = OrigLen / OrigChangeL;
		ZScale = OrigHeight / 280.0;
		
		//compute the rotation degree and radian value
		float OrigAng = PlugObj->GetOrigAngle();		
		float RotDegree = 0.0;
		RotDegree = Angle;
		float RotRadian = RotDegree * PI / 180.0;

		TMatrix43Temp<float> IdentMat = TMatrix43Temp<float>(1.0, 0.0, 0.0,
													0.0, 1.0, 0.0,
													0.0, 0.0, 1.0,
													0.0, 0.0, 0.0);
		TMatrix43Temp<float> ScaleMatrix, RotateMatrix, TempMatrix, TransMatrix, TotalMatrix;
		TotalMatrix=IdentMat;
		ScaleMatrix=IdentMat;
		RotateMatrix=IdentMat;
		TransMatrix=IdentMat;

		//compute the location of scale and changing matrix
		kPoint DefaultLoc = PlugObj->GetDefaultLoc();
		float DeltaX, DeltaY;
		DeltaX = Loc.X - OrigPosit.X;
		DeltaY = Loc.Y - OrigPosit.Y;


		ScaleMatrix.SetScale(WScale, LScale, ZScale);
		RotateMatrix.SetRotationZ(RotRadian);
		TransMatrix.SetOrigin(DeltaX, DeltaY, 0);
		TotalMatrix = ScaleMatrix * RotateMatrix * TransMatrix;

		kVector3D DeltaPos = TotalMatrix.TransformVect(kVector3D(DefaultLoc, 0.0));
		kVector3D ChangeVect = kVector3D(Loc, 0.0) - DeltaPos;

		kVector3D TotalRotVal = TotalMatrix.GetRotationXYZ();
		RotMatrix.Scale = TotalMatrix.GetScale();
		RotMatrix.Rotation = kRotation(TotalRotVal.Y, RotDegree, TotalRotVal.X);
		RotMatrix.Location = ChangeVect+TotalMatrix.GetOrigin();
		PlugObj->SetLocation(Loc);
		
		//the disable value
		if (isnan(RotMatrix.Location.X) || isinf(RotMatrix.Location.X) || isnan(RotMatrix.Location.Y) || isinf(RotMatrix.Location.Y) ||
			isnan(RotMatrix.Location.Z) || isinf(RotMatrix.Location.Z))
		{
			return false;
		}

		ModulateNormalVal(RotMatrix);
		
		Obj->SetTransform(RotMatrix);
		Obj->MarkNeedUpdate(EChannelTransform);

		BorderVect = PlugObj->GetOrigBorder();
		for (size_t i = 0; i < BorderVect.size(); i++)
		{
			kVector3D OrigBoder = BorderVect.at(i);
			kVector3D TransBord = TotalMatrix.TransformVect(OrigBoder)+ChangeVect;
			BorderVect.at(i) = TransBord;
		}
		PlugObj->SetBorder(BorderVect);
		BorderVect.clear();

		return true;
	}

	return false;
}

void PoleEditor::DeletePole(ObjectID ObjID)
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

bool PoleEditor::UpdatePole(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		bool UpdateBool = false;

		IObject* PluginObj = Suite->GetObject(ObjID);
		Pole* PolePlug = (Pole*)PluginObj->GetPlugin();

		float BaseYawVal = 0.0;
		bool IsSetAng = PolePlug->GetIsAngle();
		if (!IsSetAng)
		{
			PolePlug->SetIsAngle(true);
			PolePlug->SetOrigAngle(RotAngle);
		}
		BaseYawVal = RotAngle;

		//set pole plugin current rotation angle
		kXform RotMatrix = PluginObj->GetTransform();
		kPoint OrigLoc = PolePlug->GetLocation();
		kPoint DefLoc = PolePlug->GetDefaultLoc();
			
		kPoint ChangeLoc = Loc;
		kVector3D RotScale = RotMatrix.GetMatrix().GetScale();

		//calculate the component transform value
		TMatrix43Temp<float> ScaleMatrix, RotateMatrix, TransMatrix, TotalMatrix, SelfMatrix;
		TotalMatrix.MakeIdentity();
		ScaleMatrix.MakeIdentity();
		RotateMatrix.MakeIdentity();
		TransMatrix.MakeIdentity();
				
		float DeltaX, DeltaY;
		DeltaX = ChangeLoc.X - OrigLoc.X;
		DeltaY = ChangeLoc.Y - OrigLoc.Y;
				
		float DForwAng = BaseYawVal * 180.0 / PI ;
		float RadForwAng = BaseYawVal * PI / 180.0;

		ScaleMatrix.SetScale(RotScale.X, RotScale.Y, RotScale.Z);
		RotateMatrix.SetRotationZ(RadForwAng);
		TransMatrix.SetOrigin(DeltaX, DeltaY, 0);
		TotalMatrix = ScaleMatrix*RotateMatrix*TransMatrix;

		kVector3D DeltaPos = TotalMatrix.TransformVect(kVector3D(DefLoc, 0.0));
		kVector3D ChangeVect = kVector3D(ChangeLoc, 0.0) - DeltaPos;
				
		RotMatrix.Scale = TotalMatrix.GetScale();
		kVector3D RotVect = TotalMatrix.GetRotationXYZ();
		RotMatrix.Rotation = kRotation(RotVect.Y, BaseYawVal, RotVect.X);
		RotMatrix.Location = ChangeVect+TotalMatrix.GetOrigin();
		PolePlug->SetLocation(ChangeLoc);

		//the illegal value
		if (IsNaN(RotMatrix.Location.X)  || IsNaN(RotMatrix.Location.Y) || IsNaN(RotMatrix.Location.Z))
		{
			return false;
		}

		ModulateNormalVal(RotMatrix);
		
		//set object update transform
		PluginObj->SetTransform(RotMatrix);
		PluginObj->MarkNeedUpdate(EChannelTransform);
		UpdateBool = true;
		
		//modulate the model border vertex value
		BorderVect = PolePlug->GetOrigBorder();
		for (size_t i = 0; i < BorderVect.size(); i++)
		{
			kVector3D OrigBoder = BorderVect.at(i);
			kVector3D TransBord = (RotMatrix.GetMatrix()).TransformVect(OrigBoder);
			BorderVect.at(i) = TransBord;
		}
		PolePlug->SetBorder(BorderVect);
		BorderVect.clear();

		return UpdateBool;
	}

	return false;
}

bool PoleEditor::SetPoleSurface(ObjectID ObjID, ObjectID SectionIndex, const char *MaterialUri, int MaterialType)
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
			if(SectionIndex < SectNum)
			{
				Obj->SetSurfaceUri(MeshObj, MaterialType, MaterialUri, SectionIndex);
				Obj->MarkNeedUpdate(EChannelSurface);	
				IsSet = true;
			}
		}
	}
	return IsSet;
}

//protected function

void PoleEditor::SetPoleAttribute(ObjectID ObjID, float width, float length, float wallHeight, const kPoint &Loc, const kPoint &Forward)
{
	IBuildingResourceMgr *ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		Pole* _PoleObj = (Pole*)PlugObj->GetPlugin();
		_PoleObj->SetWidth(width);
		_PoleObj->SetLength(length);
		_PoleObj->SetHeight(wallHeight);
		_PoleObj->SetLocation(Loc);
		_PoleObj->SetDefaultLoc(Loc);
		_PoleObj->SetIsAngle(false);

		//get wall direction
		kPoint WallDirVect = kPoint(-Forward.Y, Forward.X);

		kVector3D size = kVector3D(width, length, wallHeight);
		_PoleObj->SetSize(size);

		PoleMesh = PlugObj->GetMeshObject();
		if (PoleMesh)
		{
			PoleMeshBuilder = PlugObj->BeginMesh();
			SetFlueVertex(Loc, Forward, WallDirVect, width, length, wallHeight);
			PoleMeshBuilder->End();

			_PoleObj->SetOrigBorder(BorderVect);
			BorderVect.clear();

			/*const char* SurfUri = WallSurfObj->GetUri();
			int SurfType = WallSurfObj->GetSurfaceType();
			if (WallSurfObj)
			{
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 0);
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 1);
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 2);
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 3);
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 4);
				PlugObj->SetSurfaceUri(PoleMesh, SurfType, SurfUri, 5);
			}*/
		}
	}
}

void PoleEditor::GetNearestWall(const FluePrimitve &FluePrim, const kPoint &SceneLoc, ObjectID &WallId, kPoint &NearLoc)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	NearLoc = SceneLoc;
	//ObjectID WallIDs[2] = { -1,-1 };
	WallId = -1;
	

	if (Suite)
	{
		kPoint UpL, DownL, DownR, UpR;
		UpL = FluePrim.UpLeft;
		DownL = FluePrim.DownLeft;
		DownR = FluePrim.DownRight;
		UpR = FluePrim.UpRight;
		std::vector<int> HitWallIds;
		kPoint DownUpRDir = UpR - DownR;
		kPoint DownLRDir = DownL - DownR;

		int HitId = Suite->HitTest(UpL);
		if (HitId != -1)
		{
			IObject* obj = Suite->GetObject(HitId);
			int ObjType = obj->GetType();
			if (ObjType == (EObjectType::EWall))
			{
				if (HitWallIds.size() > 0)
				{
					std::vector<int>::iterator FindIte;
					//FindIte = std::find(HitWallIds.begin(), HitWallIds.end(), HitWallIds);
					/*if (FindIte == HitWallIds.end())
					{
						HitWallIds.push_back(HitId);
					}*/
				}
				
				//Todo 计算变换矩阵，计算自身旋转角度

			}
		}
		if (HitWallIds.size()>0)
		{
			int WallNum = HitWallIds.size();
			if ((WallNum == 1))
			{
				WallId = HitWallIds.at(0);
				kPoint DownLRVect, RightDUVect;
				DownLRVect = FluePrim.DownLeft - FluePrim.DownRight;
				RightDUVect = FluePrim.UpRight - FluePrim.DownRight;
				
				kPoint StartP, EndP, DirVect;
				GetWallDirect(WallId, StartP, EndP);
				DirVect = EndP - StartP;
				float DownAngVal, RightAngVal, DAng, RAng;
				DownAngVal = DirVect.Dot(DownLRVect) / (DirVect.Size()*DownLRVect.Size());
				RightAngVal = DirVect.Dot(RightDUVect) / (DirVect.Size()*RightDUVect.Size());
				
				if (DownAngVal <0.0)
				{
					DAng = 180.0 - acos(DownAngVal) / PI * 180.0;
				}
				else
				{
					DAng = -acos(DownAngVal) / PI * 180.0;
				}

				if (RightAngVal < 0.0)
				{
					RAng = 180.0 - acos(RightAngVal) / PI * 180.0;
				}
				else
				{
					DAng = -acos(RightAngVal) / PI * 180.0;
				}

				kPoint DRP0, DLP0, URP0, DRMapPt, TransVect;
				float DwRDist, DwLDist, UpRDist,DwRCos, DwLCos, UpRCos, LenRate, RotateAng;
				DRP0 = FluePrim.DownRight - StartP;
				DLP0 = FluePrim.DownLeft - StartP;
				URP0 = FluePrim.UpRight - StartP;

				DwRCos = DRP0.Dot(DirVect)/(DRP0.Size()*DirVect.Size());
				DwLCos = DLP0.Dot(DirVect)/(DLP0.Size()*DirVect.Size());
				UpRCos = URP0.Dot(DirVect)/(URP0.Size()*DirVect.Size());

				DwRDist = DRP0.Size()*Sqrt(1.0 - Square(DwRCos));
				DwLDist = DLP0.Size()*Sqrt(1.0 - Square(DwLCos));
				UpRDist = URP0.Size()*Sqrt(1.0 - Square(UpRCos));

				//calculate move vector
				LenRate = (DwRCos *DRP0.Size()) / DirVect.Size();
				DRMapPt = DirVect * kPoint(LenRate, LenRate) + StartP;
				
				if (abs(DAng) > abs(RAng))
				{
					if (RAng>0.0)
					{
						if (DwRDist > UpRDist)
						{
							RAng += 90.0;
						}
					}
					else
					{
						if (DwRDist > UpRDist)
						{
							RAng -= 90.0;
						}
					}
					/*float MoveRate = RightDUVect.Size() / DwRDist;
					kPoint MoveDirect = FluePrim.DownRight - DRMapPt;
					TransVect = DRMapPt + MoveDirect * kPoint(MoveRate, MoveRate);*/
					RotateAng = RAng;
				}
				else
				{
					if (DAng > 0.0)
					{
						if (DwRDist > DwRDist)
						{
							DAng += 90.0;
						}
					}
					else
					{
						if (DwRDist > DwRDist)
						{
							DAng -= 90.0;
						}
					}

					RotateAng = DAng;
				}
				TransVect = FluePrim.DownRight - DRMapPt;
				NearLoc = SceneLoc + TransVect;
			}
			else if ((WallNum == 2))
			{
				//TODO:calculate the downright
				int FirWallId, SecWallId;
				FirWallId = HitWallIds[0];
				SecWallId = HitWallIds[1];
				IObject *WallObj, *WallObjS;
				IValue *Corner0, *Corner1;
				int FirP0, FirP1, SecP0, SecP1;
				WallObj = Suite->GetObject(FirWallId);
				WallObjS = Suite->GetObject(SecWallId);
				Corner0 = &WallObj->GetPropertyValue("P0");
				Corner1 = &WallObj->GetPropertyValue("P1");
				FirP0 = Corner0->IntValue();
				FirP1 = Corner1->IntValue();

				Corner0 = &WallObjS->GetPropertyValue("P0");
				Corner1 = &WallObjS->GetPropertyValue("P1");
				SecP0 = Corner0->IntValue();
				SecP1 = Corner1->IntValue();

			}
		}
		
	}
}

//private function

kVector3D PoleEditor::GetVertexNormal(const kVector3D &Vert, const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2)
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
	normal.Normalize();

	return normal;
}

void PoleEditor::SetFlueVertex(const kPoint &Location, const kPoint &Forward, const kPoint &WallDir, float Width, float Length, float Height)
{
	std::string MaterialUri = "";
	int SurfaceID[6], SectIndex[6];
	for (int i=0;i< 6;++i)
	{
		SurfaceID[i] = PoleMeshBuilder->NewSurface(ESurfaceType::ERefMaterial, MaterialUri.c_str());
		ISurfaceObject* TestSurfObj;
		TestSurfObj = PoleMeshBuilder->GetSurface(SurfaceID[i]);
	}
	for (int i = 0; i < 6; ++i)
	{
		SectIndex[i] = PoleMeshBuilder->NewSection(SurfaceID[i]);
	}	

	float width, length;
	width = Width;
	length = Length;
	
	double dx = width/2;
	double dy = length/2;
	float SinForw, CosForw, SinHoriz, CosHoriz;
	SinForw = (Forward.Y / Forward.Size());
	CosForw = (Forward.X / Forward.Size());
	SinHoriz = (WallDir.Y/WallDir.Size());
	CosHoriz = (WallDir.X/WallDir.Size());

	kPoint WidthVector = kPoint(dx*CosForw, dx*SinForw);
	kPoint LengthVector = kPoint(dy*CosHoriz, dy*SinHoriz);


	kPoint DownLeft = Location - WidthVector - LengthVector;
	kPoint UpLeft = Location + WidthVector - LengthVector;
	kPoint DownRight = Location - WidthVector + LengthVector;
	kPoint UpRight = Location + WidthVector + LengthVector;

	float ConsHeight = 280.0;
	kVector3D V0, V1, V2, V3, V4, V5, V6, V7;
	V0 = kVector3D(DownLeft.x, DownLeft.y, 0.0);
	V1 = kVector3D(DownRight.x, DownRight.y, 0.0);
	V2 = kVector3D(UpRight.x, UpRight.y, 0.0);
	V3 = kVector3D(UpLeft.x, UpLeft.y, 0.0);
	V4 = kVector3D(DownLeft.x, DownLeft.y, ConsHeight);
	V5 = kVector3D(DownRight.x, DownRight.y, ConsHeight);
	V6 = kVector3D(UpRight.x,UpRight.y, ConsHeight);
	V7 = kVector3D(UpLeft.x,UpLeft.y, ConsHeight);

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
	for (int i=0; i < 6; ++i)
	{
		std::vector<kPoint> uvArr = GetVertexUV(AddV[i][0], AddV[i][1], AddV[i][2], AddV[i][3]);
		for (int j = 0; j < 4; ++j)
		{			
			index[j+i*4]= AddVert(SectIndex[i], AddV[i][j], NormalVert[i], tangent, uvArr[j], lightUV);
		}
	}
	int indexCW[6][6] = { { 0,2,3,0,1,2},{4,6,5,4,7,6},{8,9,10,8,10,11},{12,14,15,12,13,14},{17,18,19,16,17,19},{20,21,22,20,22,23} };
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; j += 3)
			AddTri(SectIndex[i], index[indexCW[i][j]], index[indexCW[i][j + 1]], index[indexCW[i][j + 2]]);
	}

	//set border vertex value vector
	if (!BorderVect.empty())
	{
		BorderVect.clear();
	}
	BorderVect.push_back(V0);
	BorderVect.push_back(V1);
	BorderVect.push_back(V2);
	BorderVect.push_back(V3);
}



int PoleEditor::CreateMeshSection()
{
	int sectNum = PoleMeshBuilder->GetSectionCount();
	int sectIndex = PoleMeshBuilder->NewSection(sectNum);
	return sectIndex;
}

int PoleEditor::AddVert(int MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV)
{
	int sectionIndex = PoleMeshBuilder->AddVert(MeshSection, Vert, Normal, Tan, UV, LightMapUV);
	return sectionIndex;
}

void PoleEditor::AddTri(int MeshSection, int V0, int V1, int V2)
{
	PoleMeshBuilder->AddTri(MeshSection, V0, V1, V2);
}

bool PoleEditor::GetWallDirect(ObjectID WallId, kPoint& Start, kPoint& End)
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

std::vector<kPoint> PoleEditor::GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3)
{
	std::vector<kPoint> UVArr;
	UVArr.clear();
	kVector3D tempVec = AdjVert2 - AdjVert0;
	/*float L = tempVec.z / (abs(tempVec.x) + abs(tempVec.y));*/
	float W = 0.0;
	if (abs(tempVec.x)<0.005)
	{
		W = abs(tempVec.y) / 100.0;
	}
	else if (abs(tempVec.y)<0.005)
	{
		W = abs(tempVec.x) / 100.0;
	}
	
	float L = tempVec.z / 100.0f;
	UVArr.push_back(kPoint(0, L));
	UVArr.push_back(kPoint(W, L));
	UVArr.push_back(kPoint(W, 0));
	UVArr.push_back(kPoint(0, 0));
	return UVArr;
}

//int PoleEditor::GetBorderInfo(IObject* Obj, kArray<kVector3D> BorderPoints)
//{
//	if (Obj)
//	{
//		IPluginObject* FlueObj = Obj->GetPlugin();
//		IValue* BorderIV = FlueObj->GetFunctionProperty("BorderVertex");
//		kArray<kVector3D> BorderVal = BorderIV->Vec3ArrayValue();
//		if (BorderPoints.empty())
//		{
//			return BorderVal.size();
//		}
//		else
//		{
//			int Num = BorderVal.size();
//			for (int i = 0; i < Num; i++)
//			{
//				kVector3D RetVal = BorderVal[i];
//				BorderPoints[i] = RetVal;
//			}
//			return Num;
//		}
//	}
//	
//	return -1;
//}
//
//float PoleEditor::GetFlueArea(IObject* Obj)
//{
//	if (Obj)
//	{
//		IPluginObject* PlugObj = Obj->GetPlugin();
//		IValue* AreaIV = PlugObj->GetFunctionProperty("FlueSurfArea");
//		return AreaIV->FloatValue();
//	}
//	return -1.0;
//}
//
//float PoleEditor::GetWallArea(IObject* Obj)
//{
//	if (Obj)
//	{
//		IPluginObject* PlugObj = Obj->GetPlugin();
//		IValue* WallIV = PlugObj->GetFunctionProperty("WallArea");
//		kPoint WallArea = WallIV->Vec2Value();
//		float TotalWallArea = WallArea.X + WallArea.Y;
//		return TotalWallArea;
//	}
//
//	return -1.0;
//}

bool PoleEditor::GetPoleBorder(ObjectID ObjID, kVector3D* OutBorder)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
/*	std::vector<kVector3D> BorderArr;*/
	if (ObjID!= INVALID_OBJID && Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		Pole* PoleObj = (Pole*)PlugObj->GetPlugin();
		//int Len = PoleObj->GetBorder().size();
		
		std::vector<kVector3D>BorderArr(PoleObj->GetBorder());
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

bool PoleEditor::SetPoleHeightVal(ObjectID ObjID, float Height)
{
	IBuildingResourceMgr * ResMgr = GSDK->GetResourceMgr();
	ISuite *Suite = GSDK ? GSDK->GetSuite("") : nullptr;
	if (ObjID != INVALID_OBJID && Suite)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		Pole* PoleObj = (Pole*)PlugObj->GetPlugin();
		if (Height>0.0)
		{
			float OrigHeight = 280.0;
			PoleObj->SetHeight(Height);
			kXform RotMatrix = PlugObj->GetTransform();
			float ZScale = Height/OrigHeight;
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

void PoleEditor::SetDefUV(float wU, float lU)
{
	if (PoleMesh)
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
			bool IsGetSect = PoleMesh->GetSectionMesh(i, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices);
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

void PoleEditor::ModulateNormalVal(kXform &Transform)
{
	if (PoleMesh)
	{
		kVector3D CurScale = Transform.Scale;
		for (size_t i = 2; i < 6; i++)
		{
			float *pVertices = nullptr;
			float *pNormals = nullptr;
			float *pTangents = nullptr;
			float *pUVs = nullptr;
			float *pLightmapUVs = nullptr;
			int nVerts = 0, nIndices = 0;
			int *pIndices = nullptr;
			bool IsGetSect = PoleMesh->GetSectionMesh(i, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices);
			if (IsGetSect)
			{
				for (int i = 0; i < nVerts; i++)
				{
					/*int startInd = i * 3;
					kVector3D TempNorm = kVector3D(0.0);
					TempNorm.x = pNormals[startInd];
					TempNorm.y = pNormals[startInd + 1];
					TempNorm.z = pNormals[startInd + 2];
					kVector3D OutNorm = (Transform.GetMatrix_ZUp()).TransformNormal(TempNorm);
					if ((OutNorm.x)*(OutNorm.y)>0.00001)
					{
						(OutNorm.x) *= -1.0;
					}
					OutNorm.Normalize();
					pNormals[startInd] = OutNorm.x;
					pNormals[startInd + 1] = OutNorm.y;
					pNormals[startInd + 2] = OutNorm.z;*/
					if (abs(CurScale.z-1.0)>0.00005)
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