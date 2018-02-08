// Fill out your copyright notice in the Description page of Project Settings.

#include "SFSItemBase.h"




void USFSItemBase::InitializeComplexProperties()
{
	// Stuff will go here.
}

int32 USFSItemBase::GetMaxStackSize()
{
	return StackCapacity;
}

int32 USFSItemBase::GetPropertyValue(EItemPropertyType ItemProperty)
{
	FItemProperties* PropertyQuery = ItemProperties.FindByPredicate([ItemProperty](const FItemProperties& ItemPropertyStruct)
	{return ItemPropertyStruct.Property == ItemProperty; });
	
	if (PropertyQuery)
	{
		return PropertyQuery->PropertyValue;
	}

	return -1;
}
