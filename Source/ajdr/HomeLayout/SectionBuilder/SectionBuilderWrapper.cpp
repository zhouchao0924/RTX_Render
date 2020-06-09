
#include "SectionBuilderWrapper.h"
#include "WallSectionBuilder.h"

void FSectionBuilderWrapper::BuildWallSections(TArray<FMeshSectionDef>& OutWallSections,
	const FWallNode &InNode, bool bIn3d)
{
	FWallSectionBuilder WallSectionBuilder(InNode);

	if (bIn3d)
	{
		WallSectionBuilder.Build3dSection();
		WallSectionBuilder.Fetch3dSections(OutWallSections);
	}
	else
	{
		WallSectionBuilder.Build2dSection();
		WallSectionBuilder.Fetch2dSections(OutWallSections);
	}
}
