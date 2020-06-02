#ifndef PATTERN_OBJECT_INTERFACE_H
#define PATTERN_OBJECT_INTERFACE_H

#include <list>
#include <memory>

#include "IShapeObject.h"
#include "IBuildingPlugin.h"

#define PATTERN_OBJECT_TYPE (PLUGIN_SECTION_1 + 1)

class IPatternObject
{
public:
	virtual int GetShapeCount() = 0;
	virtual std::shared_ptr<IShapeObject> GetShape(SHAPE_ID ShapeID) = 0;

	virtual IObject* GetRawObject() = 0;

	virtual const char* GetResId() = 0;

	virtual int GetLocalVersion() = 0;

	virtual int GetModifyVersion() = 0;

	virtual void AddLocalVersion(int Number) = 0;

	virtual bool IsModified() = 0;

	virtual void SetModifyFlag() = 0;

	virtual void ResetModifyFlag() = 0;

	virtual kBox3D GetBounds() = 0;

	virtual void GetAllShapeMeshData(std::shared_ptr<TArrayTemplate<FMeshSectionData>>& OutMeshData, std::shared_ptr<TArrayTemplate<SHAPE_ID>>& OutShapeIds) = 0;
	virtual void GetIntervalMeshData(std::shared_ptr<FMeshSectionData>& OutMeshData) = 0;

	virtual bool GetShapePreviewColor(SHAPE_ID ShapeId, kColor& PreviewColor) = 0;
	virtual bool SetShapePreviewColor(SHAPE_ID ShapeId, const kColor& PreviewColor) = 0;

	virtual const char* GetJointExpression(bool bIsHorizontal) = 0;
	virtual bool SetJointExpression(const char* Expression, bool bIsHorizontal) = 0;

	virtual bool SetShapeOffsetExpression(SHAPE_ID ShapeId, const char* Expression, bool bIsOffsetX) = 0;

	virtual bool CheckAliasIsValid(const char* Alias) = 0;

	virtual SHAPE_ID AddShape(std::shared_ptr<ShapeOptions> ShapeOptions) = 0;
	virtual bool SetShapeOptions(SHAPE_ID ShapeId, std::shared_ptr<ShapeOptions> ShapeOptions) = 0;
	virtual void RemoveShape(SHAPE_ID ShapeId) = 0;
	virtual bool SetShapeAngle(SHAPE_ID ShapeId, float Angle) = 0;

	virtual const char* GetMaterialResId(MAT_GROUP_ID GroupId, unsigned int MaterialIndex) = 0;

	virtual unsigned int GetMaterialGroupCount() = 0;
	virtual unsigned int GetMaterialCountInGroup(MAT_GROUP_ID GroupID) = 0;

	virtual MAT_GROUP_ID CreateMaterialGroup() = 0;
	virtual bool AddMaterialToGroup(MAT_GROUP_ID GroupId, const char* ResId) = 0;
	virtual bool RemoveMaterialFromGroup(MAT_GROUP_ID GroupId, const char* ResId) = 0;
	virtual bool ClearShapeMaterialGroup(SHAPE_ID ShapeId) = 0;

	virtual bool SavePattern(const char* Path, const char* FileName) = 0;
};

enum class EPatternVersion
{
	PV_Initializer = 0,
	PV_AddExpression = 1,
};

#endif
