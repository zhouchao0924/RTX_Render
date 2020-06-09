
#include "WallSectionBuilder.h"
#include "../DataNodes/NodeTypes.h"
#include "CGALWrapper/PolygonAlg.h"

FWallSectionBuilder::FWallSectionBuilder(const FWallNode &InNode) : DataNode(InNode)
{
}

void FWallSectionBuilder::Build2dSection()
{
	FUVTextureMap TextureUVMap;
	TextureUVMap.OffSet = FVector2D(0, 0);
	TextureUVMap.MatSize = FVector2D(DEF_WALL_MAT_SIZE);
	TextureUVMap.MatRotation = 0.0f;
	float TopHeight = DataNode.Height + PLANE_INCREASING_HEIGHT;
	BuildSexangleSections(Sections2d, TopHeight, TextureUVMap);
}

void FWallSectionBuilder::Build3dSection()
{
	// 侧面、正面、后面UV设置
	BuildFrontFace();

	BuildBackFace();

	BuildSideFaces();

	BuildTopBottomFaces();
}

void FWallSectionBuilder::BuildFrontFace()
{
	FUVTextureMap TextureUVMap;
	TextureUVMap.OffSet = FVector2D(0,0);
	TextureUVMap.MatSize = FVector2D(100,100);
	TextureUVMap.MatRotation = 0;
	TextureUVMap.UDir = FVector(-1, 0, 0);  // ue4 left hand
	TextureUVMap.VDir = FVector(0, -1, 0);
	TextureUVMap.TextureBase = FVector2D(0, 0);
	TextureUVMap.TextureBaseInScene = FVector(DataNode.LocalEndRight.X, DataNode.Height, 0);

	FPoint2DArray Polygon;
	Polygon.Add(FVector2D(DataNode.LocalStartRight.X, 0));
	Polygon.Add(FVector2D(DataNode.LocalStartRight.X, DataNode.Height));
	Polygon.Add(FVector2D(DataNode.LocalEndRight.X, DataNode.Height));
	Polygon.Add(FVector2D(DataNode.LocalEndRight.X, 0));

	FMeshSectionDef Section3d;
	TArray<FPoint2DArray> HoleList;
	FPoint2DArray TrianglePnts;
	FPolygonAlg::PolyTriagulation(TrianglePnts, Polygon, HoleList, true);
	int32 TriangleCnt = TrianglePnts.Num() / 3;
	for (int32 TriangleIdx = 0; TriangleIdx < TriangleCnt; ++TriangleIdx)
	{
		int PntIdx = 3 * TriangleIdx;
		FVector V0(TrianglePnts[PntIdx], 0);
		FVector V1(TrianglePnts[PntIdx + 1], 0);
		FVector V2(TrianglePnts[PntIdx + 2], 0);

		auto V0UV = TextureUVMap.MapScene2TexturePos(V0);
		auto V1UV = TextureUVMap.MapScene2TexturePos(V1);
		auto V2UV = TextureUVMap.MapScene2TexturePos(V2);

		FVector Ver0 = FVector(V0.X, -DataNode.RightThick, V0.Y);
		FVector Ver1 = FVector(V1.X, -DataNode.RightThick, V1.Y);
		FVector Ver2 = FVector(V2.X, -DataNode.RightThick, V2.Y);
		FVector V0V1 = Ver1 - Ver0;
		FVector V0V2 = Ver2 - Ver0;
		FVector FaceNormal = FVector::CrossProduct(V0V2, V0V1);
		if (FVector::DotProduct(FaceNormal, FVector::RightVector) > 0)
		{
			continue;
		}

		Section3d.Vertices.Add(FVector(V0.X, -DataNode.RightThick, V0.Y));
		Section3d.Vertices.Add(FVector(V1.X, -DataNode.RightThick, V1.Y));
		Section3d.Vertices.Add(FVector(V2.X, -DataNode.RightThick, V2.Y));

		Section3d.UVs.Add(V0UV);
		Section3d.UVs.Add(V1UV);
		Section3d.UVs.Add(V2UV);
	}

	Section3d.BuildTriangleMesh();

	Sections3d.Add(Section3d);
}

void FWallSectionBuilder::BuildBackFace()
{
	// Build back face
	FUVTextureMap TextureUVMap;
	TextureUVMap.OffSet = FVector2D(0, 0);
	TextureUVMap.MatSize = FVector2D(100, 100);
	TextureUVMap.MatRotation = 0;
	TextureUVMap.UDir = FVector(1, 0, 0);  // ue4 left hand
	TextureUVMap.VDir = FVector(0, -1, 0);
	TextureUVMap.TextureBase = FVector2D(0, 0);
	TextureUVMap.TextureBaseInScene = FVector(DataNode.LocalStartLeft.X, DataNode.Height, 0);

	FPoint2DArray Polygon;
	Polygon.Add(FVector2D(DataNode.LocalStartLeft.X, 0));
	Polygon.Add(FVector2D(DataNode.LocalStartLeft.X, DataNode.Height));
	Polygon.Add(FVector2D(DataNode.LocalEndLeft.X, DataNode.Height));
	Polygon.Add(FVector2D(DataNode.LocalEndLeft.X, 0));

	FMeshSectionDef Section3d;
	TArray<FPoint2DArray> HoleList;
	FPoint2DArray TrianglePnts;
	FPolygonAlg::PolyTriagulation(TrianglePnts, Polygon, HoleList, true);
	int32 TriangleCnt = TrianglePnts.Num() / 3;
	for (int32 TriangleIdx = 0; TriangleIdx < TriangleCnt; ++TriangleIdx)
	{
		int PntIdx = 3 * TriangleIdx;
		FVector V0(TrianglePnts[PntIdx], 0);
		FVector V1(TrianglePnts[PntIdx + 1], 0);
		FVector V2(TrianglePnts[PntIdx + 2], 0);

		auto V0UV = TextureUVMap.MapScene2TexturePos(V0);
		auto V1UV = TextureUVMap.MapScene2TexturePos(V1);
		auto V2UV = TextureUVMap.MapScene2TexturePos(V2);

		FVector Ver0 = FVector(V0.X, DataNode.LeftThick, V0.Y);
		FVector Ver1 = FVector(V1.X, DataNode.LeftThick, V1.Y);
		FVector Ver2 = FVector(V2.X, DataNode.LeftThick, V2.Y);
		FVector V0V1 = Ver1 - Ver0;
		FVector V0V2 = Ver2 - Ver0;
		FVector FaceNormal = FVector::CrossProduct(V0V2, V0V1);
		if (FVector::DotProduct(FaceNormal, FVector::RightVector) < 0)
		{
			continue;
		}

		Section3d.Vertices.Add(FVector(V0.X, DataNode.LeftThick, V0.Y));
		Section3d.Vertices.Add(FVector(V1.X, DataNode.LeftThick, V1.Y));
		Section3d.Vertices.Add(FVector(V2.X, DataNode.LeftThick, V2.Y));

		Section3d.UVs.Add(V0UV);
		Section3d.UVs.Add(V1UV);
		Section3d.UVs.Add(V2UV);
	}

	Section3d.BuildTriangleMesh();

	Sections3d.Add(Section3d);
}

void FWallSectionBuilder::BuildSideFaces()
{
	FUVTextureMap TextureUVMap;
	TextureUVMap.OffSet = FVector2D(0, 0);
	TextureUVMap.MatSize = FVector2D(100, 100);
	TextureUVMap.MatRotation = 0;
	TextureUVMap.VDir = FVector(0, 0, -1);

	float WallTopHeight = DataNode.Height;

	// left
	FMeshSectionDef LeftSideSection;
	FVector2D Start2StartRight2D = DataNode.LocalStartRight - DataNode.LocalStart;
	FVector Start2StartRight(Start2StartRight2D.X, Start2StartRight2D.Y, 0);

	TextureUVMap.TextureBase = FVector2D(0, 0);
	TextureUVMap.TextureBaseInScene = FVector(DataNode.LocalStartRight.X, DataNode.LocalStartRight.Y, WallTopHeight);
	TextureUVMap.UDir = -Start2StartRight;  // ue4 left hand

	BuildSectionByTextureMap(LeftSideSection, TextureUVMap,
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, 0),
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, DataNode.Height),
		FVector(DataNode.LocalStartRight.X, DataNode.LocalStartRight.Y, 0));
	
	BuildSectionByTextureMap(LeftSideSection, TextureUVMap,
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, DataNode.Height),
		FVector(DataNode.LocalStartRight.X, DataNode.LocalStartRight.Y, DataNode.Height),
		FVector(DataNode.LocalStartRight.X, DataNode.LocalStartRight.Y, 0));

	FVector StartTextureBaseInScene(DataNode.LocalStart.X, DataNode.LocalStart.Y, WallTopHeight);
	TextureUVMap.TextureBase = TextureUVMap.MapScene2TexturePos(StartTextureBaseInScene);
	TextureUVMap.TextureBaseInScene = StartTextureBaseInScene;
	FVector2D StartLeft2Start2D = DataNode.LocalStart - DataNode.LocalStartLeft;
	FVector StartLeft2Start(StartLeft2Start2D.X, StartLeft2Start2D.Y, 0);
	TextureUVMap.UDir = -StartLeft2Start;  // ue4 left hand

	BuildSectionByTextureMap(LeftSideSection, TextureUVMap,
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, 0),
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, DataNode.Height),
		FVector(DataNode.LocalStartLeft.X, DataNode.LocalStartLeft.Y, 0));

	BuildSectionByTextureMap(LeftSideSection, TextureUVMap,
		FVector(DataNode.LocalStart.X, DataNode.LocalStart.Y, DataNode.Height),
		FVector(DataNode.LocalStartLeft.X, DataNode.LocalStartLeft.Y, DataNode.Height),
		FVector(DataNode.LocalStartLeft.X, DataNode.LocalStartLeft.Y, 0));

	LeftSideSection.BuildTriangleMesh();
	Sections3d.Add(LeftSideSection);

	// right
	FMeshSectionDef RightSideSection;

	TextureUVMap.TextureBase = FVector2D(0, 0);
	TextureUVMap.TextureBaseInScene = FVector(DataNode.LocalEndLeft.X, DataNode.LocalEndLeft.Y, WallTopHeight);
	FVector2D End2EndLeft2D = DataNode.LocalEndLeft - DataNode.LocalEnd;
	FVector End2EndLeft(End2EndLeft2D.X, End2EndLeft2D.Y, 0);
	TextureUVMap.UDir = -End2EndLeft;

	BuildSectionByTextureMap(RightSideSection, TextureUVMap,
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, 0), 
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, DataNode.Height),
		FVector(DataNode.LocalEndLeft.X, DataNode.LocalEndLeft.Y, 0));

	BuildSectionByTextureMap(RightSideSection, TextureUVMap,
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, DataNode.Height),
		FVector(DataNode.LocalEndLeft.X, DataNode.LocalEndLeft.Y, DataNode.Height), 
		FVector(DataNode.LocalEndLeft.X, DataNode.LocalEndLeft.Y, 0));

	FVector EndTextureBaseInScene(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, WallTopHeight);
	TextureUVMap.TextureBase = TextureUVMap.MapScene2TexturePos(EndTextureBaseInScene);
	TextureUVMap.TextureBaseInScene = EndTextureBaseInScene;
	FVector2D EndRight2End2D = DataNode.LocalEnd - DataNode.LocalEndRight;
	FVector EndRight2End(EndRight2End2D.X, EndRight2End2D.Y, 0);
	TextureUVMap.UDir = -EndRight2End;

	BuildSectionByTextureMap(RightSideSection, TextureUVMap,
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, 0), 
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, DataNode.Height),
		FVector(DataNode.LocalEndRight.X, DataNode.LocalEndRight.Y, 0));

	BuildSectionByTextureMap(RightSideSection, TextureUVMap,
		FVector(DataNode.LocalEnd.X, DataNode.LocalEnd.Y, DataNode.Height),
		FVector(DataNode.LocalEndRight.X, DataNode.LocalEndRight.Y, DataNode.Height), 
		FVector(DataNode.LocalEndRight.X, DataNode.LocalEndRight.Y, 0));

	RightSideSection.BuildTriangleMesh();
	Sections3d.Add(RightSideSection);
}

void FWallSectionBuilder::BuildTopBottomFaces()
{
	FUVTextureMap TextureUVMap;
	TextureUVMap.OffSet = FVector2D(0, 0);
	TextureUVMap.MatSize = FVector2D(100, 100);
	TextureUVMap.MatRotation = 0;
	BuildSexangleSections(Sections3d, DataNode.Height, TextureUVMap);

	BuildSexangleSections(Sections3d, 0.0f, TextureUVMap);
}

void FWallSectionBuilder::BuildSexangleSections(TArray<FMeshSectionDef> &OutSections,
	float TopHeight, FUVTextureMap& InTextureMap)
{
	FPoint2DArray Polygon;
	Polygon.Add(FVector2D(DataNode.LocalStartLeft));
	Polygon.Add(FVector2D(DataNode.LocalStart));
	Polygon.Add(FVector2D(DataNode.LocalStartRight));
	Polygon.Add(FVector2D(DataNode.LocalEndRight));
	Polygon.Add(FVector2D(DataNode.LocalEnd));
	Polygon.Add(FVector2D(DataNode.LocalEndLeft));

	TArray<FPoint2DArray> HoleList;
	FPoint2DArray TrianglePnts;
	FPolygonAlg::PolyTriagulation(TrianglePnts, Polygon, HoleList, true);

	InTextureMap.UDir = FVector(1, 0, 0);  // ue4 left hand
	InTextureMap.VDir = FVector(0, 1, 0);
	InTextureMap.TextureBase = FVector2D(0, 0);
	InTextureMap.TextureBaseInScene = FVector(DataNode.LocalStart, 0);

	FMeshSectionDef OutSection;
	int32 TriangleCnt = TrianglePnts.Num() / 3;
	for (int32 TriangleIdx = 0; TriangleIdx < TriangleCnt; ++TriangleIdx)
	{
		int PntIdx = 3 * TriangleIdx;
		FVector V0(TrianglePnts[PntIdx], 0);
		FVector V1(TrianglePnts[PntIdx + 1], 0);
		FVector V2(TrianglePnts[PntIdx + 2], 0);

		FVector2D V0UV = InTextureMap.MapScene2TexturePos(V0);
		FVector2D V1UV = InTextureMap.MapScene2TexturePos(V1);
		FVector2D V2UV = InTextureMap.MapScene2TexturePos(V2);

		V0.Z = V1.Z = V2.Z = TopHeight;
		OutSection.Vertices.Add(V0);
		OutSection.Vertices.Add(V1);
		OutSection.Vertices.Add(V2);

		OutSection.UVs.Add(V0UV);
		OutSection.UVs.Add(V1UV);
		OutSection.UVs.Add(V2UV);
	}

	OutSection.BuildTriangleMesh();
	OutSections.Add(OutSection);
}

void FWallSectionBuilder::BuildSectionByTextureMap(FMeshSectionDef &OutMeshSection, FUVTextureMap &InTextMap,
	const FVector &V0, const FVector &V1, const FVector &V2)
{
	auto V0UV = InTextMap.MapScene2TexturePos(V0);
	auto V1UV = InTextMap.MapScene2TexturePos(V1);
	auto V2UV = InTextMap.MapScene2TexturePos(V2);
	OutMeshSection.Vertices.Add(V0);
	OutMeshSection.Vertices.Add(V1);
	OutMeshSection.Vertices.Add(V2);
	OutMeshSection.UVs.Add(V0UV);
	OutMeshSection.UVs.Add(V1UV);
	OutMeshSection.UVs.Add(V2UV);

	OutMeshSection.Vertices.Add(V0);
	OutMeshSection.Vertices.Add(V2);
	OutMeshSection.Vertices.Add(V1);
	OutMeshSection.UVs.Add(V0UV);
	OutMeshSection.UVs.Add(V2UV);
	OutMeshSection.UVs.Add(V1UV);
}
