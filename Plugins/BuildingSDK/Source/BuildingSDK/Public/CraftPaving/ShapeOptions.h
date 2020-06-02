#ifndef SHAPE_OPTIONS_H
#define SHAPE_OPTIONS_H

#include "Math/kXform.h"

enum class EShapeType
{
	ST_Triangle = 0,
	ST_Diamond = 1,
	ST_Rectangle = 2,
	ST_Parallelogram = 3,
	ST_Hexagon = 4,
	ST_UnKown
};

enum class EShapeOptionsVersion
{
	SOV_Initializer = 0,
	SOV_ParallelogramAngle = 1, // Add angle option for parallelogram
};

struct ShapeOptions
{
public:
	EShapeType ShapeType;
	EShapeOptionsVersion ShapeVersion;
	kXform	ShapeTransform;

	float Length;
	float Width;
	float Radius;
	float Angle;
	float CornerRadius;
	float Segments;

	ShapeOptions()
		: ShapeVersion(EShapeOptionsVersion::SOV_ParallelogramAngle)
		, ShapeTransform(kXform(kVector3D(0.0f, 0.0f, 0.0f), kRotation(0.0f, 0.0f, 0.0f), kVector3D(1.0f, 1.0f, 1.0f)))
	{ }
};

enum EAttributeType
{
	AT_Length = 0,
	AT_Width = 1,
	AT_Radius = 2,
	AT_Angle = 3,
	AT_CornerRadius = 4,
	AT_Segments = 5
};

#endif
