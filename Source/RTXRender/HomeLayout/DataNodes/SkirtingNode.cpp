

#include "SkirtingNode.h"
#include "CGALWrapper/LinearEntityAlg.h"

void FSkirtingNode::CopyAttrToSegNode()
{
	for (FSkirtingSegNode &SkirtingSegNode : SkirtingSegments)
	{
		SkirtingSegNode.SkirtingMeshID = SkirtingMeshID;
		SkirtingSegNode.MaterialID = MaterialID;
		SkirtingSegNode.HeadingDir = HeadingDir;
		SkirtingSegNode.SkirtingType = SkirtingType;
		SkirtingSegNode.SkirtingMeshModelID = SkirtingMeshModelID;
		SkirtingSegNode.MaterialModelID = MaterialModelID;
		SkirtingSegNode.RoomClassID = RoomClassID;
		SkirtingSegNode.SKUID = SKUID;
		SkirtingSegNode.CraftId = CraftId;
	}
}

bool FSkirtingNode::IsClosedSkirting() const
{
	if (SkirtingSegments.Num() < 3)
	{
		return false;
	}

	return SkirtingSegments[0].SkirtingStartCorner == SkirtingSegments.Top().SkirtingEndCorner;
}

void FSkirtingNode::InitFromAreaPath(const TArray<FVector2D> &AreaPath, ESkirtingType InSkirtingType, bool isClosure)
{
	SkirtingType = InSkirtingType;

	int32 AreaPathSegCnt = AreaPath.Num();
	if (isClosure)
	{
		AutoHeadingDirByClosedPath(AreaPath);
		for (int32 i = 0; i < AreaPathSegCnt; ++i)
		{
			FSkirtingSegNode SkirtingSegNode;
			SkirtingSegNode.SkirtingStartCorner.Position = AreaPath[i];
			int32 NextIndex = (i + 1) % AreaPathSegCnt;
			if (0 == NextIndex)
			{
				SkirtingSegNode.SkirtingEndCorner = SkirtingSegments[0].SkirtingStartCorner;
			}
			else
			{
				SkirtingSegNode.SkirtingEndCorner.Position = AreaPath[NextIndex];
			}
			SkirtingSegments.Add(SkirtingSegNode);
		}
	}
	else
	{
		AutoHeadingDirByClosedPath(AreaPath);
		for (int32 i = 0; i < (AreaPathSegCnt-1); ++i)
		{
			FSkirtingSegNode SkirtingSegNode;
			SkirtingSegNode.SkirtingStartCorner.Position = AreaPath[i];
			SkirtingSegNode.SkirtingEndCorner.Position = AreaPath[i + 1];
			SkirtingSegments.Add(SkirtingSegNode);
		}
	}
}

void FSkirtingNode::AutoHeadingDirByClosedPath(const TArray<FVector2D> &ClosedPath)
{
	// ue4 left hand
	switch (ULinearEntityLibrary::GetPolygonOrientation(ClosedPath))
	{
	case ECgPolyOrientation::ECLOCKWISE:
	{
		HeadingDir = ECgHeadingDirection::ELeftTurn;
	}
	break;
	case ECgPolyOrientation::ECOUNTERCLOCKWISE:
	{
		HeadingDir = ECgHeadingDirection::ERightTurn;
	}
	break;
	default:
		break;
	}
}


