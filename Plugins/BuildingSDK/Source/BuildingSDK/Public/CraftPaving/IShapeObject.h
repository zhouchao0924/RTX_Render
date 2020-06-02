#ifndef SHAPE_OBJECT_INTERFACE_H
#define SHAPE_OBJECT_INTERFACE_H

#include <memory>

#include "Math/kVector3D.h"
#include "Math/kVector2D.h"
#include "TArray.h"
#include "ISerialize.h"

#include "ShapeOptions.h"

typedef unsigned int SHAPE_ID;
typedef unsigned int MAT_GROUP_ID;

struct FMeshSectionData
{
	TArrayTemplate<kVector3D>	Vertices;
	TArrayTemplate<int>			Triangles;
	TArrayTemplate<kVector3D>	Normals;
	TArrayTemplate<kTexUV>		UV0;
	TArrayTemplate<kVector3D>	Tangents;
};

class IShapeObject
{
public:
	virtual EShapeType GetShapeType() const = 0;
	virtual bool HasMaterialGroup() = 0;
	virtual bool IsValidShape() = 0;
	virtual kBox3D GetBounds() = 0;
	virtual SHAPE_ID GetShapeId() = 0;
	virtual MAT_GROUP_ID GetMaterialGroupId() = 0;
	virtual std::shared_ptr<ShapeOptions> GetShapeOptions() = 0;

	virtual kColor GetPreviewColor() = 0;

	virtual kXform GetGlobalTransform() = 0;

	virtual const char* GetAliasByAttribute(EAttributeType AttributeType) = 0;
	virtual bool GetAttributeTypeByAlias(const char* Alias, EAttributeType& AttributeType) = 0;
	virtual void GetExistAliasAttribute(std::shared_ptr<TArrayTemplate<EAttributeType>>& AttributeTypes) = 0;
	virtual const char* GetOffsetExpression(bool bIsOffsetX) = 0;
	virtual void GetValueByAttributeType(EAttributeType AttributeType, float& Value) = 0;

	virtual void SetAttributeAlias(EAttributeType AttributeType, const char* Alias) = 0;
	virtual void RemoveAttributeAlias(EAttributeType AttributeType) = 0;
	virtual bool SetOffsetExpreesion(const char* Expression, bool bIsOffsetX) = 0;

	virtual bool GetMeshData(FMeshSectionData& OutMeshData) = 0;

	virtual bool GenerateMeshData(std::shared_ptr<ShapeOptions> Options) = 0;
	virtual std::shared_ptr<FMeshSectionData> GenerateIntervalData(float IntervalSize) = 0;

	virtual void SetPreviewColor(const kColor& Color) = 0;
	virtual void SetGlobalLocation(const kVector3D& Location) = 0;
	virtual void SetGlobalRotation(const kRotation& Rotation) = 0;
	virtual void SetMaterialGroupId(MAT_GROUP_ID GroupId) = 0;

	virtual void CleanShape() = 0;
};

#endif
