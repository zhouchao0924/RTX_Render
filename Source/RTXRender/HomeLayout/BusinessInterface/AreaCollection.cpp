// Fill out your copyright notice in the Description page of Project Settings.
// material node
#pragma once


#include "AreaCollection.h"


FWallCollectionHandle& FWallCollectionHandle::operator+=(const FWallCollectionHandle &OtherCollection)
{
	if (WallMaterial == OtherCollection.WallMaterial)
	{
		WallArea += OtherCollection.WallArea;
	}

	return *this;
}

float FWallCollectionHandle::GetAreaOfWall() const
{
	return (float)WallArea;
}

FSkirtingCollectionHandle& FSkirtingCollectionHandle::operator+=(const FSkirtingCollectionHandle &OtherCollection)
{
	if (SkirtingModelID == OtherCollection.SkirtingModelID
		&& SkirtingMaterialID == OtherCollection.SkirtingMaterialID)
	{
		SkirtingLength += OtherCollection.SkirtingLength;
	}

	return *this;
}

float FSkirtingCollectionHandle::GetSkirtingLength() const
{
	return SkirtingLength;
}

const TArray<FWallCollectionHandle>& FRegionCollection::CalculateWallCollections()
{
	return WallCollections;
}

const TArray<FSkirtingCollectionHandle>& FRegionCollection::CalculateSkirtingCollections()
{
	return SkirtingCollections;
}

