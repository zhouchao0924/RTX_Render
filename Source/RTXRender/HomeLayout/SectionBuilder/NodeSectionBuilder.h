#pragma once

#include "../DataNodes/MeshSectionDef.h"
#include "CoreMinimal.h"

// base section builder
class FNodeSectionBuilder
{
public:
	FNodeSectionBuilder();

	virtual ~FNodeSectionBuilder() = default;

	virtual void Build2dSection() = 0;

	virtual void Build3dSection() = 0;

	// you can only invoke it once
	void Fetch2dSections(TArray<FMeshSectionDef>& OutStructSections);

	void Fetch3dSections(TArray<FMeshSectionDef>& OutStructSections);

protected:
	TArray<FMeshSectionDef> Sections2d;

	TArray<FMeshSectionDef> Sections3d;
};
