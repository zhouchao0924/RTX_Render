

#include "Wall.h"
#include "kBox.h"
#include "Corner.h"
#include "Window.h"
#include "WallHole.h"
#include "DoorHole.h"
#include "ISuite.h"
#include "Room.h"
#include "Class/Property.h"

void FWallHoleInfo::Serialize(ISerialize &Ar)
{
	Ar << HoleID;
	Ar << MinX;
	Ar << MaxX;
	KSERIALIZE_ENUM(EObjectType, ObjType);
}

Wall::Wall()
	: RoomLeft(INVALID_OBJID)
	, RoomRight(INVALID_OBJID)
	, bCapsBuilded(false)
	, Baton(nullptr)
{
	P[0] = INVALID_OBJID;
	P[1] = INVALID_OBJID;
}

BEGIN_DERIVED_CLASS(Wall, Primitive)
	ADD_PROP(RoomLeft, IntProperty)
	ADD_PROP(RoomRight, IntProperty)
END_CLASS()

void Wall::Serialize(ISerialize &Ar)
{
	Primitive::Serialize(Ar);
	Ar << P[0];
	Ar << P[1];	
}

void Wall::OnCreate()
{
	Primitive::OnCreate();
	assert(P[0] != P[1]);
}

IValue *Wall::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = nullptr;
	if (name == "Forward")
	{
		pValue = &GValueFactory->Create(&GetForward(), false);
	}
	else if (name == "Right")
	{
		pValue = &GValueFactory->Create(&GetRight(), false);
	}
	else if (name == "P0")
	{
		pValue = &GValueFactory->Create(P[0]);
	}
	else if (name == "P1")
	{
		pValue = &GValueFactory->Create(P[1]);
	}
	return pValue;
}

void Wall::OnDestroy()
{
	if (RoomLeft != INVALID_OBJID)
	{
		_Suite->DeleteObject(RoomLeft, true);
	}

	if (RoomRight != INVALID_OBJID)
	{
		_Suite->DeleteObject(RoomRight, true);
	}

	Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(P[0], Corner);
	if (pCorner0)
	{
		pCorner0->RemoveWall(_ID);
	}

	Corner *pCorner1 = SUITE_GET_BUILDING_OBJ(P[1], Corner);
	if (pCorner1)
	{
		pCorner1->RemoveWall(_ID);
	}

	Primitive::OnDestroy();
}


void Wall::GetLocations(kPoint &P0, kPoint &P1)
{
	Corner *s = SUITE_GET_BUILDING_OBJ(P[0], Corner);
	if (s)
	{
		P0 = s->GetLocation();
	}

	Corner *e = SUITE_GET_BUILDING_OBJ(P[1], Corner);
	if (e)
	{
		P1 = e->GetLocation();
	}
}

kPoint Wall::GetForward()
{
	kPoint P0, P1;
	GetLocations(P0, P1);
	kPoint Forward = P1 - P0;
	return Forward.Normalize();
}

kPoint Wall::GetRight()
{
	kPoint P0, P1;
	GetLocations(P0, P1);

	kVector3D Forward(P1.X - P0.X, P1.Y - P0.Y, 0);
	Forward.Normalize();

	kVector3D Right = kVector3D(0, 0, 1.0f).CrossProduct(Forward);

	return kPoint(Right.x, Right.y);
}

void Wall::GetBorderLines(kLine &CenterLine, kLine &LeftLine, kLine &RightLine)
{
	if (!bCapsBuilded || bNeedUpdate)
	{
		BuildCaps();
		bCapsBuilded = true;
	}

	kPoint P0, P1;
	GetLocations(P0, P1);

	CenterLine.Set(kVector3D(P0), kVector3D(P1));
	LeftLine.Set(kVector3D(Points[ELeft0]), kVector3D(Points[ELeft1]));
	RightLine.Set(kVector3D(Points[ERight0]), kVector3D(Points[ERight1]));
}

void Wall::GetOriginalBorderLines(kLine &CenterLine, kLine &LeftLine, kLine &RightLine)
{
	kPoint P0, P1;
	GetLocations(P0, P1);

	kVector3D Forward(P1.X - P0.X, P1.Y - P0.Y, 0);
	Forward.Normalize();

	kVector3D Right = kVector3D(0, 0, 1.0f).CrossProduct(Forward);

	CenterLine.Set(kVector3D(P0), kVector3D(P1));
	LeftLine.Set(kVector3D(P0) - Right*GetThickLeft(), kVector3D(P1) - Right*GetThickLeft());
	RightLine.Set(kVector3D(P0) + Right*GetThickRight(), kVector3D(P1) + Right*GetThickRight());
}

kPoint Wall::GetDirection(ObjectID From)
{
	kPoint P0, P1, Dir;
	GetLocations(P0, P1);

	if (P[0] == From)
	{
		Dir = P1 - P0;
	}
	else if (P[1] == From)
	{
		Dir = P0 - P1;
	}

	Dir.Normalize();

	return Dir;
}

ObjectID Wall::GetOtherCorner(ObjectID CornerID)
{
	return P[0] == CornerID ? P[1] : P[0];
}

struct SegmentDistToSegment_Solver
{
	SegmentDistToSegment_Solver(const kVector3D& InA1, const kVector3D& InB1, const kVector3D& InA2, const kVector3D& InB2) :
		bLinesAreNearlyParallel(false),
		A1(InA1),
		A2(InA2),
		S1(InB1 - InA1),
		S2(InB2 - InA2),
		S3(InA1 - InA2)
	{
	}

	bool bLinesAreNearlyParallel;

	const kVector3D& A1;
	const kVector3D& A2;

	const kVector3D S1;
	const kVector3D S2;
	const kVector3D S3;

	void Solve(kVector3D& OutP1, kVector3D& OutP2)
	{
		const float Dot11 = S1.Dot(S1);
		const float Dot12 = S1.Dot(S2);
		const float Dot13 = S1.Dot(S3);
		const float Dot22 = S2.Dot(S2);
		const float Dot23 = S2.Dot(S3);

		const float D = Dot11 * Dot22 - Dot12 * Dot12;

		float D1 = D;
		float D2 = D;

		float N1;
		float N2;

		if (bLinesAreNearlyParallel || D < 0.00001f)
		{
			// the lines are almost parallel
			N1 = 0.f;	// force using point A on segment S1
			D1 = 1.f;	// to prevent possible division by 0 later
			N2 = Dot23;
			D2 = Dot22;
		}
		else
		{
			// get the closest points on the infinite lines
			N1 = (Dot12*Dot23 - Dot22 * Dot13);
			N2 = (Dot11*Dot23 - Dot12 * Dot13);

			if (N1 < 0.f)
			{
				// t1 < 0.f => the s==0 edge is visible
				N1 = 0.f;
				N2 = Dot23;
				D2 = Dot22;
			}
			else if (N1 > D1)
			{
				// t1 > 1 => the t1==1 edge is visible
				N1 = D1;
				N2 = Dot23 + Dot12;
				D2 = Dot22;
			}
		}

		if (N2 < 0.f)
		{
			// t2 < 0 => the t2==0 edge is visible
			N2 = 0.f;

			// recompute t1 for this edge
			if (-Dot13 < 0.f)
			{
				N1 = 0.f;
			}
			else if (-Dot13 > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = -Dot13;
				D1 = Dot11;
			}
		}
		else if (N2 > D2)
		{
			// t2 > 1 => the t2=1 edge is visible
			N2 = D2;

			// recompute t1 for this edge
			if ((-Dot13 + Dot12) < 0.f)
			{
				N1 = 0.f;
			}
			else if ((-Dot13 + Dot12) > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = (-Dot13 + Dot12);
				D1 = Dot11;
			}
		}

		// finally do the division to get the points' location
		const float T1 = (std::abs(N1) < 0.00001f ? 0.f : N1 / D1);
		const float T2 = (std::abs(N2) < 0.00001f ? 0.f : N2 / D2);

		// return the closest points
		OutP1 = A1 + T1 * S1;
		OutP2 = A2 + T2 * S2;
	}
};

void ClipBorderLine(kVector3D &P0, kVector3D &DL0, kVector3D &P1, kVector3D &DL1, bool &bClip, kPoint &PtClip, kPoint &End)
{
	const float TestLen = 10000.0f;

	if (std::abs(DL0.Dot(DL1))<0.98f)
	{
		kLine L0(P0 - DL0 * TestLen, P0 + DL0 * TestLen);
		kLine L1(P1 - DL1 * TestLen, P1 + DL1 * TestLen);

		kVector3D OutP1, OutP2;
		SegmentDistToSegment_Solver(L0.start, L0.end, L1.start, L1.end).Solve(OutP1, OutP2);

		kPoint pt = kPoint(OutP2.x, OutP2.y);
		if (!bClip)
		{
			bClip = true;
			PtClip = pt;
		}
		else
		{
			if ((pt - End).Size() < (PtClip - End).Size())
			{
				PtClip = pt;
			}
		}
	}
}

void Wall::BuildCaps()
{
	Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(P[0], Corner);
	Corner *pCorner1 = SUITE_GET_BUILDING_OBJ(P[1], Corner); 
		
	if (pCorner0 && pCorner1)
	{
		Wall *pWall = NULL;

		kPoint Dir;
		kLine  l_left, l_center, l_right;
		GetOriginalBorderLines(l_center, l_left, l_right);

		Points[ELeft0] = kPoint(l_left.start.x, l_left.start.y);
		Points[ELeft1] = kPoint(l_left.end.x, l_left.end.y);

		Points[ERight0] = kPoint(l_right.start.x, l_right.start.y);
		Points[ERight1] = kPoint(l_right.end.x, l_right.end.y);

		Dir = kPoint(l_center.end.x - l_center.start.x, l_center.end.y - l_center.start.y) ;
		Dir.Normalize();

		ObjectID connect_wall_0[2];
		bool leftClip0 = false;
		bool rightClip0 = false;
		pCorner0->GetNearByWall(_ID, connect_wall_0[0], connect_wall_0[1]);

		kVector3D UpDir(0, 0, 1.0f);

		for (int i = 0; i < 2; ++i)
		{
			ObjectID wall_id = connect_wall_0[i];
			if (wall_id != INVALID_OBJID)
			{
				pWall = SUITE_GET_BUILDING_OBJ(wall_id, Wall);
				if (pWall)
				{
					kVector3D Oth_D = kVector3D(pWall->GetDirection(P[0]));
					kVector3D Oth_Basis = kVector3D(0, 0, 1.0f).CrossProduct(Oth_D);

					float OthLeft, OthRight;
					if (pWall->P[0] == P[0])
					{
						OthLeft = pWall->GetThickLeft();
						OthRight = pWall->GetThickRight();
					}
					else
					{
						OthLeft = pWall->GetThickRight();
						OthRight = pWall->GetThickLeft();
					}

					kVector3D Oth_Cap_r = kVector3D(pCorner0->Location) + Oth_Basis * OthRight;
					ClipBorderLine(Oth_Cap_r, -Oth_D, l_left.start, -kVector3D(Dir), leftClip0, Points[ELeft0], kPoint(l_left.end.x, l_left.end.y));

					kVector3D Oth_Cap_l = kVector3D(pCorner0->Location) - Oth_Basis * OthLeft;
					ClipBorderLine(Oth_Cap_l, -Oth_D, l_right.start, -kVector3D(Dir), rightClip0, Points[ERight0], kPoint(l_right.end.x, l_right.end.y));
				}
			}
		}

		//-----------------------------------------------------------------------------
		ObjectID connect_wall_1[2];
		bool leftClip1 = false;
		bool rightClip1 = false;
		pCorner1->GetNearByWall(_ID, connect_wall_1[0], connect_wall_1[1]);
		for (int i = 0; i < 2; ++i)
		{
			ObjectID wall_id = connect_wall_1[i];

			if (wall_id != INVALID_OBJID)
			{
				pWall = SUITE_GET_BUILDING_OBJ(wall_id, Wall);
				if (pWall)
				{
					kVector3D Oth_D = kVector3D(pWall->GetDirection(P[1]));
					kVector3D Oth_Basis = kVector3D(0, 0, 1.0f).CrossProduct(Oth_D);

					kLine inv_l_left(l_left.end, l_left.start);
					kLine inv_l_right(l_right.end, l_right.start);

					float OthLeft, OthRight;
					if (pWall->P[0] == P[1])
					{
						OthLeft = pWall->GetThickLeft();
						OthRight = pWall->GetThickRight();
					}
					else
					{
						OthLeft = pWall->GetThickRight();
						OthRight = pWall->GetThickLeft();
					}

					kVector3D Oth_Cap_r = kVector3D(pCorner1->Location) + Oth_Basis * OthRight;
					ClipBorderLine(Oth_Cap_r, -Oth_D, l_right.end, kVector3D(Dir), rightClip1, Points[ERight1], kPoint(l_right.start.x, l_right.start.y));

					kVector3D Oth_Cap_l = kVector3D(pCorner1->Location) - Oth_Basis * OthLeft;
					kPlane3D clip_plane_l(Oth_Cap_l, -Dir);
					ClipBorderLine(Oth_Cap_l, -Oth_D, l_left.end, kVector3D(Dir), leftClip1, Points[ELeft1], kPoint(l_left.start.x, l_left.start.y));
				}
			}
		}
	}
}

void Wall::SetCorner(int CornerIndex, ObjectID NewCorner, std::vector<FWallHoleInfo> &OtherHoles)
{
	OtherHoles.clear();

	if ((CornerIndex >= 0  && CornerIndex < 2) && P[CornerIndex]!=NewCorner)
	{
		ObjectID OtherCornerID = P[(CornerIndex + 1) % 2];
		assert(OtherCornerID != NewCorner);

		Corner *pCorner = SUITE_GET_BUILDING_OBJ(P[CornerIndex], Corner);
		if (pCorner)
		{
			ObjectID WallID = GetID();
			pCorner->RemoveWall(WallID);
		}

		P[CornerIndex] = INVALID_OBJID;

		pCorner = SUITE_GET_BUILDING_OBJ(NewCorner, Corner); 
		Corner *pOtherCorner = SUITE_GET_BUILDING_OBJ(OtherCornerID, Corner);
		assert(pOtherCorner);

		if (pCorner)
		{
			P[CornerIndex] = NewCorner;
			pCorner->AddWall(this);			

			float Dist = (pCorner->Location - pOtherCorner->Location).Size();
			
			size_t i = 0;
			std::vector<FWallHoleInfo> *Holes = GetHoles();
			if (Holes)
			{
				for (; i < Holes->size(); ++i)
				{
					FWallHoleInfo &Info = (*Holes)[i];
					if (Info.MinX > Dist)
					{
						break;
					}
				}

				if (CornerIndex == 1)
				{
					size_t nRemove = Holes->size() - i;
					OtherHoles.resize(nRemove);

					for (; i < Holes->size(); ++i)
					{
						OtherHoles[i] = (*Holes)[i];
						OtherHoles[i].MinX -= Dist;
						OtherHoles[i].MaxX -= Dist;
					}

					if (nRemove > 0)
					{
						Holes->erase(Holes->begin() + i, Holes->begin() + nRemove);
					}
				}
				else if (CornerIndex == 0)
				{
					size_t nRemove = i;
					OtherHoles.resize(nRemove);

					for (size_t k = 0; k < nRemove; ++i)
					{
						OtherHoles[k] = (*Holes)[k];
					}

					if (nRemove > 0)
					{
						Holes->erase(Holes->begin(), Holes->begin() + nRemove);
					}

					for (size_t k = nRemove; k < Holes->size(); ++k)
					{
						FWallHoleInfo &Info = (*Holes)[k];
						Info.MinX = Info.MinX - Dist;
						Info.MaxX = Info.MaxX - Dist;
					}
				}
			}
		}

		MarkNeedUpdate();
	}
}

void Wall::MarkNeedUpdate()
{
	if (RoomLeft!=INVALID_OBJID)
	{
		Room *pRoom = SUITE_GET_BUILDING_OBJ(RoomLeft, Room);
		if (pRoom)
		{
			pRoom->MarkNeedUpdate();
		}
	}

	if (RoomRight != INVALID_OBJID)
	{
		Room *pRoom = SUITE_GET_BUILDING_OBJ(RoomRight, Room);
		if (pRoom)
		{
			pRoom->MarkNeedUpdate();
		}
	}

	bCapsBuilded = false;
	Primitive::MarkNeedUpdate();
}

bool Wall::HitTest(const kPoint &Location, ObjectID &ObjID)
{
	kPoint P0, P1;
	ObjID = INVALID_OBJID;
	GetLocations(P0, P1);

	kPoint SegDir = P1 - P0;
	float  Len = SegDir.Size();
	if (Len <= 0)
	{
		return false;
	}

	kPoint D = SegDir*(1.0f / Len);
	kPoint Vec = Location - P0;
	kPoint vD = Vec;
	vD.Normalize();

	float fDot = D.Dot(Vec);
	kPoint projP = P0 + D*fDot;
	float fDst = (Location - projP).Size();

	float Thickness = fDot>0? GetThickLeft() : GetThickRight();
	if (fDst < Thickness)
	{
		fDot = D.Dot(projP - P0);
		if (fDot > 0 && fDot < Len)
		{
			ObjID = HitTestHole(fDot);
			return true;
		}
	}

	return false;
}

ObjectID Wall::HitTestHole(float Dist)
{
	ObjectID HitObj = INVALID_OBJID;

	std::vector<FWallHoleInfo> *Holes = GetHoles();
	if (Holes)
	{
		for (int i = 0; i < Holes->size(); ++i)
		{
			FWallHoleInfo &HoleInfo = (*Holes)[i];
			if (Dist > HoleInfo.MinX && Dist < HoleInfo.MaxX)
			{
				HitObj = HoleInfo.HoleID;
				break;
			}
		}
	}

	return HitObj;
}

bool Wall::Move(const kPoint &DeltaMove)
{
	Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(P[0], Corner);
	Corner *pCorner1 = SUITE_GET_BUILDING_OBJ(P[1], Corner);
	if (pCorner0 && pCorner1)
	{
		kPoint SaveLoc = pCorner0->Location;

		if (!pCorner0->Move(DeltaMove))
		{
			return false;
		}

		if (!pCorner1->Move(DeltaMove))
		{
			pCorner0->Location = SaveLoc;
			return false;
		}

		MarkNeedUpdate();
	}

	return true;
}

void Wall::GetCorners(std::vector<Corner *> &Corners)
{
	Corners.clear();
	if (_Suite)
	{
		Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(P[0], Corner);
		if (pCorner0)
		{
			Corners.push_back(pCorner0);
		}

		Corner *pCorner1 = SUITE_GET_BUILDING_OBJ(P[1], Corner);
		if (pCorner1)
		{
			Corners.push_back(pCorner1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
BEGIN_DERIVED_CLASS(VirtualWall, Wall)
END_CLASS()

SolidWall::SolidWall()
	: ThickLeft(20.0f)
	, ThickRight(20.0f)
	, ZPos(0.0f)
{
	Height[0] = 280.0f;
	Height[1] = 280.0f;
}

BEGIN_DERIVED_CLASS(SolidWall, Wall)
	ADD_PROP(bMainWall, BoolProperty)
	ADD_PROP(ThickLeft, FloatProperty)
	ADD_PROP(ThickRight, FloatProperty)
	ADD_PROP(ZPos, FloatProperty)
END_CLASS()

void SolidWall::Serialize(ISerialize &Ar)
{
	Wall::Serialize(Ar);
	
	Ar << Height[0];
	Ar << Height[1];

	SERIALIZE_COMPLEXVEC(Holes);
}

void SolidWall::SetWallInfo(float InThickLeft, float InThickRight, float Height0, float Height1)
{
	ThickLeft = InThickLeft;
	ThickRight = InThickRight;
	Height[0] = Height0;
	Height[1] = Height1;
}

IValue *SolidWall::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = Wall::GetFunctionProperty(name);

	if (!pValue)
	{
		if (name == "Holes")
		{
			IValue &HolesInfo = GValueFactory->Create();

			for (size_t i = 0; i < Holes.size(); ++i)
			{
				FWallHoleInfo &HoleInfo = Holes[i];
				IValue &pHole = GValueFactory->Create();
				pHole.AddField("HoleID", GValueFactory->Create(HoleInfo.HoleID));
				pHole.AddField("MinX", GValueFactory->Create(HoleInfo.MinX));
				pHole.AddField("MaxX", GValueFactory->Create(HoleInfo.MaxX));
				pHole.AddField("ObjType", GValueFactory->Create(HoleInfo.ObjType));
				HolesInfo.AddField(pHole);
			}

			pValue = &HolesInfo;
		}
		else if (name == "Height")
		{
			pValue = &GValueFactory->Create(Height[0]);
		}
	}

	return pValue;
}

void SolidWall::RemoveHoles()
{
	for (size_t i = 0; i < Holes.size(); ++i)
	{
		_Suite->DeleteObject(Holes[i].HoleID);
	}
	Holes.size();
}

void SolidWall::UpdateWallHoles()
{
	if (_Suite)
	{
		kPoint P0, P1;
		GetLocations(P0, P1);
		kPoint DL = (P1 - P0).Normalize();
		for (int i = 0; i < Holes.size(); ++i)
		{
			FWallHoleInfo &HoleInfo = Holes[i];
			WallHole *pHole = SUITE_GET_BUILDING_OBJ(HoleInfo.HoleID, WallHole);
			if (pHole)
			{
				Corner *pCorner = SUITE_GET_BUILDING_OBJ(pHole->CornerID, Corner);
				if (pCorner)
				{
					pCorner->Location = P0 + DL * (HoleInfo.MinX + pHole->Width / 2.0f);
					pCorner->MarkNeedUpdate();
				}
			}
		}
	}
}

void  SolidWall::Build()
{
	if (_Suite)
	{
		//                  /4--------/0
		//                 /  |      / |
		//                /   |     /  |
		//                6---------2  |
		//                |   5- - -| -1
		//                |  /      |  /
		//                |/        | /
		//                7---------3/ 

		FMeshSection *mesh = NULL;

		kLine l_center, l_left, l_right;
		GetBorderLines(l_center, l_left, l_right);

		// Bottom 
		kVector3D ZUpBottom = kVector3D(0, 0, ZPos);
		kVector3D ZUpTop = kVector3D(0, 0, Height[0] + ZPos);

		kVector3D ls_b = l_left.start + ZUpBottom;
		kVector3D le_b = l_left.end + ZUpBottom;

		kVector3D cs_b = l_center.start + ZUpBottom;
		kVector3D ce_b = l_center.end + ZUpBottom;

		kVector3D rs_b = l_right.start + ZUpBottom;
		kVector3D re_b = l_right.end + ZUpBottom;

		kVector3D ls_t = l_left.start + ZUpTop;
		kVector3D le_t = l_left.end + ZUpTop;

		kVector3D cs_t = l_center.start + ZUpTop;
		kVector3D ce_t = l_center.end + ZUpTop;

		kVector3D rs_t = l_right.start + ZUpTop;
		kVector3D re_t = l_right.end + ZUpTop;

		kVector3D Forward = (le_b - ls_b).Normalize();
		kVector3D Up = kVector3D(0, 0, 1.0f);
		kVector3D Right = Up.CrossProduct(Forward);

		//bottom
		mesh = AddMesh();
		AddQuad(mesh, ls_b, cs_b, ce_b, le_b, -Up, Forward, 0);
		AddQuad(mesh, cs_b, rs_b, re_b, ce_b, -Up, Forward, 0);

		//top
		mesh = AddMesh();
		AddQuad(mesh, ls_t, le_t, ce_t, cs_t, Up, Forward, 0);
		AddQuad(mesh, cs_t, ce_t, re_t, rs_t, Up, Forward, 0);

		//front ->x
		mesh = AddMesh();
		AddQuad(mesh, ls_t, cs_t, cs_b, ls_b, -Forward, Right, 0);
		AddQuad(mesh, cs_t, rs_t, rs_b, cs_b, -Forward, Right, 0);

		//back
		mesh = AddMesh();
		AddQuad(mesh, ce_t, le_t, le_b, ce_b, Forward, Right, 0);
		AddQuad(mesh, re_t, ce_t, ce_b, re_b, Forward, Right, 0);

		std::vector<p2t::IndexPoint *> polyline;
		float x = (ce_t - cs_t).Size();
		float y = (ce_t - ce_b).Size();
		polyline.push_back(new p2t::IndexPoint(0, 0, 0));
		polyline.push_back(new p2t::IndexPoint(0, y, 1));
		polyline.push_back(new p2t::IndexPoint(x, y, 2));
		polyline.push_back(new p2t::IndexPoint(x, 0, 3));
		int Offset = 4;

		p2t::CDT *CDT = new p2t::CDT(polyline);
		std::vector<ObjectID> Windows;
		std::vector<ObjectID> Doors;
		for (int i = 0; i < Holes.size(); ++i)
		{
			FWallHoleInfo &Info = Holes[i];
			if (Info.ObjType == EDoorHole)
			{
				Doors.push_back(Info.HoleID);
			}
			else if (Info.ObjType == EWindow)
			{
				Windows.push_back(Info.HoleID);
			}
		}

		//Window Holes
		AddWallHoles(CDT, Windows, Offset, polyline);
		AddWallHoles(CDT, Doors, Offset, polyline);

		CDT->Triangulate();
		std::vector<p2t::Triangle*> triangles = CDT->GetTriangles();
		int numTri = triangles.size();
		static bool bInverse = false;

		//right
		/*
			rs_t	re_t
			rs_b	re_b
		*/
		mesh = AddMesh();
		AddVertDefaultUV(mesh, rs_b, Right, Up, kPoint(0, 1.0f));
		AddVertDefaultUV(mesh, rs_t, Right, Up, kPoint(0, 0));
		AddVertDefaultUV(mesh, re_t, Right, Up, kPoint(1.0f, 0));
		AddVertDefaultUV(mesh, re_b, Right, Up, kPoint(1.0f, 1.0f));

		float xlen = (re_t - rs_t).Size();
		float ylen = (rs_t - rs_b).Size();
		for (int i = 4; i < (int)polyline.size(); ++i)
		{
			p2t::IndexPoint *p = polyline[i];
			kVector3D V = cs_b + Right * ThickRight + Forward * p->x + kVector3D(0, 0, p->y);
			float ux = p->x / xlen;
			float uy = p->y / ylen;
			AddVertDefaultUV(mesh, V, Right, Up, kPoint(ux, uy));
		}
		AddTris(mesh, triangles, bInverse);

		//left
		/*
		ls_t	le_t
		ls_b	le_b
		*/
		mesh = AddMesh();
		AddVertDefaultUV(mesh, ls_b, -Right, Up, kPoint(0, 1.0f));
		AddVertDefaultUV(mesh, ls_t, -Right, Up, kPoint(0, 0));
		AddVertDefaultUV(mesh, le_t, -Right, Up, kPoint(1.0f, 0));
		AddVertDefaultUV(mesh, le_b, -Right, Up, kPoint(1.0f, 1.0f));

		xlen = (le_t - ls_t).Size();
		ylen = (ls_t - ls_b).Size();
		for (int i = 4; i < (int)polyline.size(); ++i)
		{
			p2t::IndexPoint *p = polyline[i];
			kVector3D V = cs_b - Right * ThickLeft + Forward * p->x + kVector3D(0, 0, p->y);
			float ux = p->x / xlen;
			float uy = p->y / ylen;
			AddVertDefaultUV(mesh, V, -Right, Up, kPoint(ux, uy));
		}
		AddTris(mesh, triangles, !bInverse);

		for (int i = 0; i < polyline.size(); ++i)
		{
			if (polyline[i])
			{
				delete polyline[i];
			}
		}

		polyline.clear();

		int MaterialOffset = 4;
		BrushHoleMeshs(Windows, MaterialOffset, cs_b, Forward, Right, ThickLeft, ThickRight);
		BrushHoleMeshs(Doors, MaterialOffset, cs_b, Forward, Right, ThickLeft, ThickRight);

		if (CDT)
		{
			delete CDT;
		}
	}
}

void SolidWall::AddWallHoles(p2t::CDT *CDT, std::vector<ObjectID> &Holes, int &Offset, std::vector<p2t::IndexPoint*> &polyline)
{
	for (int i = 0; i < Holes.size(); ++i)
	{
		WallHole *pWallHole = SUITE_GET_BUILDING_OBJ(Holes[i], WallHole);
		if (pWallHole)
		{
			int Num = 0;
			kPoint *pPolygon = nullptr;
			std::vector<p2t::IndexPoint *> hole;
			pWallHole->GetPolygon(pPolygon, Num);
			for (int j = 0; j < Num; ++j)
			{
				static bool bInv = false;
				p2t::IndexPoint *pt = nullptr;
				if (bInv)
				{
					pt = new p2t::IndexPoint(pPolygon[Num - j - 1].X, pPolygon[Num - j - 1].Y, Offset + j);
				}
				else
				{
					pt = new p2t::IndexPoint(pPolygon[j].X, pPolygon[j].Y, Offset + j);
				}
				hole.push_back(pt);
				polyline.push_back(pt);
			}
			Offset += Num;
			CDT->AddHole(hole);
		}
	}
}

void SolidWall::BrushHoleMeshs(std::vector<ObjectID> &Holes, int &MaterialOffset, const kVector3D &Base, const kVector3D &Forward, const kVector3D &Right, float InThickLeft, float InThickRight)
{
	for (int i = 0; i < Holes.size(); ++i)
	{
		WallHole *pWallHole = SUITE_GET_BUILDING_OBJ(Holes[i], WallHole);
		if (pWallHole)
		{
			int Num = 0;
			kPoint *pPolygon = nullptr;
			pWallHole->GetPolygon(pPolygon, Num);

			FMeshSection *mesh = AddMesh();

			kVector3D LV0 = Base + pPolygon[0].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[0].Y);
			kVector3D LV1 = Base + pPolygon[0].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[0].Y);
			kVector3D LV2 = Base + pPolygon[1].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[1].Y);
			kVector3D LV3 = Base + pPolygon[1].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[1].Y);
			AddQuad(mesh, LV0, LV1, LV2, LV3, Forward, Right, MaterialOffset + i);

			kVector3D TV0 = Base + pPolygon[1].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[1].Y);
			kVector3D TV1 = Base + pPolygon[1].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[1].Y);
			kVector3D TV2 = Base + pPolygon[2].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[2].Y);
			kVector3D TV3 = Base + pPolygon[2].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[2].Y);
			AddQuad(mesh, TV0, TV1, TV2, TV3, kVector3D(0.7f, 0, 0.7f), Right, MaterialOffset + i);

			kVector3D RV0 = Base + pPolygon[2].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[2].Y);
			kVector3D RV1 = Base + pPolygon[2].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[2].Y);
			kVector3D RV2 = Base + pPolygon[3].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[3].Y);
			kVector3D RV3 = Base + pPolygon[3].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[3].Y);
			AddQuad(mesh, RV0, RV1, RV2, RV3, -Forward, Right, MaterialOffset + i);

			kVector3D BV0 = Base + pPolygon[3].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[3].Y);
			kVector3D BV1 = Base + pPolygon[3].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[3].Y);
			kVector3D BV2 = Base + pPolygon[0].X*Forward + Right * InThickRight + kVector3D(0, 0, pPolygon[0].Y);
			kVector3D BV3 = Base + pPolygon[0].X*Forward - Right * InThickLeft + kVector3D(0, 0, pPolygon[0].Y);
			AddQuad(mesh, BV0, BV1, BV2, BV3, kVector3D(0, 0, 1.0f), Right, MaterialOffset + i);

			++MaterialOffset;
		}
	}
}

void SolidWall::AddTris(FMeshSection *mesh, std::vector<p2t::Triangle*> &triangles, bool bInverse)
{
	for (int i = 0; i < (int)triangles.size(); ++i)
	{
		p2t::Triangle *tri = triangles[i];
		if (tri)
		{
			int tri_index[3] = {};
			for (int c = 0; c < 3; ++c)
			{
				tri_index[c] = tri->GetPoint(c)->refIndex;
			}
			if (bInverse)
			{
				AddTri(mesh, tri_index[0], tri_index[2], tri_index[1]);
			}
			else
			{
				AddTri(mesh, tri_index[0], tri_index[1], tri_index[2]);
			}
		}
	}
}

void SolidWall::AddHole(WallHole *pHole, const kPoint &Location, float InZPos, float InHeight, float InWidth)
{
	if (pHole)
	{
		pHole->ZPos = InZPos;
		pHole->Width = InWidth;
		pHole->Height = InHeight;
		pHole->WallID = GetID();

		Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(P[0], Corner);
		if (pCorner0)
		{
			float Dist = (Location - pCorner0->Location).Size();
			float MinX = Dist - InWidth / 2.0f;
			float MaxX = Dist + InWidth / 2.0f;

			size_t n = Holes.size();
			Holes.resize(n + 1);

			int i = n;
			for (; i > 0; --i)
			{
				if (Holes[i - 1].MinX < MinX)
				{
					break;
				}
				Holes[i] = Holes[i - 1];
			}

			FWallHoleInfo &Info = Holes[i];
			Info.HoleID = pHole->GetID();
			Info.ObjType = pHole->GetType();
			Info.MinX = MinX;
			Info.MaxX = MaxX;
		}
	}
}

void SolidWall::CopyHoles(std::vector<FWallHoleInfo> &InHoles)
{
	Holes = InHoles;
}

void SolidWall::OnDestroy()
{
	RemoveHoles();
	Wall::OnDestroy();
}

void SolidWall::MarkNeedUpdate()
{
	UpdateWallHoles();
	Wall::MarkNeedUpdate();
}

