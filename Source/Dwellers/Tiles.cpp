// Fill out your copyright notice in the Description page of Project Settings.

#include "Tiles.h"
#include "TileObject.h"
#include "WorldMap.h"

FVector TTile::GetTileLocationAsVector()
{
	return FVector(GameEncapsulator::GetGame()->map->cellsize * location->x, GameEncapsulator::GetGame()->map->cellsize * location->y, GameEncapsulator::GetGame()->map->cellsize * GameEncapsulator::GetGame()->map->cliffheight * height);
}

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