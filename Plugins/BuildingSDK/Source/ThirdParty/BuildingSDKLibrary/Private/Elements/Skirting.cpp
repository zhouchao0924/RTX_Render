
#include "DoorHole.h"
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "Skirting.h"
#include "Room.h"
#include "ModelInstance.h"
#include "kVector3D.h"
#include <algorithm>
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(Skirting, Anchor)
	ADD_PROP(RoomID, IntProperty)
	ADD_PROP(Extent, Vec3DProperty)
	ADD_PROP(SkirtingResID, StdStringProperty)
END_CLASS()

Skirting::Skirting()
	:RoomID(INVALID_OBJID)
	,SkirtingType(SKirtingNone)
{
}

void Skirting::Serialize(ISerialize &Ar)
{
	Anchor::Serialize(Ar);

	SERIALIZE_VEC(SkirtingModels);
	KSERIALIZE_ENUM(ESkirtingType, SkirtingType);
}

void Skirting::OnCreate()
{
	Room *pRoom = SUITE_GET_BUILDING_OBJ(RoomID, Room);

	if (pRoom && SkirtingResID.length()>0)
	{
		std::vector<FSegmentPoint> Points;
		std::vector<int> Segments;
		pRoom->GetSegments(Segments, Points, SkirtingType== ESkirtingBottom);
			
		int nPoints = (int)Points.size();
		int nSegments = (int)Segments.size();

		for (size_t iSeg = 0; iSeg < Segments.size(); ++iSeg)
		{
			int iStart = Segments[iSeg];
			int nPolygon = (iSeg < Segments.size() - 1) ? (Segments[iSeg + 1] - iStart) : (Points.size() - iStart);
			FSegmentPoint *pPolygon = &Points[iStart];

			//结束点为对象 不再链接
			bool bClosed = true;
			int Num = nPolygon;

			FSegmentPoint &StartPos = pPolygon[0];
			FSegmentPoint &TerminatePos = ((iStart + Num)==nPoints)? Points[0] : Points[iStart + Num - 1];
			if (TerminatePos.ID != INVALID_OBJID)
			{
				bClosed = false;
				Num = nPolygon - 1;
			}

			kPoint CurrentPos, NextPos, PrePos;
			std::vector<kVector3D> ClipNormals;
			ClipNormals.resize(Num+1);

			for (int i = 0; i < Num+1; ++i)
			{
				int Index = iStart + i;

				FSegmentPoint &SegPt = Points[Index%nPoints];
				CurrentPos = SegPt.Point;

				kPoint HalfD;
				if (SegPt.ID != INVALID_OBJID)
				{
					ClipNormals[i] = -SegPt.Forward;
				}
				else
				{
					PrePos = Points[(nPoints + Index - 1) % nPoints].Point;	
					NextPos = Points[(Index + 1) % nPoints].Point;

					kPoint V0 = (CurrentPos - PrePos).Normalize();
					kPoint V1 = (NextPos - CurrentPos).Normalize();

					if (std::abs(V0.Dot(V1)) > 0.99f)
					{
						HalfD = HalfD.RotateByAngle(V0, 1.57079632679489661923);
					}
					else
					{
						HalfD = ((V1 - V0)*0.5f).Normalize();
						if (!SegPt.bCW)
						{
							HalfD = -HalfD;
						}
					}

					kVector3D Normal = kVector3D(0, 0, 1.0f).CrossProduct(HalfD);
					ClipNormals[i] = Normal;
				}
			}

			for (int i = 0; i < Num; ++i)
			{
				int index = i + iStart;

				FSegmentPoint &SegP0 = Points[index];
				FSegmentPoint &SegP1 = Points[(index+1)%nPoints];

				float H0 = 0, H1 = 0;
				if (SkirtingType == ESkirtingTop)
				{
					H0 = SegP0.Height - Extent.Z / 2.0f;
					H1 = SegP1.Height - Extent.Z / 2.0f;
				}
				else
				{
					H0 = SegP0.Height + Extent.Z / 2.0f;
					H1 = SegP1.Height + Extent.Z / 2.0f;
				}
				AddSkirtingModel(SegP0.Inner, SegP0.Point, SegP1.Point, ClipNormals[i], ClipNormals[i+1], H0, H1);
			}
		}
	}

	Anchor::OnCreate();
}

void Skirting::OnDestroy()
{
	for (size_t i = 0; i < SkirtingModels.size(); ++i)
	{
		ObjectID ID = SkirtingModels[i];
		_Suite->DeleteObject(ID, true);
	}

	SkirtingModels.clear();
		
	Anchor::OnDestroy();
}

void Skirting::AddSkirtingModel(const kPoint &Right, const kPoint &P0, const kPoint &P1, const kVector3D &Normal0, const kVector3D &Normal1, float Height0, float Height1)
{
	EModelAlignType	AlignType = EAlignFrontCenter;

	kPoint Forward = P1 - P0;
	float  Len = Forward.Size();

	if (Len > 0)
	{
		Forward = Forward * (1.0f/Len);
			
		static float Scale = 1.0f;
		float Offset = -std::max<float>(Extent.Y, Extent.Z)*Scale;
		float ClipLen = Len - Offset;

		float RightOffset = Extent.Y / 2.0f;

		if (ClipLen < Extent.X + Offset)
		{
			//start clip plane/end clip plane
			kPoint Loc = P0 + Right*RightOffset;
				
			ObjectID ModelID = _Suite->AddModelToAnchor(_ID, SkirtingResID.c_str(), Loc);
			ModelInstance *pModel = (ModelInstance *)_Suite->GetObject(ModelID, EModelInstance);
			if (pModel)
			{
				pModel->Forward = Forward;
				pModel->ClipBase0 = P0;
				pModel->ClipBase1 = P1;
				pModel->ClipNormal0 = -Normal0;
				pModel->ClipNormal1 = Normal1;
				pModel->Location = kVector3D(Loc.x, Loc.y, Height0);
				pModel->AlignType = AlignType;
				SkirtingModels.push_back(pModel->GetID());
			}
		}
		else
		{
			int nSegments = 0;

			while (ClipLen > Extent.X)
			{
				kPoint Loc = P0 + Forward*Offset + Right*RightOffset;

				ObjectID ModelID = _Suite->AddModelToAnchor(_ID, SkirtingResID.c_str(), Loc);
				ModelInstance *pModel = (ModelInstance *)_Suite->GetObject(ModelID, EModelInstance);
				if (pModel)
				{
					pModel->Forward = Forward;
					pModel->Location = kVector3D (Loc.x, Loc.y, Height0);
					pModel->AlignType = AlignType;
					SkirtingModels.push_back(pModel->GetID());
				}

				if (nSegments <= 0)
				{
					pModel->ClipBase0 = P0;
					pModel->ClipNormal0 = -Normal0;
				}

				++nSegments;
				Offset += Extent.X;
				ClipLen -= Extent.X;
			}

			if (Offset < Len && ClipLen>0)
			{
				//end clip plane
				kPoint Loc = P0 + Forward*Offset + Right*RightOffset;

				ObjectID ModelID = _Suite->AddModelToAnchor(_ID, SkirtingResID.c_str(), Loc);
				ModelInstance *pModel = (ModelInstance *)_Suite->GetObject(ModelID, EModelInstance);
				if (pModel)
				{
					pModel->Forward = Forward;
					pModel->ClipBase1 = P1;
					pModel->ClipNormal1 = Normal1;
					pModel->Location = kVector3D(Loc.x, Loc.y, Height0);
					pModel->AlignType = AlignType;
					SkirtingModels.push_back(pModel->GetID());
				}
			}
		}
	}
}
	
void Skirting::Link(ModelInstance *InModel)
{
	Anchor::Link(InModel);
}


