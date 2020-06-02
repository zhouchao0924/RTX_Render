
#pragma once

#include "IClass.h"
#include "Math/kVector2D.h"

enum EShapeType
{
	EShape_Polygon,
	EShape_Rectangle,
};

class FPaveShape
{
public:
	EShapeType GetType() { return EShape_Polygon; }
	virtual kArray<kPoint> GetPolygons() { return kArray<kPoint>(points); }
protected:
	std::vector<kPoint> points;
};

class FShapeInstance
{
public:
	FShapeInstance()
		: pShape(nullptr)
		, bDirty(true)
	{
	}
	virtual int GetMaterialGroupID() = 0;
	virtual kArray<kPoint> CaclShapePoints() = 0;
protected:
	int		Id;
	FPaveShape *pShape;
	kPoint  Offset;
	float   Angle;
	int		MaterialGroupID;
	bool	bDirty;
	std::vector<kPoint> points;
};

struct FPaveStyleJoint
{
	kPoint SlotLocation;
	kPoint PinLocation;
};

class FPaveStyle
{
public:
	struct FMaterialGroup 
	{
		int Id;
		std::vector<std::string> materials;
	};

	virtual int AddShape(EShapeType Type, const kPoint &Offset, float Angle) = 0;
	virtual int SetShapeOffset(int ShapeID, const kPoint &Offset) = 0;
	virtual int SetShapeAngle(int ShapeID, const kPoint &Offset) = 0;

	virtual void SetPivotPoint(const kPoint &pivotPoint) = 0;
	virtual void SetVerticalJoint(const kPoint &SlotLocation, const kPoint &PinLocation) = 0;
	virtual void SetHorizontalJoint(const kPoint &SlotLocation, const kPoint &PinLocation) = 0;

	virtual int  CreateMaterialGroup(const char *mxResID) = 0;
	virtual int  AddMaterialToGroup(int GroupID, const char *mxResID) = 0;
	virtual void ClearMaterialGroup(int GroupID) = 0;
	virtual void SetShapeMaterial(int ShapeID, int MaterialGroupID) = 0;
	virtual void SetIntervalSize(float IntervalSize) = 0;

	//使用者
	virtual int GetShapeCount() = 0;
	virtual FShapeInstance *GetShapeInstance(int ShapeID) = 0;
	virtual int GetMaterialGroupCount() = 0;
	virtual int GetMaterialCountInGroup(int GroupID) = 0;
	virtual const char *GetMaterial(int GroupID, int MaterialIndex) = 0;
	virtual kPoint GetSize() = 0;
	virtual kPoint GetPivot() { return pivotPoint; }
	virtual const FPaveStyleJoint &GetHorizontalJoint() { return horizontalJoint; }
	virtual const FPaveStyleJoint &GetVerticalJointJoint() { return verticalJoint; }
protected:
	std::vector<FShapeInstance>	  shapes;
	std::vector<FMaterialGroup *> materialGroups;
	FPaveStyleJoint				  horizontalJoint;
	FPaveStyleJoint				  verticalJoint;
	kPoint						  pivotPoint;
};

enum EPaveMethod
{
	//铺贴方法  工字铺   人字铺  鱼骨铺。。。。
};

struct FPaveParameters
{
	EPaveMethod Method;
	float		Angle;
	kPoint		Offset;
	float		IntervalSize;
};

class IPave
{
public:
// 	virtual ObjectID PaveArea(ObjectID AreaID, ObjectID StyleID, FPaveParameters &Params) = 0;
	virtual ObjectID PaveArea(ObjectID AreaID, const FPaveStyle &Style, FPaveParameters &Params) = 0;
};


