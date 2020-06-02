
#include "Corner.h"
#include "ISuite.h"
#include "kBox.h"
#include "Wall.h"
#include "kLine2D.h"
#include "Class/Property.h"

const float const_minAngle = 30.0f;
const float const_minDist = 40.0f;

BEGIN_CLASS(Corner)
	ADD_PROP(Location, Vec2DProperty)
END_CLASS()

Corner::Corner()
	: Baton(nullptr)
	, RelativeID(INVALID_OBJID)
{
}

void Corner::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);
	SERIALIZE_VEC(ConnectedWalls)
	SERIALIZE_VEC(Angles);
}

void Corner::MarkNeedUpdate()
{
	BuildingObject::MarkNeedUpdate();

	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		BuildingObject *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i], Wall);
		if (pWall)
		{
			pWall->MarkNeedUpdate();
		}
	}
}

void Corner::AddWall(Wall *pWall)
{
	if (pWall)
	{
		kPoint	 Direction;
		ObjectID CornerID = GetID();

		kPoint P0, P1;
		pWall->GetLocations(P0, P1);

		if (pWall->P[0] == CornerID)
		{
			Direction = P1 - P0;
		}
		else if (pWall->P[1]==CornerID)
		{
			Direction = P0 - P1;
		}

		Direction.Normalize();

		float Degree = Direction.GetAngle();
		size_t n = ConnectedWalls.size();

		Angles.resize(n + 1);
		ConnectedWalls.resize(n + 1);

		size_t i = n;
		for (; i > 0; --i)
		{
			if (Angles[i - 1] > Degree)
			{
				Angles[i] = Angles[i - 1];
				ConnectedWalls[i] = ConnectedWalls[i - 1];
			}
			else
			{
				break;
			}
		}
		Angles[i] = Degree;
		ConnectedWalls[i] = pWall->GetID();

		MarkNeedUpdate();
	}
}

void Corner::SetLocation(const kPoint &Loc)
{
	Location = Loc;
	MarkNeedUpdate();
}
	
void Corner::GetNearByWall(ObjectID WallID, ObjectID &LeftWall, ObjectID &RightWall)
{
	LeftWall = RightWall = INVALID_OBJID;

	int l_index, r_index;
	GetNearByWallIndex(WallID, l_index, r_index);
		
	if (l_index >= 0)
	{
		LeftWall = ConnectedWalls[l_index];
	}

	if (r_index >= 0)
	{
		RightWall = ConnectedWalls[r_index];
	}
}

int Corner::GetWallIndex(ObjectID WallID)
{
	int foundIndex = -1;
	int n = (int)ConnectedWalls.size();

	for (int i = 0; i < n; ++i)
	{
		if (ConnectedWalls[i] == WallID)
		{
			foundIndex = i;
			break;
		}
	}

	return foundIndex;
}

void Corner::GetNearByWallIndex(ObjectID WallID, int &LeftWall, int &RightWall)
{
	LeftWall = RightWall = -1;

	int n = (int)ConnectedWalls.size();
	if (n > 1)
	{
		for (size_t i = 0; i < n; ++i)
		{
			if (ConnectedWalls[i] == WallID)
			{
				if (i > 0)
				{
					RightWall = i - 1;
				}
				else if (ConnectedWalls.size() > 2)
				{
					RightWall = n - 1;
				}

				if (i < ConnectedWalls.size() - 1)
				{
					LeftWall = i + 1;
				}
				else if (ConnectedWalls.size() > 2)
				{
					LeftWall = 0;
				}
			}
		}
	}
}

bool Corner::IsConnect(ObjectID CornerID)
{
	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i], Wall);
		if (pWall)
		{
			if (CornerID == pWall->GetOtherCorner(_ID))
			{
				return true;
			}
		}
	}
	return false;
}

void Corner::RemoveWall(ObjectID WallID)
{
	if (WallID != INVALID_OBJID)
	{
		for (size_t i = 0; i < ConnectedWalls.size(); ++i)
		{
			if (ConnectedWalls[i] == WallID)
			{
				ConnectedWalls.erase(ConnectedWalls.begin() + i);
				Angles.erase(Angles.begin() + i);
				MarkNeedUpdate();
				break;
			}
		}
	}
}

bool Corner::IsFree()
{
	return ConnectedWalls.size() == 0 && RelativeID==INVALID_OBJID;
}

bool Corner::IsValid()
{
	int iMin = 0;
	std::vector<float> newConnAngles;
	RecalcAngles(newConnAngles, nullptr);
	return IsValid(newConnAngles, iMin);
}

bool Corner::IsWallDistValid()
{
	if (_Suite)
	{
		const float const_MinDistSQ = const_minDist*const_minDist;
		IObject ** ppWalls = nullptr;
		int nWall = _Suite->GetAllObjects(ppWalls, EWall);
			
		std::vector<kLine2D> ConnWallLines;
		for (size_t i = 0; i < ConnectedWalls.size(); ++i)
		{
			Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i], Wall);
			if (pWall)
			{
				kPoint P0, P1;
				pWall->GetLocations(P0, P1);
				ConnWallLines.push_back(kLine2D(P0, P1));
			}
		}

		for (int i = 0; i < nWall; ++i)
		{
			Wall *pWall = (Wall *)ppWalls[i];
			if (pWall->P[0] == _ID || pWall->P[1] == _ID)
			{
				continue;
			}

			kPoint P0, P1;
			pWall->GetLocations(P0, P1);

			kPoint Vec = P1 - P0;
			float Len = Vec.Size();
			if (Len > 0)
			{
				kPoint N = Vec / Len;
				kPoint Dir = Location - P0;
				float fDot = Dir.Dot(N);

				float DistSQ = 0;
				if (fDot < 0)
				{
					DistSQ = Dir.SizeSquared();
				}
				else if (fDot > Len)
				{
					DistSQ = (Location - P1).SizeSquared();
				}
				else
				{
					kPoint ProjP = P0 + N*fDot;
					DistSQ = (Location - ProjP).SizeSquared();
				}

				if (DistSQ < const_MinDistSQ)
				{
					return false;
				}

				float Ratio = 0;
				for (size_t k = 0; k < ConnWallLines.size(); ++k)
				{
					if (ConnWallLines[k].GetIntersection(kLine2D(P0, P1), Ratio) && Ratio> 0 && Ratio<1.0f)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}

bool Corner::Move(const kPoint &DeltaMove)
{
	kPoint SavedLoc = Location;
	Location += DeltaMove;

	std::vector<float>	newAngles;
	std::vector<ObjectID> connCorners;

	RecalcAngles(newAngles, &connCorners);

	int iMin = 0, n = (int)ConnectedWalls.size();
	if (!IsValid(newAngles, iMin)) // self valid check
	{
		Location = SavedLoc;
		return false;
	}

	for (size_t i = 0; i < connCorners.size(); ++i) //connnect corner valid check
	{
		Corner *pConnCorner = SUITE_GET_BUILDING_OBJ(connCorners[i], Corner);
		if (!pConnCorner || !pConnCorner->IsValid())
		{
			Location = SavedLoc;
			return false;
		}
	}
		
	//intersection check
	if (!IsWallDistValid())
	{
		Location = SavedLoc;
		return false;
	}

	//resort connnect wall
	std::vector<ObjectID> newConnectWalls;
	newConnectWalls.resize(n);

	for (int i = 0; i < n; i++)
	{
		Angles[i] = newAngles[(i + iMin) % n];
		newConnectWalls[i] = ConnectedWalls[(i + iMin) % n];
	}

	ConnectedWalls.swap(newConnectWalls);

	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i], Wall);
		if (pWall)
		{
			ObjectID IDCon = pWall->GetOtherCorner(_ID);
			Corner *pCornerCon = SUITE_GET_BUILDING_OBJ(IDCon, Corner);
			if(pCornerCon)
			{
				pCornerCon->MarkNeedUpdate();
			}
		}
	}

	MarkNeedUpdate();

	return true;
}

void Corner::RecalcAngles(std::vector<float> &newAngles, std::vector<ObjectID> *connCorners)
{
	size_t n = ConnectedWalls.size();
	newAngles.resize(n);

	if (connCorners)
	{
		connCorners->resize(n);
	}

	kPoint Direction;
	for (size_t i = 0; i < ConnectedWalls.size(); ++i)
	{
		kPoint P0, P1;

		Wall *pWall = SUITE_GET_BUILDING_OBJ(ConnectedWalls[i], Wall);
		pWall->GetLocations(P0, P1);

		if (pWall->P[0] == _ID)
		{
			Direction = P1 - P0;
			if (connCorners)
			{
				(*connCorners)[i] = pWall->P[1];
			}
		}
		else if (pWall->P[1] == _ID)
		{
			Direction = P0 - P1;
			if (connCorners)
			{
				(*connCorners)[i] = pWall->P[0];
			}
		}

		Direction.Normalize();
		float Degree = Direction.GetAngle();
		newAngles[i] = Degree;
	}
}

bool Corner::IsValid(std::vector<float> &newAngles, int &iMin)
{
	iMin = 0;
	float minAngle = 400.0f;
	for (size_t i = 0; i < newAngles.size(); ++i)
	{
		if (newAngles[i] < minAngle)
		{
			minAngle = newAngles[i];
			iMin = i;
		}
	}

	float delta = 0;
	int n = newAngles.size();
	if (n > 1)
	{
		float preAngle = newAngles[iMin];
		for (int i = 1; i < n; ++i)
		{
			float curAngle = newAngles[(i + iMin) % n];
			delta = curAngle - preAngle;

			if (delta < 0) //角点相连的墙体穿插检测
			{
				return false;
			}

			if (delta < const_minAngle) //角度限制检测
			{
				return false;
			}

			preAngle = curAngle;
		}

		delta = newAngles[iMin] - preAngle;
		assert(delta < 0);

		if (-delta < const_minAngle)
		{
			return false;
		}
	}

	return true;
}


