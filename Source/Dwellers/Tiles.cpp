// Fill out your copyright notice in the Description page of Project Settings.

#include "Tiles.h"
#include "TileObject.h"
#include "WorldMap.h"

void TTile::RemoveObject()
{
	if (object != nullptr)
	{
		FTileObject* tmp = object;
		object = nullptr;
		tmp->Destroy(this, GameEncapsulator::GetGame()->map->FindChunkWithTile(this));
		delete tmp;
	}

}

float TGroundTile::GetCost()
{
	if (object == nullptr)
		return 1;
	else
		return object->GetTraversalCost();
}