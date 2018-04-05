// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldMap.h"
#include "WorldChunk.h"
#include "Tiles.h"
#include "TileObject.h"

WorldMap::WorldMap()
{
}

WorldMap::~WorldMap()
{
	for (int x = 0; x < MapSize; x++)
	{
		delete[] Tiles[x];
	}
	delete[] Tiles;
}

TTile* WorldMap::GetTile(int x, int y)
{
	if (x >= MapSize || y >= MapSize || x < 0 || y < 0)
		return nullptr;

	return Tiles[x][y];
}

TTile* WorldMap::GetTileAtLocation(FVector Loc)
{
	int x = (FMath::RoundToInt(Loc.X)) / CellSize;
	int y = (FMath::RoundToInt(Loc.Y)) / CellSize;

	if (x < 0 || x >= MapSize || y < 0 || y >= MapSize)
		return nullptr;

	return Tiles[x][y];
}

bool WorldMap::IsTileInChunk(TTile* Tile)
{
	int HeightChunkNum = HeightChunks.Num();
	for (int x = 0; x < HeightChunkNum; x++)
	{
		if (HeightChunks[x].Contains(Tile))
			return true;
	}
	return false;
}

AWorldChunk* WorldMap::FindChunkWithTile(TTile* Tile)
{
	return Chunks[(Tile->location->x / ChunkSize) * (MapSize / ChunkSize) + (Tile->location->y / ChunkSize)];
}

void WorldMap::MakeTileRoad(TTile* CurrentTile)
{
	//Make sure tile is valid
	if (CurrentTile == nullptr)
		return;

	//Make sure the tile is a ground tile
	if (CurrentTile->TileType != ETileType::Ground)
		return;

	//Make sure the tile object isn't already a road object
	if (CurrentTile->object != nullptr && CurrentTile->object->GetName().Equals("Road"))
		return;

	//Add this tile to the path
	PathHandler.AddTile(CurrentTile);

	//Find the chunk in the tile
	AWorldChunk* Chunk = FindChunkWithTile(CurrentTile);

	//Remove the current object if it exists
	CurrentTile->RemoveObject();

	//Create a road object and make it affect this tile
	CurrentTile->object = new FTileObject_Road();
	CurrentTile->object->AffectTile(CurrentTile, Chunk);

	TTile* Tile;

	////Reaffect the adjacent tiles if it has a road object////
	 
	//Get the right tile
	Tile = GetTile(CurrentTile->location->x + 1, CurrentTile->location->y);
	if (Tile != nullptr && Tile->object != nullptr)
	{
		if (Tile->object->GetName().Equals("Road"))
		{
			Tile->object->AffectTile(Tile, FindChunkWithTile(Tile));
		}
	}

	//Get the left tile
	Tile = GetTile(CurrentTile->location->x - 1, CurrentTile->location->y);
	if (Tile != nullptr && Tile->object != nullptr)
	{
		if (Tile->object->GetName().Equals("Road"))
		{
			Tile->object->AffectTile(Tile, FindChunkWithTile(Tile));
		}
	}

	//Get the north tile
	Tile = GetTile(CurrentTile->location->x, CurrentTile->location->y + 1);
	if (Tile != nullptr && Tile->object != nullptr)
	{
		if (Tile->object->GetName().Equals("Road"))
		{
			Tile->object->AffectTile(Tile, FindChunkWithTile(Tile));
		}
	}

	//Get the south tile
	Tile = GetTile(CurrentTile->location->x, CurrentTile->location->y - 1);
	if (Tile != nullptr && Tile->object != nullptr)
	{
		if (Tile->object->GetName().Equals("Road"))
		{
			Tile->object->AffectTile(Tile, FindChunkWithTile(Tile));
		}
	}
}