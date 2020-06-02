#pragma once

#include "../DataNodes/WallNode.h"
#include "NodeSectionBuilder.h"

// wall section builder
class FWallSectionBuilder : public FNodeSectionBuilder
{
public:
	FWallSectionBuilder(const FWallNode &InNode);

	virtual ~FWallSectionBuilder() = default;

	virtual void Build2dSection() override;

	virtual void Build3dSection() override;

protected:
	void BuildFrontFace();

	void BuildBackFace();

	void BuildSideFaces();

	void BuildTopBottomFaces();

	void BuildSexangleSections(TArray<FMeshSectionDef> &OutSections, 
		float TopHeight, FUVTextureMap& InTextureMap);

	void BuildSectionByTextureMap(FMeshSectionDef &OutMeshSection, FUVTextureMap &InTextMap,
		const FVector &V0, const FVector &V1, const FVector &V2);

protected:
	FWallNode DataNode;
};
