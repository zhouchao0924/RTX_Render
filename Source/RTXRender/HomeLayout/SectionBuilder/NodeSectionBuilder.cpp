
#include "NodeSectionBuilder.h"

FNodeSectionBuilder::FNodeSectionBuilder()
{
	
}

void FNodeSectionBuilder::Fetch2dSections(TArray<FMeshSectionDef>& OutStructSections)
{
	OutStructSections = MoveTemp(Sections2d);
}

void FNodeSectionBuilder::Fetch3dSections(TArray<FMeshSectionDef>& OutStructSections)
{
	OutStructSections = MoveTemp(Sections3d);
}
