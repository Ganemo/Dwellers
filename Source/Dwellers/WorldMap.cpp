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
	for (int x = 0; x < mapsize; x++)
	{
		delete[] tiles[x];
	}
	delete[] tiles;
}

TTile* WorldMap::GetTile(int x, int y)
{
	if (x >= mapsize || y >= mapsize || x < 0 || y < 0)
		return nullptr;

	TTile* tilelookup = tiles[x][y];
	return tilelookup;
}

TTile* WorldMap::GetTileAtLocation(FVector loc)
{
	int x = (FMath::RoundToInt(loc.X)) / cellsize;
	int y = (FMath::RoundToInt(loc.Y)) / cellsize;

	if (x < 0 || x >= mapsize || y < 0 || y >= mapsize)
		return nullptr;

	return tiles[x][y];
}

bool WorldMap::IsTileInChunk(TTile* tile)
{
	int num = heightchunks.Num();
	for (int x = 0; x < num; x++)
	{
		if (heightchunks[x].Contains(tile))
			return true;
	}
	return false;
}

AWorldChunk* WorldMap::FindChunkWithTile(TTile* tile)
{
	return chunks[(tile->location->x / chunksize) * (mapsize / chunksize) + (tile->location->y / chunksize)];
}

int WorldMap::GetPointFromReference(int xpos, int ypos, int xdiff, int ydiff)
{
	return ((chunksize * 2 + 2) + ((chunksize * 2 + 1) * xpos * 2) + (2 * ypos)) + xdiff * (chunksize * 2 + 1) + ydiff;
}

void WorldMap::MakeTileRoad(TTile* tile)
{
	if (tile == nullptr)
		return;

	if (tile->type != ETileType::Ground)
		return;

	if (tile->object != nullptr && tile->object->GetName().Equals("Road"))
		return;

	PathHandler.AddTile(tile);

	AWorldChunk* chnk = FindChunkWithTile(tile);

	tile->RemoveObject();

	tile->object = new FTileObject_Road();
	tile->object->AffectTile(tile, chnk);


	TTile* ti;
	ti = GetTile(tile->location->x + 1, tile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			ti->object->AffectTile(ti, FindChunkWithTile(ti));
		}
	}

	ti = GetTile(tile->location->x - 1, tile->location->y);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			ti->object->AffectTile(ti, FindChunkWithTile(ti));
		}
	}

	ti = GetTile(tile->location->x, tile->location->y + 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			ti->object->AffectTile(ti, FindChunkWithTile(ti));
		}
	}

	ti = GetTile(tile->location->x, tile->location->y - 1);
	if (ti != nullptr && ti->object != nullptr)
	{
		if (ti->object->GetName().Equals("Road"))
		{
			ti->object->AffectTile(ti, FindChunkWithTile(ti));
		}
	}

}