
#pragma once

#include <set>
#include <unordered_map>
#include "Elements/Corner.h"

struct  FCell
{
	int X, Y;
	std::set<Corner *> Objs;
	void Add(Corner *pCorner);
	bool Del(Corner *pCorner);
	bool Find(Corner *pCorner);
	int  Num();
};

typedef __int64 CellID;
typedef std::unordered_map<CellID, FCell *>  CellMap;

class SparseMap
{
public:
	void Init(int InCellSize);
	void Clear();
	void Add(Corner *pCorner);
	bool Del(Corner *pCorner);
	bool FindByRadius(const kPoint &Loc, float Radius, std::vector<Corner *> &Corners, bool bOrder);
	bool FindByBox2D(const kPoint &Min, const kPoint &Max, std::vector<Corner *> &Corners);
	bool FindByLine(const kPoint &StartLoc, const kPoint &Direction, float Dist, std::vector<Corner *> &Corners);
protected:
	CellID ToCellID(const kPoint &Loc);
	float GetMinProjDistToLine(int X, int Y, const kPoint &StartLoc, const kPoint &Direction);
	void GetCells(int X0, int Y0, int X1, int Y1, std::vector<FCell *> &Cells);
	FCell *GetCeil(const kPoint &Loc, bool bCreateIfNotExsit, CellID *pID = nullptr);
private:
	int					 m_CellSize;
	CellMap				 m_CellMap;
	std::vector<FCell *> m_FreeCells;
};



