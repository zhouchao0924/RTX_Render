// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRVirtualWallAdapter.h"
#include "ISuite.h"

UDRVirtualWallAdapter::UDRVirtualWallAdapter(const FObjectInitializer& ObjectIntializer)
	:Super(ObjectIntializer)
{
}

int32 UDRVirtualWallAdapter::GetAssociationID()
{
	return Obj->GetInt("AssociationID");
}

/********************************Set Function*************************************/

void UDRVirtualWallAdapter::SetAssociationID(const int32 InAssociationID)
{
	Obj->SetInt("AssociationID", InAssociationID);
}