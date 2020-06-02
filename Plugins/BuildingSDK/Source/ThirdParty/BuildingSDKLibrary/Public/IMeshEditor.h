
#pragma once

#include "IMeshObject.h"
#include "Math/kXform.h"

/*
---------------->
|		|		|
|		|		|
---------------->X
*/

enum ETAlign
{
	EAlign_Original,

	EAlign_PlaneZLow,
	EAlign_PlaneZHigh,
	EAlign_PlaneYLow,
	EAlign_PlaneYHigh,
	EAlign_PlaneXLow,
	EAlign_PlaneXHigh,

	EAlign_EdgeZHXL,
	EAlign_EdgeZHXH,
	EAlign_EdgeYLZH,
	EAlign_EdgeYHZH,

	EAlign_EdgeZLXL,
	EAlign_EdgeZLXH,
	EAlign_EdgeYLZL,
	EAlign_EdgeYHZL,

	EAlign_EdgeXLYL,
	EAlign_EdgeXLYH,
	EAlign_EdgeXHYL,
	EAlign_EdgeXHYH,

	EAlign_PointXLYLZL,
	EAlign_PointXLYHZL,	
	EAlign_PointXHYLZL,
	EAlign_PointXHYHZL,

	EAlign_PointXLYLZH,
	EAlign_PointXLYHZH,
	EAlign_PointXHYLZH,
	EAlign_PointXHYHZH,
};

typedef int ElementID;
typedef int	PinID;
#define INVALID_ELEMENT -1
#define INVALID_PIN		-1

struct FElementInterface
{
	ElementID		ElemID;
	kArray<PinID>	Pins;
};

enum EKeepRuleType
{
	EKeepLocation,
	EKeepPin,
};

class ICompoundModel
{
public:
	virtual ElementID	AddElement(ObjectID CompoundID, const char *ResID) = 0;
	virtual bool		DeleteElement(ObjectID CompoundID, ElementID ID) = 0;
	virtual bool		GetElementSize(ObjectID CompoundID, ElementID ID, int &Width, int &Height, int &Depth) = 0;
	virtual bool		SetElement(ObjectID CompoundID, ElementID ID, const char *ResID, EKeepRuleType Rule = EKeepLocation) = 0;
	virtual bool		SetElementSurfaceByMeshName(ObjectID CompoundID, ElementID ID, const char *MeshName, const char *ResID) = 0;
	virtual bool		SetElementSurfaceByMaterialName(ObjectID CompoundID, ElementID ID, const char *MaterialName, const char *ResID) = 0;
	virtual	bool		SetElementTransform(ObjectID CompoundID, ElementID ID, const kVector3D &Location, const kRotation &Rotation, const kVector3D &Scale) = 0; //只对自由态element有效
	virtual	bool		GetElementLocationAndRotation(ObjectID CompoundID, ElementID ID, kVector3D &Location, kRotation &Rotation) = 0; 
	virtual	bool		GetElementScale(ObjectID CompoundID, ElementID ID, kVector3D &Scale) = 0; 

	virtual kArray<ElementID> GetElements(ObjectID CompoundID) = 0;
	virtual bool		GetElementInfo(ObjectID CompoundID, ElementID ID, ObjectID &ObjID, const char *&ResID, kBox3D &InnerBounds, kBox3D &OuterBounds) = 0;
	virtual bool		ConvertToLocalLocation(ObjectID CompoundID, ElementID ID, const kVector3D &NormalizeLocation, kVector3D &OutLocalLocation, bool bInner) = 0;
	virtual bool		ConvertToNormalizeLocation(ObjectID CompoundID, ElementID ID, const kVector3D &LocalLocation, kVector3D &OutNormalizeLocation, bool bInner) = 0;

	virtual PinID		CreatePin(ObjectID CompoundID, ElementID ID, const kVector3D &UniformOffset, bool bInner) = 0;
	virtual bool		DeletePin(ObjectID CompoundID, PinID ID) = 0; //只对自由态pin有效
	virtual void		SetPinLocation(ObjectID CompoundID, PinID ID, const kVector3D &UniformLocation) = 0; //只对自由态pin有效
	virtual kArray<PinID>	GetPins(ObjectID CompoundID, ElementID ID) = 0;
	virtual bool		GetPinInfo(ObjectID CompoundID, PinID ID, kVector3D &OutUniformLocation, kVector3D &OutLocation) = 0;
	virtual bool		GetConnectPins(ObjectID CompoundID, ElementID ID0, ElementID ID1, kArray<PinID> &OutPins0, kArray<PinID> &OutPins1) = 0;
	virtual bool		CanConnect(ObjectID CompoundID, const FElementInterface &FixedElementInterface, const FElementInterface &SubElementInterface) = 0;
	virtual bool		Connect(ObjectID CompoundID, const FElementInterface &FixedElementInterface, const FElementInterface &SubElementInterface) = 0;
	virtual kVector3D   GetOffset(const kBox3D &Bounds,	ETAlign AlignType) = 0;
	
	virtual	bool		IsConnected(ObjectID CompoundID, ElementID ID) = 0;
	virtual void		UnConnect(ObjectID CompoundID, ElementID ID, bool bKeepTransform) = 0;
	virtual int			GetChildIndex(ObjectID CompoundID, ElementID ID)  = 0;
};

class ITriangulatedMesh
{
public:
	virtual ObjectID CreatePlane(const kArray<kPoint> &Polygon, float Thickness, ETAlign Type) = 0;
	virtual void AddHole(ObjectID MeshID, const kArray<kPoint> &Hole) = 0;
	virtual void SetSurface(ObjectID MeshID, ESurfaceType SurfaceType, const char *Uri) = 0;
};


