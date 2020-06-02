
#include "SparseMap.h"

void FCell::Add(Corner *pCorner)
{
	if (!Find(pCorner))
	{
		Objs.insert(pCorner);
	}
}

bool FCell::Del(Corner *pCorner)
{
	std::set<Corner *>::iterator it = Objs.find(pCorner);
	if (it != Objs.end())
	{
		Objs.erase(it);
		return true;
	}
	return false;
}

bool FCell::Find(Corner *pCorner)
{
	return Objs.find(pCorner) != Objs.end();
}

int  FCell::Num()
{
	return (int)Objs.size();
}

//////////////////////////////////////////////////////////////////////////
void SparseMap::Init(int InCellSize)
{
	m_CellSize = InCellSize;
}

CellID SparseMap::ToCellID(const kPoint &Loc)
{
	int X = std::floor(Loc.X / m_CellSize);
	int Y = std::floor(Loc.Y / m_CellSize);
	return CellID( 32<<X | Y);
}

FCell *SparseMap::GetCeil(const kPoint &Loc, bool bCreateIfNotExsit, CellID *pID)
{
	FCell *pCell = nullptr;

	int X = std::floor(Loc.X / m_CellSize);
	int Y = std::floor(Loc.Y / m_CellSize);
	CellID ID = CellID(32 << X | Y);
		
	CellMap::iterator it = m_CellMap.find(ID);
	if (it != m_CellMap.end())
	{
		pCell = it->second;
	}

	if (!pCell && bCreateIfNotExsit)
	{
		if (!m_FreeCells.empty())
		{
			pCell = m_FreeCells.back();
			m_FreeCells.pop_back();
		}
		else
		{
			pCell = new FCell();
		}
			
		pCell->X = X;
		pCell->Y = Y;

		m_CellMap[ID] = pCell;
	}

	if (pID)
	{
		*pID = ID;
	}

	return pCell;
}

void  SparseMap::Add(Corner *pCorner)
{
	FCell *pCell = GetCeil(pCorner->Location, true);
	if (pCell)
	{
		pCell->Add(pCorner);
	}
}

bool SparseMap::Del(Corner *pCorner)
{
	CellID ID = 0;
	FCell *pCell = GetCeil(pCorner->Location, false, &ID);
	if (pCell)
	{
		if (pCell->Del(pCorner))
		{
			if (pCell->Num() == 0)
			{
				m_CellMap[ID] = pCell;
				m_FreeCells.push_back(pCell);
			}
			return true;
		}
	}
	return false;
}

bool SparseMap::FindByRadius(const kPoint &Loc, float Radius, std::vector<Corner *> &Corners, bool bOrder)
{
	kPoint Min = Loc - kPoint(Radius, Radius);
	kPoint Max = Loc + kPoint(Radius, Radius);

	std::vector<float> dists;
	std::vector<Corner *> RangeCorners;
	if (FindByBox2D(Min, Max, RangeCorners))
	{
		float radSQ = Radius*Radius;
		for (size_t i = 0; i < RangeCorners.size(); ++i)
		{
			Corner *pCorner = RangeCorners[i];
			float distSQ = (pCorner->Location - Loc).SizeSquared();
			if (distSQ < radSQ)
			{
				if (bOrder)
				{
					size_t n = Corners.size();
					Corners.resize(n + 1);
					dists.resize(n + 1);
					int j = n;
					for (; j > 0; --j)
					{
						if (distSQ < dists[n - 1])
						{
							dists[n] = dists[n - 1];
							Corners[n] = Corners[n - 1];
						}
						else
						{
							break;
						}
					}
					dists[j] = distSQ;
					Corners[j] = pCorner;
				}
				else
				{
					Corners.push_back(pCorner);
				}
			}
		}
	}
	return !Corners.empty();
}

bool SparseMap::FindByBox2D(const kPoint &Min, const kPoint &Max, std::vector<Corner *> &Corners)
{
	int X0 = std::floor(Min.X / m_CellSize);
	int Y0 = std::floor(Min.Y / m_CellSize);
	int X1 = std::floor(Max.X / m_CellSize);
	int Y1 = std::floor(Max.Y / m_CellSize);
		
	std::vector<FCell *> Cells;
	GetCells(X0, Y0, X1, Y1, Cells);

	for (size_t i = 0; i < Cells.size(); ++i)
	{
		FCell *pCell = Cells[i];
		for (std::set<Corner *>::iterator it = pCell->Objs.begin(); it!=pCell->Objs.end(); ++it)
		{
			Corner *pCorner = *it;
			if ( pCorner->Location.X>Min.X && pCorner->Location.X<Max.X && 
					pCorner->Location.Y>Min.Y && pCorner->Location.Y<Max.Y)
			{
				Corners.push_back(pCorner);
			}
		}
	}
	return !Corners.empty();
}

bool SparseMap::FindByLine(const kPoint &StartLoc, const kPoint &Direction, float Dist, std::vector<Corner *> &Corners)
{
	float radDistSQ = Dist * Dist;
	std::vector<float> dists;

	for (CellMap::iterator it = m_CellMap.begin(); it != m_CellMap.end(); ++it)
	{
		FCell *pCell = it->second;
		if (pCell)
		{
			for (std::set<Corner *>::iterator itCorner = pCell->Objs.begin(); itCorner != pCell->Objs.end(); ++itCorner)
			{
				Corner *pCorner = *itCorner;
				kPoint Dir = pCorner->Location - StartLoc;
				float fDot = Direction.Dot(Dir);
				kPoint Proj = StartLoc + Direction*fDot;
				float distSQ = (pCorner->Location - Proj).SizeSquared();
				if (distSQ < radDistSQ)
				{
					int n = (int) Corners.size();
					Corners.resize(n + 1);
					dists.resize(n + 1);
					int i = n;
					for (; i > 0; --i)
					{
						if (distSQ < dists[i-1])
						{
							Corners[i] = Corners[i - 1];
							dists[i] = dists[i - 1];
						}
						else
						{
							break;
						}
					}
					dists[i] = distSQ;
					Corners[i] = pCorner;
				}
			}
		}
	}

	return !Corners.empty();
}

void SparseMap::GetCells(int X0, int Y0, int X1, int Y1, std::vector<FCell *> &Cells)
{
	for (int X = X0; X <= X1; ++X)
	{
		for (int Y = Y0; Y <= Y1; ++Y)
		{
			CellID ID = (32<<X | Y);
			CellMap::iterator it = m_CellMap.find(ID); 
			if (it != m_CellMap.end())
			{
				FCell *pCell = it->second;
				if (pCell)
				{
					Cells.push_back(pCell);
				}
			}
		}
	}
}

void SparseMap::Clear()
{
	for (CellMap::iterator it = m_CellMap.begin(); it != m_CellMap.end(); ++it)
	{
		FCell *pCell = it->second;
		if (pCell)
		{
			delete pCell;
		}
	}
	m_CellMap.clear();

	for (size_t i = 0; i < m_FreeCells.size(); ++i)
	{
		FCell *pCell = m_FreeCells[i];
		if (pCell)
		{
			delete pCell;
		}
	}
	m_FreeCells.clear();
}



