

#include "Window.h"
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "ModelInstance.h"

BEGIN_DERIVED_CLASS(Window, WallHole)
END_CLASS()

Window::Window()
{
}

void Window::UpdateTransform(ModelInstance *pModel)
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(CornerID, Corner);
	if (pCorner)
	{
		pModel->Location = kVector3D(pCorner->Location.X, pCorner->Location.Y, ZPos);
	}

	Wall *pWall = SUITE_GET_BUILDING_OBJ(WallID, Wall);
	if (pWall)
	{
		pModel->Forward = pWall->GetForward();
	}

	pModel->AlignType = EAlignBottomCenter;
}

