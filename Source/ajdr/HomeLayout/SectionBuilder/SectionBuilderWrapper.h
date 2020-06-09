#pragma once

#include "../DataNodes/WallNode.h"
#include "../DataNodes/MeshSectionDef.h"
#include "CoreMinimal.h"

class FSectionBuilderWrapper
{
public:
	static void BuildWallSections(TArray<FMeshSectionDef>& OutWallSections, 
		const FWallNode &InNode, bool bIn3d = false);
};