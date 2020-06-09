
#include "CabinetSetApi.h"
//////////////////////////////////////////////////////////////////////////
FAutoSetBase* NewAutoSet(eAutoSetType eType)
{
	FAutoSetBase* pSet = nullptr;
	switch (eType)
	{
	case eAutoSetFloor:
	{
		pSet = &AutoSetCookFloor::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetHang:
	{
		pSet = &AutoSetCookHang::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetToilet:
	{
		pSet = &AutoSetToilet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetToiletHang:
	{
		pSet = &AutoSetToiletHang::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetTatami:
	{
		pSet = &AutoSetTatami::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetWardrobe:
	{
		pSet = &AutoSetWardrobe::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetWardorbeHang:
	{
		pSet = &AutoSetWardrobeHang::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetSideboardCabinet:
	{
		pSet = &AutoSetSideboard::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetTelevision:
	{
		pSet = &AutoSetTelevision::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetBookcase:
	{
		pSet = &AutoSetBookcase::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetPorchArk:
	{
		pSet = &AutoSetPorchArk::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetWashCabinet:
	{
		pSet = &AutoSetWashCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetStoreCabinet:
	{
		pSet = &AutoSetStoreCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetBedCabinet:
	{
		pSet = &AutoSetBedCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetDesk:
	{
		pSet = &AutoSetDesk::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetHangBedCabinet:
	{
		pSet = &AutoSetHangBedCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetCoatroom:
	{
		pSet = &AutoSetCoatroom::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetCoatroomHang:
	{
		pSet = &AutoSetCoatroomHang::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetBayWindowCabinet:
	{
		pSet = &AutoSetBayWindowCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetTakeInCabinet:
	{
		pSet = &AutoSetTakeInCabinet::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetLaminate:
	{
		pSet = &AutoSetLaminate::FAutoSetCabinet::Instance();
	}
	break;
	case eAutoSetBedside:
	{
		pSet = &AutoSetBedside::FAutoSetCabinet::Instance();
	}
	break;
	}
	CHECK_ERROR(pSet);
	pSet->mSetType = eType;
	return pSet;
}