// Fill out your copyright notice in the Description page of Project Settings.

#include "Tiles.h"
#include "TileObject.h"
#include "WorldMap.h"

FVector TTile::GetTileLocationAsVector()
{
	return FVector(
		GameEncapsulator::GetGame()->Map->CellSize * location->x,
		GameEncapsulator::GetGame()->Map->CellSize * location->y,
		GameEncapsulator::GetGame()->Map->CellSize * GameEncapsulator::GetGame()->Map->CliffHeight * Height
	);
}

void TTile::RemoveObject()
{
	//Remove an object if there is on
	//Call it's destroy method
	if (object != nullptr)
	{
		FTileObject* tmp = object;
		object = nullptr;
		tmp->Destroy(this, GameEncapsulator::GetGame()->Map->FindChunkWithTile(this));
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