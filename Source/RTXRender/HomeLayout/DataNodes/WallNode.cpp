

#include "WallNode.h"
#include "CoreMinimal.h"
#include "CGALWrapper/LinearEntityAlg.h"

FWallNode::FWallNode()
{
	// default wall material size
	FrontUVScale = SideUVScale = BackUVScale = DEF_WALL_MAT_SIZE;

	FrontUVAngle = SideUVAngle = BackUVAngle = 0.0f;

	// default wall size
	Height = DEF_WALL_CUBE_SIZE.Z;
	LeftThick = RightThick = DEF_WALL_CUBE_SIZE.Y / 2;

	FVector HalfWallSize = DEF_WALL_CUBE_SIZE / 2;
	StartPos = FVector2D(0, -HalfWallSize.X);

	StartLeft = FVector2D(-LeftThick, -HalfWallSize.X);

	StartRight = FVector2D(RightThick, -HalfWallSize.X);

	EndPos = FVector2D(0, HalfWallSize.X);

	EndLeft = FVector2D(-LeftThick, HalfWallSize.X);

	EndRight = FVector2D(RightThick, HalfWallSize.X);

	ZPos = 0.0f;

	ComputeWallInfo();
}

void FWallNode::GetWallCornerPos(TArray<FVector2D> &CornerPnts)
{
	FVector2D TmpStart2EndDir = EndPos - StartPos;
	FVector2D LeftDir = TmpStart2EndDir.GetRotated(90);
	LeftDir.Normalize();

	CornerPnts.Add(StartPos + LeftDir * LeftThick);
	CornerPnts.Add(StartPos - LeftDir * RightThick);
	CornerPnts.Add(EndPos - LeftDir * RightThick);
	CornerPnts.Add(EndPos + LeftDir * LeftThick);
}

void FWallNode::ComputeWallInfo()
{
	WallCenter = (StartPos + EndPos) * 0.5f;
	Start2EndDir = EndPos - StartPos;
	WallAngle = FMath::RadiansToDegrees(ULinearEntityLibrary::GetRotatedOfVector2D(Start2EndDir));
	WallLength = Start2EndDir.Size();

	LocalStart = FVector2D(-0.5 * WallLength, 0);
	LocalEnd = FVector2D(0.5 * WallLength, 0);

	FVector2D LeftDir = Start2EndDir.GetRotated(90);
	LeftDir.Normalize();
	StartLeft = StartPos + LeftDir * LeftThick;
	StartRight = StartPos - LeftDir * RightThick;
	EndLeft = EndPos + LeftDir * LeftThick;
	EndRight = EndPos - LeftDir * RightThick;

	ComputeWallLocalPos();
}

void FWallNode::ComputeWallLocalPos()
{
	LocalStartLeft = ConvertScene2LocalPnt(StartLeft);
	LocalStartRight = ConvertScene2LocalPnt(StartRight);
	LocalEndLeft = ConvertScene2LocalPnt(EndLeft);
	LocalEndRight = ConvertScene2LocalPnt(EndRight);
}

FVector2D FWallNode::ConvertScene2LocalPnt(const FVector2D& ScenePnt) const
{
	return (ScenePnt - WallCenter).GetRotated(-WallAngle); 
}

FVector2D FWallNode::ConvertLocal2ScenePnt(const FVector2D& LocalPnt) const
{
	return LocalPnt.GetRotated(WallAngle) + WallCenter;
}
